################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/lcd/hd44780.c \
../drivers/lcd/lcd.c 

OBJS += \
./drivers/lcd/hd44780.o \
./drivers/lcd/lcd.o 

C_DEPS += \
./drivers/lcd/hd44780.d \
./drivers/lcd/lcd.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/lcd/%.o: ../drivers/lcd/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Windows GCC C Compiler'
	arm-elf-gcc -Os -Wall -std=gnu99 -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=arm7tdmi-s -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


