################################################################################
# MRS Version: 1.9.2
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/Game_help_func/Name_func.c \
../User/Game_help_func/eeprom.c 

OBJS += \
./User/Game_help_func/Name_func.o \
./User/Game_help_func/eeprom.o 

C_DEPS += \
./User/Game_help_func/Name_func.d \
./User/Game_help_func/eeprom.d 


# Each subdirectory must supply rules for building sources it contributes
User/Game_help_func/%.o: ../User/Game_help_func/%.c
	@	@	riscv-none-embed-gcc -march=rv32ecxw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\Users\azati\OneDrive\Desktop\Game_RISCV-main\Game_RISCV-main\Core" -I"C:\Users\azati\OneDrive\Desktop\Game_RISCV-main\Game_RISCV-main\User" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

