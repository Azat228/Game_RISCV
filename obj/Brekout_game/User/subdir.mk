################################################################################
# MRS Version: 1.9.2
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Brekout_game/User/main.c 

OBJS += \
./Brekout_game/User/main.o 

C_DEPS += \
./Brekout_game/User/main.d 


# Each subdirectory must supply rules for building sources it contributes
Brekout_game/User/%.o: ../Brekout_game/User/%.c
	@	@	riscv-none-embed-gcc -march=rv32ecxw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\Users\azati\OneDrive\Desktop\Game_RISCV\Core" -I"C:\Users\azati\OneDrive\Desktop\Game_RISCV\User" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

