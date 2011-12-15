################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include fat/subdir.mk
-include fat/option/subdir.mk
-include drivers/uart/subdir.mk
-include drivers/subdir.mk
-include drivers/motor/subdir.mk
-include drivers/lcd/subdir.mk
-include drivers/QTR8RC/subdir.mk
-include System/subdir.mk
-include General/subdir.mk
-include FreeRTOS/subdir.mk
-include FreeRTOS/hooks/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \

SECONDARY_LIST += \

SECONDARY_SIZE += \


# All Target
all: CMPE146FINALPROJECT.elf secondary-outputs

# Tool invocations
CMPE146FINALPROJECT.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Windows GCC C Linker'
	arm-elf-gcc -T"..\lpc2148-rom.ld" -nostartfiles -Wl,-Map,CMPE146FINALPROJECT.map -mcpu=arm7tdmi-s -o"CMPE146FINALPROJECT.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

CMPE146FINALPROJECT.hex: CMPE146FINALPROJECT.elf
	@echo 'Invoking: ARM Windows GNU Create Flash Image'
	arm-elf-objcopy -O ihex CMPE146FINALPROJECT.elf "CMPE146FINALPROJECT.hex"
	@echo 'Finished building: $@'
	@echo ' '

CMPE146FINALPROJECT.lst: CMPE146FINALPROJECT.elf
	@echo 'Invoking: ARM Windows GNU Create Listing'
	arm-elf-objdump -h -S CMPE146FINALPROJECT.elf >"CMPE146FINALPROJECT.lst"
	@echo 'Finished building: $@'
	@echo ' '

CMPE146FINALPROJECT.siz: CMPE146FINALPROJECT.elf
	@echo 'Invoking: ARM Windows GNU Print Size'
	arm-elf-size  --format=berkeley CMPE146FINALPROJECT.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(SECONDARY_SIZE)$(OBJS)$(C_DEPS)$(ASM_DEPS)$(SECONDARY_FLASH)$(EXECUTABLES)$(SECONDARY_LIST)$(S_UPPER_DEPS) CMPE146FINALPROJECT.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
