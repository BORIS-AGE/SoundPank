################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/sd_card/diskio/diskio.c 

OBJS += \
./Core/Src/sd_card/diskio/diskio.o 

C_DEPS += \
./Core/Src/sd_card/diskio/diskio.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/sd_card/diskio/%.o Core/Src/sd_card/diskio/%.su Core/Src/sd_card/diskio/%.cyclo: ../Core/Src/sd_card/diskio/%.c Core/Src/sd_card/diskio/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"E:/progs/STM32/Projects/SoundPunk/Core/Src/Segger" -I"E:/progs/STM32/Projects/SoundPunk/Core/Src/RDA_lib" -I"E:/progs/STM32/Projects/SoundPunk/Core/Src/display" -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"E:/progs/STM32/Projects/SoundPunk/Core/Src/sd_card" -I"E:/progs/STM32/Projects/SoundPunk/Core/Src/sd_card/sd_spi" -I"E:/progs/STM32/Projects/SoundPunk/Core/Src/sd_card/sd_fs" -I"E:/progs/STM32/Projects/SoundPunk/Core/Src/font" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-sd_card-2f-diskio

clean-Core-2f-Src-2f-sd_card-2f-diskio:
	-$(RM) ./Core/Src/sd_card/diskio/diskio.cyclo ./Core/Src/sd_card/diskio/diskio.d ./Core/Src/sd_card/diskio/diskio.o ./Core/Src/sd_card/diskio/diskio.su

.PHONY: clean-Core-2f-Src-2f-sd_card-2f-diskio

