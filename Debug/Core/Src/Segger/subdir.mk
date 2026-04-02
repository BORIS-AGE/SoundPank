################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Segger/SEGGER_RTT.c \
../Core/Src/Segger/SEGGER_RTT_printf.c 

OBJS += \
./Core/Src/Segger/SEGGER_RTT.o \
./Core/Src/Segger/SEGGER_RTT_printf.o 

C_DEPS += \
./Core/Src/Segger/SEGGER_RTT.d \
./Core/Src/Segger/SEGGER_RTT_printf.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Segger/%.o Core/Src/Segger/%.su Core/Src/Segger/%.cyclo: ../Core/Src/Segger/%.c Core/Src/Segger/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"E:/progs/STM32/Projects/SoundPunk/Core/Src/Segger" -I"E:/progs/STM32/Projects/SoundPunk/Core/Src/RDA_lib" -I"E:/progs/STM32/Projects/SoundPunk/Core/Src/display" -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"E:/progs/STM32/Projects/SoundPunk/Core/Src/sd_card" -I"E:/progs/STM32/Projects/SoundPunk/Core/Src/sd_card/sd_spi" -I"E:/progs/STM32/Projects/SoundPunk/Core/Src/sd_card/sd_fs" -I"E:/progs/STM32/Projects/SoundPunk/Core/Src/font" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Segger

clean-Core-2f-Src-2f-Segger:
	-$(RM) ./Core/Src/Segger/SEGGER_RTT.cyclo ./Core/Src/Segger/SEGGER_RTT.d ./Core/Src/Segger/SEGGER_RTT.o ./Core/Src/Segger/SEGGER_RTT.su ./Core/Src/Segger/SEGGER_RTT_printf.cyclo ./Core/Src/Segger/SEGGER_RTT_printf.d ./Core/Src/Segger/SEGGER_RTT_printf.o ./Core/Src/Segger/SEGGER_RTT_printf.su

.PHONY: clean-Core-2f-Src-2f-Segger

