################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_gap_aci.c \
../Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_gatt_aci.c \
../Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_hal_aci.c \
../Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_l2cap_aci.c \
../Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_updater_aci.c \
../Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_utils_small.c 

OBJS += \
./Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_gap_aci.o \
./Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_gatt_aci.o \
./Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_hal_aci.o \
./Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_l2cap_aci.o \
./Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_updater_aci.o \
./Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_utils_small.o 

C_DEPS += \
./Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_gap_aci.d \
./Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_gatt_aci.d \
./Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_hal_aci.d \
./Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_l2cap_aci.d \
./Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_updater_aci.d \
./Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_utils_small.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_gap_aci.o: ../Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_gap_aci.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../Middlewares/ST/BlueNRG-MS/hci/hci_tl_patterns/Basic -I../Middlewares/ST/BlueNRG-MS/includes -I../BlueNRG-MS/App -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../BlueNRG-MS/Target -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/BSP/STM32F4xx_Nucleo -I../Middlewares/ST/BlueNRG-MS/utils -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_gap_aci.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_gatt_aci.o: ../Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_gatt_aci.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../Middlewares/ST/BlueNRG-MS/hci/hci_tl_patterns/Basic -I../Middlewares/ST/BlueNRG-MS/includes -I../BlueNRG-MS/App -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../BlueNRG-MS/Target -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/BSP/STM32F4xx_Nucleo -I../Middlewares/ST/BlueNRG-MS/utils -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_gatt_aci.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_hal_aci.o: ../Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_hal_aci.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../Middlewares/ST/BlueNRG-MS/hci/hci_tl_patterns/Basic -I../Middlewares/ST/BlueNRG-MS/includes -I../BlueNRG-MS/App -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../BlueNRG-MS/Target -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/BSP/STM32F4xx_Nucleo -I../Middlewares/ST/BlueNRG-MS/utils -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_hal_aci.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_l2cap_aci.o: ../Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_l2cap_aci.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../Middlewares/ST/BlueNRG-MS/hci/hci_tl_patterns/Basic -I../Middlewares/ST/BlueNRG-MS/includes -I../BlueNRG-MS/App -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../BlueNRG-MS/Target -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/BSP/STM32F4xx_Nucleo -I../Middlewares/ST/BlueNRG-MS/utils -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_l2cap_aci.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_updater_aci.o: ../Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_updater_aci.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../Middlewares/ST/BlueNRG-MS/hci/hci_tl_patterns/Basic -I../Middlewares/ST/BlueNRG-MS/includes -I../BlueNRG-MS/App -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../BlueNRG-MS/Target -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/BSP/STM32F4xx_Nucleo -I../Middlewares/ST/BlueNRG-MS/utils -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_updater_aci.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_utils_small.o: ../Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_utils_small.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../Middlewares/ST/BlueNRG-MS/hci/hci_tl_patterns/Basic -I../Middlewares/ST/BlueNRG-MS/includes -I../BlueNRG-MS/App -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../BlueNRG-MS/Target -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/BSP/STM32F4xx_Nucleo -I../Middlewares/ST/BlueNRG-MS/utils -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_utils_small.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

