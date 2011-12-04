
#include "./osHandles.h"                // Includes all OS Files
#include "./System/cpu.h"               // Initialize CPU Hardware

#include "./System/crash.h"             // Detect exception (Reset)
#include "./System/watchdog.h"
#include "./drivers/uart/uart0.h"       // Initialize UART
#include "./System/rprintf.h"			// Reduced printf.  STDIO uses lot of FLASH space & Stack space
#include "./general/userInterface.h"	// User interface functions to interact through UART

//Need these to read files
#include "./fat/diskio.h"
#include "./fat/ff.h"
#include "./drivers/sta013.h"
#include "./drivers/ssp_spi.h"
#include "./drivers/pcm1774.h"

//Need the functions from here for Buttons/LEDs
#include "./drivers/i2c.h"

//Standard includes
#include "string.h"
#include <stdbool.h>



//GLOBALS
int lastFileIndex;
int currentSong;
char songArray[30][15];
char *nowPlaying;
int paused;
char command;

//Additions: Gets rid of "magic" port expander register numbers
#define SWREG  0x01
#define LEDREG 0x02

int outputVol;
/*pcm1774_OutputVolume(outputVol, outputVol);
pcm1774_DigitalVolume(vol, vo1);*/

/* INTERRUPT VECTORS:
 * 0:    OS Timer Tick
 * 1:    Not Used
 * 2:    UART Interrupt
 * 3:    Not Used
 * 4:    I2C
 * 5-16: Not Used
 */

/* Resources Used
 * 1.  TIMER0 FOR OS Interrupt
 * 2.  TIMER1 FOR Run-time Stats (can be disabled at FreeRTOSConfig.h)
 * 3.  Watchdog for timed delay functions (No CPU Reset or timer)
 */
#include "./fat/diskio.h"
void diskTimer()
{
	for(;;)
	{
		vTaskDelay(10);
		disk_timerproc();
	}
}


void initMotor(void *pvParameters)
{

}

void initPE(void)
{
	//OSHANDLES * handles = (OSHANDLES*)v;
	//rprintf("Initializing Port Expander...\n");

	//i2cWrite(0x40, 0x07, init, 2); //Command configuration register write: Port 0 is Output
	// and Command configuration register write: Port 1 is Input simultaneously

	i2cWriteDeviceRegister(0x40, 0x06, 0x00);
	i2cWriteDeviceRegister(0x40, 0x07, 0xFF);

	//rprintf("initPE complete!\n");

	i2cWriteDeviceRegister(0x40, LEDREG, 0xFF);
	vTaskDelay(50);
	i2cWriteDeviceRegister(0x40, LEDREG, 0x00);
}

