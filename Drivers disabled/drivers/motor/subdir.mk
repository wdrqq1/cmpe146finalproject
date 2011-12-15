################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/motor/QIK_2S9V1.c 

OBJS += \
./drivers/motor/QIK_2S9V1.o 

C_DEPS += \
./drivers/motor/QIK_2S9V1.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/motor/%.o: ../drivers/motor/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Windows GCC C Compiler'
	arm-elf-gcc -I"C:\Users\tony_sid\Desktop\CmpE146Package\workspace\CMPE146FINALPROJECT\drivers" -Os -Wall -std=gnu99 -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=arm7tdmi-s -mthumb -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


