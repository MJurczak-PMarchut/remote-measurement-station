################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/ST/BlueNRG-MS/hci/hci_tl_patterns/Basic/hci_tl.c 

OBJS += \
./Middlewares/ST/BlueNRG-MS/hci/hci_tl_patterns/Basic/hci_tl.o 

C_DEPS += \
./Middlewares/ST/BlueNRG-MS/hci/hci_tl_patterns/Basic/hci_tl.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/ST/BlueNRG-MS/hci/hci_tl_patterns/Basic/hci_tl.o: ../Middlewares/ST/BlueNRG-MS/hci/hci_tl_patterns/Basic/hci_tl.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../Middlewares/ST/BlueNRG-MS/hci/hci_tl_patterns/Basic -I../Middlewares/ST/BlueNRG-MS/includes -I../BlueNRG-MS/App -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../BlueNRG-MS/Target -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/BSP/STM32F4xx_Nucleo -I../Middlewares/ST/BlueNRG-MS/utils -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/ST/BlueNRG-MS/hci/hci_tl_patterns/Basic/hci_tl.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