void mp3(void *pvParameters)
{
	OSHANDLES *osHandles = (OSHANDLES*)pvParameters;

	char songname[15];
	paused = 0;
	bool broken = false;

	while(1)
	{
		if(xQueueReceive(osHandles->queue.songname, &songname[0], portMAX_DELAY))
		{
			nowPlaying = songname;
			rprintf("%c\n", songname[0]);
			rprintf("%c\n", songname[1]);
			rprintf("%c\n", songname[2]);
			rprintf("%c\n", songname[3]);
			rprintf("%c\n", songname[4]);
			rprintf("%c\n\n", songname[5]);
			rprintf("Song to play: %s\n", songname);
			strcat(songname, ".mp3");
			rprintf("Received: %s", songname);
			FIL fileHandle;
			BYTE buffer[2048];

			if(FR_OK == f_open(&fileHandle, songname, FA_OPEN_EXISTING | FA_READ))
			{
				rprintf("File opened, about to play:\n");
				int numOfReadBytes = sizeof(buffer);
				while(numOfReadBytes == sizeof(buffer))
				{
					if(FR_OK == f_read(&fileHandle, buffer, sizeof(buffer), &numOfReadBytes))
					{
						if(xSemaphoreTake(osHandles->lock.SPI, 1000))
						{
							//rprintf("SPI Semaphore taken!\n");
							SELECT_MP3_CS();
							int i = 0;
							while( i < numOfReadBytes)
							{
								if( STA013_NEEDS_DATA())
								{

									rxTxByteSSPSPI(buffer[i++]);
								}
								else
								{
									vTaskDelay(1);
								}
								/*while (paused == 1)
									vTaskDelay(1);*/
								/*if (xQueueReceive(osHandles->queue.command, &command, portMAX_DELAY))
								{
									f_close(&fileHandle);
									break;
									broken = true;
								}*/
								if (command  != 0)
								{
									DESELECT_MP3_CS();
									xSemaphoreGive(osHandles->lock.SPI);
									rprintf("Semaphore given back--interrupted with a command.\n");
									f_close(&fileHandle);
									command = 0;
								}
							}
							DESELECT_MP3_CS();
							xSemaphoreGive(osHandles->lock.SPI);
							//rprintf("Semaphore given back.\n");
						}
						else
						{
							rprintf("Error taking SPI semaphore while playing mp3\n");
						}
					}
					/*if (broken)
					{
						broken = false;
						break;
					}*/
				}
				f_close(&fileHandle);
			}
			else
			{
				rprintf("Couldn't open file!\n");
			}
		}
	}
}

void popSongs(void *pvParameters)
{
	OSHANDLES *osHandles = (OSHANDLES*)pvParameters;

	char fullName[15];
	char *file;
	int counter = 0;
	char *fileExtension;
	int i = 0;
	int j = 0;
	char mp3[4] = "mp3";

	// Error check if SPI Lock doesn't exist.
	if(0 == osHandles->lock.SPI)
	{
		rprintf("You did not create an SPI Mutex\n");
		while(1);
	}

	initialize_SSPSPI();
	initialize_I2C0(100*1000);

	diskio_initializeSPIMutex(&(osHandles->lock.SPI));
	initialize_SdCardSignals();
	initialize_sta013();
	initialize_pcm1774();

	FATFS SDCard;        // Takes ~550 Bytes
	if(FR_OK != f_mount(0, &SDCard)) { // Mount the Card on the File System
		rprintf("WARNING: SD CARD Could not be mounted!\n");
	}

	DIR Dir;
	FILINFO Finfo;
	FATFS *fs;
	FRESULT returnCode = FR_OK;

	unsigned int fileBytesTotal, numFiles, numDirs;
	fileBytesTotal = numFiles = numDirs = 0;
	#if _USE_LFN
	char Lfname[512];
	#endif

	char dirPath[] = "0:";
	if (RES_OK != (returnCode = f_opendir(&Dir, dirPath))) {
		rprintf("Invalid directory: |%s|\n", dirPath);
		return;
	}

	rprintf("Directory listing of: %s\n\n", dirPath);
	for (j = 0; j < 100; j ++)
	{
		#if _USE_LFN
		Finfo.lfname = Lfname;
		Finfo.lfsize = sizeof(Lfname);
		#endif

		char returnCode = f_readdir(&Dir, &Finfo);
		if ( (FR_OK != returnCode) || !Finfo.fname[0])
			break;
		if (Finfo.fattrib & AM_DIR)
		{
			numDirs++;
		}
		else
		{
			numFiles++;
			fileBytesTotal += Finfo.fsize;
		}
		strcpy (fullName, Finfo.fname);
		rprintf("File Name found: %s.\n", fullName);
		char *temp = fullName;
		file = strtok(temp, ".");
		fileExtension = strtok(NULL, NULL);
		rprintf("File Extension was: %s\n", fileExtension);
		//if (strcmp(fileExtension, mp3))
		//{
			while (file[i] != NULL)
			{
				rprintf("Copying File[%d]: %c\n", i, file[i]);

				songArray[counter][i] = file[i];
				i++;
				//rprintf("Received songArray[%d][%d]: %c\n", counter, i, songArray[counter][i]);
			}
			songArray[counter][i+1] = NULL;
			i = 0;
			rprintf("just populated song: %s\n", songArray[counter]);
			//strcpy (songArray[counter], file);
			if (nowPlaying == Finfo.fname)
			{
				currentSong = counter;
			}
			//counter++;
		//}

		lastFileIndex = counter;
		counter++;

		rprintf("lastFileIndex = %d\n", lastFileIndex);
		//don't overflow the song array!
		if (counter + 1 > 30)
			break;
	}
	for (i = 0; i <= lastFileIndex; i++)
	{
		rprintf("songArray[%d] = %s\n", i, songArray[i]);
	}
	xSemaphoreGive(osHandles->lock.SPI);
	rprintf("Exiting popSongs\n");
}

