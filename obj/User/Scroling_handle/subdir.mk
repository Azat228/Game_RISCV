################################################################################
# MRS Version: 1.9.2
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/Scroling_handle/scroling_text.c 

OBJS += \
./User/Scroling_handle/scroling_text.o 

C_DEPS += \
./User/Scroling_handle/scroling_text.d 


# Each subdirectory must supply rules for building sources it contributes
User/Scroling_handle/%.o: ../User/Scroling_handle/%.c
	@	@	riscv-none-embed-gcc -march=rv32ecxw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\Users\azati\OneDrive\Desktop\Game_RISCV-main\Game_RISCV-main\Core" -I"C:\Users\azati\OneDrive\Desktop\Game_RISCV-main\Game_RISCV-main\User" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

