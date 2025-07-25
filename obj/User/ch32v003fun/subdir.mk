################################################################################
# MRS Version: 1.9.2
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/ch32v003fun/ch32v003fun.c \
../User/ch32v003fun/i2c_tx.c \
../User/ch32v003fun/oled_min.c 

OBJS += \
./User/ch32v003fun/ch32v003fun.o \
./User/ch32v003fun/i2c_tx.o \
./User/ch32v003fun/oled_min.o 

C_DEPS += \
./User/ch32v003fun/ch32v003fun.d \
./User/ch32v003fun/i2c_tx.d \
./User/ch32v003fun/oled_min.d 


# Each subdirectory must supply rules for building sources it contributes
User/ch32v003fun/%.o: ../User/ch32v003fun/%.c
	@	@	riscv-none-embed-gcc -march=rv32ecxw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\Users\azati\OneDrive\Desktop\Snake_game\Snake_game\Core" -I"C:\Users\azati\OneDrive\Desktop\Snake_game\Snake_game\User" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