void getSong(char c, char *fileName)
{
	vTaskDelay(100);
	if (c == 'S')
	{
		if (currentSong + 1 > lastFileIndex)
		{
			currentSong = 0;
		}
		else
		{
			currentSong++;
		}
		//fileName = songArray[currentSong];
		strcpy(fileName, songArray[currentSong]);
	}
	else if (c == 'P')
	{
		if (currentSong - 1 < 0)
		{
			currentSong = lastFileIndex;
		}
		else
		{
			currentSong--;
		}
		//fileName = songArray[currentSong];
		strcpy(fileName, songArray[currentSong]);
	}
	else if (c == 'C')
	{
		//fileName = songArray[currentSong];
		strcpy(fileName, songArray[currentSong]);
	}
	else if (c != NULL)
	{
		fileName = "ERROR!";
	}
	rprintf("Sent Along: songArray[%d] = %s =? %s!\n", currentSong, songArray[currentSong], fileName);
}

void i2cTimer(void *pvParameters)
{
	OSHANDLES *osHandles = (OSHANDLES*)pvParameters;

	char returnData;
	char songName[15]; //or char *?
	//char command;
/*
	initialize_SSPSPI();
	initialize_I2C0(100*1000);
//Experimental

	diskio_initializeSPIMutex(&(osHandles->lock.SPI));
	initialize_SdCardSignals();
	initialize_sta013();
	initialize_pcm1774();

	//Additions
	//i2cInit(handles);
	initPE();
	//initialize_I2C0(400000);
	outputVol = 50;
*/
	popSongs(osHandles);
	currentSong = 0;
	outputVol = 50;

	vTaskDelay(50);

	//THIS WORKS!
	/*command = 'C';
	getSong(command, songName);
	if(xQueueSend(osHandles->queue.songname, &songName, 100))
	{
		rprintf("Sent first song name: %s!\n", songName);
	}
	else
	{
		rprintf("Timeout during Send!!!\n");
	}*/

	command = 0;

	while(1)
	{
		vTaskDelay(200);
		returnData = i2cReadDeviceRegister(0x40, SWREG);
		i2cWriteDeviceRegister(0x40, LEDREG, returnData);
		if (returnData != 0x00)
		{
			switch(returnData)
			{
			case 0x01:
				if(outputVol>0)
				{
					outputVol--;
					pcm1774_OutputVolume(outputVol, outputVol);
					//pcm1774_DigitalVolume(vol, vo1);
					rprintf("Volume Down\n");
				}
						break;
			case 0x02:
				if(outputVol<100)
				{
					outputVol++;
					pcm1774_OutputVolume(outputVol, outputVol);
					//pcm1774_DigitalVolume(vol, vo1);
					rprintf("Volume Up\n");
				}
						break;
			case 0x04:
						//No function
						break;
			case 0x08:
						//No function
						break;
			case 0x10:
						command = 'S';
						if(xQueueSend(osHandles->queue.command, &command, 100))
						{
							rprintf("Sent Skip!\n");
						}
						else
						{
							rprintf("Timeout during Send!!!\n");
						}
						vTaskDelay(50);
						getSong(command, songName);
						if(xQueueSend(osHandles->queue.songname, &songName, 100))
						{
							rprintf("Sent next song name: %s!\n", songName);
						}
						else
						{
							rprintf("Timeout during Send!!!\n");
						}
						break;
			case 0x20:
					if (paused == 1)
					{
						paused = 0;
						sta013ResumeDecoder();
					}
					else if (paused == 0)
					{
						paused = 1;
						sta013PauseDecoder();
					}
					rprintf("Pause Button\n");
						break;
			case 0x40:
					command = 'C';
					if(xQueueSend(osHandles->queue.command, &command, 100))
					{
						rprintf("Sent Play!\n");
					}
					else
					{
						rprintf("Timeout during Send!!!\n");
					}
					vTaskDelay(50);
					getSong(command, songName);
					command = 0;
					if(xQueueSend(osHandles->queue.songname, &songName, 100))
					{
						rprintf("Sent current song name: %s!\n", songName);
					}
					else
					{
						rprintf("Timeout during Send!!!\n");
					}
						break;
			case 0x80:
						command = 'P';
						if(xQueueSend(osHandles->queue.command, &command, 100))
						{
							rprintf("Sent Previous!\n");
						}
						else
						{
							rprintf("Timeout during Send!!!\n");
						}
						vTaskDelay(50);
						getSong(command, songName);
						if(xQueueSend(osHandles->queue.songname, &songName, 100))
						{
							rprintf("Sent Previous song name: %s!\n", songName);
						}
						else
						{
							rprintf("Timeout during Send!!!\n");
						}
						break;
			default:
						rprintf("One button at a time, please...\n");
						break;
			}
		}
	}
}

