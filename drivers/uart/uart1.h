//
//  $Id: uart1.h 110 2008-10-11 00:21:58Z jcw $
//  $Revision: 110 $
//  $Author: jcw $
//  $Date: 2008-10-10 20:21:58 -0400 (Fri, 10 Oct 2008) $
//  $HeadURL: http://tinymicros.com/svn_public/arm/lpc2148_demo/trunk/uart/uart1.h $
//

#ifndef _uart1_H_
#define _uart1_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "../../FreeRTOS/FreeRTOS.h"
#include "../../FreeRTOS/queue.h"
#include "../../FreeRTOS/task.h"

void uart1ISR (void);

char uart1Init (unsigned long ulWantedBaud, unsigned long uxQueueLength);
void uart1Deinit (void);

unsigned long uart1PutCharPolling (char cOutChar, unsigned long c);
unsigned long uart1GetChar (char *pcRxedChar, portTickType xBlockTime);
unsigned long uart1PutChar (char cOutChar, portTickType xBlockTime);


typedef struct S_uart1
{
	union {
		/* 0xE001 0000 */unsigned long inputByte;
		/* 0xE001 0000 */unsigned long outputByte;
		/* 0xE001 0000 */unsigned long divisorLSB;	///< accessDivisors must be 1 to access this.
	};

	union {
		/* 0xE001 0004 */unsigned long divisorMSB;	///< accessDivisors must be 1 to access this.
		/* 0xE001 0004 */
		struct {	/* RW */
			unsigned long rxInterruptEnable:1;
			unsigned long txRegisterEmptyInterruptEnable:1;
			unsigned long rxLineStatusInterruptEnable:1;
			unsigned long reserved1:5;
			unsigned long autoBaudTimeoutInterruptEnable:1;
			unsigned long endOfAutoBaudInterruptEnable:1;
			unsigned long reserved2:22;
		}interrupt;
	};

	/* 0xE001 0008 */
	union {
		struct {	/* RO */
			unsigned long isInterruptPending:1;
			unsigned long interruptIdentification:3; ///< 1=THRE, 2=Rx, 3=Line Status, 6=Character Timeout
			unsigned long reserved3:2;
			unsigned long isFIFOEnabled:2;
			unsigned long isAutoBaudFinished:1;
			unsigned long isAutoBaudTimedOut:1;
			unsigned long reserved4:22;
		};
		struct {	/* WO */
			unsigned char FIFOEnable:1;
			unsigned char rxFIFOReset:1;
			unsigned char txFIFOReset:1;
			unsigned char reserved5:3;
			unsigned char rxTriggerLevel:2;
		};
	}fifo;

	/* 0xE001 000C */
	struct {
		unsigned long wordSelect:2; ///< 0=5bit, 1=6bit, 2=7bit, 3=8bit
		unsigned long twoStopBits:1;
		unsigned long parityEnable:1;
		unsigned long paritySelect:2; ///< 0=Odd, 1=Even, 2=Always 1, 3=Always 0
		unsigned long breakControl:1;
		unsigned long accessDivisors:1;
		unsigned long reserved6:24;
	}config;

	/* 0xE001 0010 */ unsigned long reserved7;

	/* 0xE001 0014 */
	struct {
		unsigned long newInputData:1;
		unsigned long dataOverrun:1;
		unsigned long parityError:1;
		unsigned long framingError:1;
		unsigned long breakInterrupt:1;
		unsigned long noOutputData:1;
		unsigned long transmitterEmpty:1;
		unsigned long rxFIFOError:1;
		unsigned long reserved8:24;
	}status;

	/* 0xE001 0018 */ unsigned long reserved9;
	/* 0xE001 001C */ unsigned long scratchPad8Bit;

	/* 0xE001 0020 */
	struct {
		unsigned long start:1;
		unsigned long mode:1;
		unsigned long autoRestart:1;
		unsigned long reserved:5;
		unsigned long clearEndOfAutoBaudInterrupt:1;
		unsigned long clearAutoBaudTimeoutInterrupt:1;
		unsigned long reserved10:22;
	}autobaud;

	/* 0xE001 0024 */ unsigned long reserved11;

	/* 0xE001 0028 */
	struct{
		unsigned long divisorValue4Bit:4;
		unsigned long multiplierValue4Bit:4;
		unsigned long reserved11:24;
	}fractionalDivider;

	/* 0xE001 002C */ unsigned long reserved12;

	/* 0xE001 0030 */
	struct{
		unsigned long reserved13:7;
		unsigned long continueTx:1;	///< This is 1 after RESET. Write 0 to pause Transmitter
		unsigned long reserved14:24;
	};

}S_uart1;
#define uart1   (*(S_uart1 *) 0xE0010000 )

#ifdef __cplusplus
}
#endif
#endif
