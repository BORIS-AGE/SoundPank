################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/font/font.c 

OBJS += \
./Core/Src/font/font.o 

C_DEPS += \
./Core/Src/font/font.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/font/%.o Core/Src/font/%.su Core/Src/font/%.cyclo: ../Core/Src/font/%.c Core/Src/font/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"E:/progs/STM32/Projects/SoundPunk/Core/Src/Segger" -I"E:/progs/STM32/Projects/SoundPunk/Core/Src/RDA_lib" -I"E:/progs/STM32/Projects/SoundPunk/Core/Src/display" -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"E:/progs/STM32/Projects/SoundPunk/Core/Src/sd_card" -I"E:/progs/STM32/Projects/SoundPunk/Core/Src/sd_card/sd_spi" -I"E:/progs/STM32/Projects/SoundPunk/Core/Src/sd_card/sd_fs" -I"E:/progs/STM32/Projects/SoundPunk/Core/Src/font" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-font

clean-Core-2f-Src-2f-font:
	-$(RM) ./Core/Src/font/font.cyclo ./Core/Src/font/font.d ./Core/Src/font/font.o ./Core/Src/font/font.su

.PHONY: clean-Core-2f-Src-2f-font