int main (void)
{
	OSHANDLES System;            // Should contain all OS Handles

	cpuSetupHardware();          // Setup PLL, enable MAM etc.
	watchdogDelayUs(2000*1000);  // Some startup delay
	uart0Init(38400, 256);       // 256 is size of Rx/Tx FIFO





	// Use polling version of uart0 to do printf/rprintf before starting FreeRTOS
	rprintf_devopen(uart0PutCharPolling);
	if(didSystemCrash())
	{
		printCrashInfo();
		clearCrashInfo();
	}
	cpuPrintMemoryInfo();

	// Open interrupt-driven version of UART0 Rx/Tx
	rprintf_devopen(uart0PutChar);

	uart1PutChar(0xAA); // initialize motor


	System.lock.SPI = xSemaphoreCreateMutex();
	System.queue.songname = xQueueCreate(1,15);
	System.queue.command = xQueueCreate(1,1);

	// Use the WATERMARK command to determine optimal Stack size of each task (set to high, then slowly decrease)
	// Priorities should be set according to response required
	if (
		// User Interaction set to lowest priority.
		//pdPASS != xTaskCreate( uartUI, (signed char*)"Uart UI", STACK_BYTES(1024*6), &System, PRIORITY_LOW,  &System.task.userInterface )
		//||
		// diskTimer should always run, and it is a short function, so assign CRITIAL priority.
		pdPASS != xTaskCreate( i2cTimer, (signed char*)"i2cTimer", STACK_BYTES(1024*4), &System, PRIORITY_LOW,  &System.task.i2cTimer )
		||
		pdPASS != xTaskCreate( diskTimer, (signed char*)"Dtimer", STACK_BYTES(256), &System, PRIORITY_CRITICAL,  &System.task.diskTimer )
		||
		pdPASS != xTaskCreate( mp3, (signed char*)"mp3", STACK_BYTES(1024*4), &System, PRIORITY_HIGH,  &System.task.mp3 )

	){
		rprintf("ERROR:  OUT OF MEMORY: Check OS Stack Size or task stack size.\n");
	}

	rprintf("\n-- Starting FreeRTOS --\n");
	vTaskStartScheduler();	// This function will not return.

	rprintf_devopen(uart0PutCharPolling);
	rprintf("ERROR: Unexpected OS Exit!\n");
	return 0;
}

