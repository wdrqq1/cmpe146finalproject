################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/QTR8RC/QTR8RC.c 

OBJS += \
./drivers/QTR8RC/QTR8RC.o 

C_DEPS += \
./drivers/QTR8RC/QTR8RC.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/QTR8RC/%.o: ../drivers/QTR8RC/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Windows GCC C Compiler'
	arm-elf-gcc -Os -Wall -std=gnu99 -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=arm7tdmi-s -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


