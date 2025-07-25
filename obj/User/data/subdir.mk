################################################################################
# MRS Version: 1.9.2
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/data/app.c \
../User/data/memory.c 

OBJS += \
./User/data/app.o \
./User/data/memory.o 

C_DEPS += \
./User/data/app.d \
./User/data/memory.d 


# Each subdirectory must supply rules for building sources it contributes
User/data/%.o: ../User/data/%.c
	@	@	riscv-none-embed-gcc -march=rv32ecxw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"D:\Alex Tong\InspireLab\Projects\RISCV\code\inspirematrix test\InspireRV3\Debug" -I"D:\Alex Tong\InspireLab\Projects\RISCV\code\inspirematrix test\InspireRV3\Core" -I"D:\Alex Tong\InspireLab\Projects\RISCV\code\inspirematrix test\InspireRV3\User" -I"D:\Alex Tong\InspireLab\Projects\RISCV\code\inspirematrix test\InspireRV3\Peripheral\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

