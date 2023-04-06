################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../autogen/sl_board_default_init.c \
../autogen/sl_device_init_clocks.c \
../autogen/sl_event_handler.c \
../autogen/sl_simple_led_instances.c 

OBJS += \
./autogen/sl_board_default_init.o \
./autogen/sl_device_init_clocks.o \
./autogen/sl_event_handler.o \
./autogen/sl_simple_led_instances.o 

C_DEPS += \
./autogen/sl_board_default_init.d \
./autogen/sl_device_init_clocks.d \
./autogen/sl_event_handler.d \
./autogen/sl_simple_led_instances.d 


# Each subdirectory must supply rules for building sources it contributes
autogen/sl_board_default_init.o: ../autogen/sl_board_default_init.c autogen/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32PG12B500F1024GL125=1' '-DSL_COMPONENT_CATALOG_PRESENT=1' '-DRTT_USE_ASM=0' '-DSEGGER_RTT_SECTION="SEGGER_RTT"' -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//hardware/kit/SLSTK3402A_EFM32PG12/config" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//hardware/kit/common/drivers" -I"C:\Users\Cyrus\SimplicityStudio\2022_Workspace\Wolfenstein_RTOS\config" -I"C:\Users\Cyrus\SimplicityStudio\2022_Workspace\Wolfenstein_RTOS\autogen" -I"C:\Users\Cyrus\SimplicityStudio\2022_Workspace\Wolfenstein_RTOS" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/Device/SiliconLabs/EFM32PG12B/Include" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/common/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//hardware/board/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/CMSIS/Include" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/CMSIS/RTOS2/Include" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/service/device_init/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/middleware/glib/dmd" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/middleware/glib" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/emlib/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/middleware/glib/glib" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/driver/leddrv/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//hardware/driver/memlcd/src/ls013b7dh03" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//hardware/driver/memlcd/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//hardware/driver/memlcd/inc/memlcd_usart" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/micrium_os/common/source" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/micrium_os/common/include" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/micrium_os/cpu/include" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/micrium_os/ports/source" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/micrium_os" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/micrium_os/kernel/source" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/micrium_os/kernel/include" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//util/third_party/segger/systemview/SEGGER" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//util/third_party/segger/systemview/Sample/MicriumOSKernel" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/common/toolchain/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/service/system/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/service/sleeptimer/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/service/udelay/inc" -Os -Wall -Wextra -fno-builtin -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"autogen/sl_board_default_init.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

autogen/sl_device_init_clocks.o: ../autogen/sl_device_init_clocks.c autogen/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32PG12B500F1024GL125=1' '-DSL_COMPONENT_CATALOG_PRESENT=1' '-DRTT_USE_ASM=0' '-DSEGGER_RTT_SECTION="SEGGER_RTT"' -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//hardware/kit/SLSTK3402A_EFM32PG12/config" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//hardware/kit/common/drivers" -I"C:\Users\Cyrus\SimplicityStudio\2022_Workspace\Wolfenstein_RTOS\config" -I"C:\Users\Cyrus\SimplicityStudio\2022_Workspace\Wolfenstein_RTOS\autogen" -I"C:\Users\Cyrus\SimplicityStudio\2022_Workspace\Wolfenstein_RTOS" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/Device/SiliconLabs/EFM32PG12B/Include" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/common/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//hardware/board/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/CMSIS/Include" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/CMSIS/RTOS2/Include" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/service/device_init/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/middleware/glib/dmd" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/middleware/glib" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/emlib/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/middleware/glib/glib" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/driver/leddrv/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//hardware/driver/memlcd/src/ls013b7dh03" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//hardware/driver/memlcd/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//hardware/driver/memlcd/inc/memlcd_usart" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/micrium_os/common/source" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/micrium_os/common/include" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/micrium_os/cpu/include" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/micrium_os/ports/source" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/micrium_os" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/micrium_os/kernel/source" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/micrium_os/kernel/include" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//util/third_party/segger/systemview/SEGGER" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//util/third_party/segger/systemview/Sample/MicriumOSKernel" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/common/toolchain/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/service/system/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/service/sleeptimer/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/service/udelay/inc" -Os -Wall -Wextra -fno-builtin -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"autogen/sl_device_init_clocks.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

autogen/sl_event_handler.o: ../autogen/sl_event_handler.c autogen/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32PG12B500F1024GL125=1' '-DSL_COMPONENT_CATALOG_PRESENT=1' '-DRTT_USE_ASM=0' '-DSEGGER_RTT_SECTION="SEGGER_RTT"' -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//hardware/kit/SLSTK3402A_EFM32PG12/config" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//hardware/kit/common/drivers" -I"C:\Users\Cyrus\SimplicityStudio\2022_Workspace\Wolfenstein_RTOS\config" -I"C:\Users\Cyrus\SimplicityStudio\2022_Workspace\Wolfenstein_RTOS\autogen" -I"C:\Users\Cyrus\SimplicityStudio\2022_Workspace\Wolfenstein_RTOS" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/Device/SiliconLabs/EFM32PG12B/Include" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/common/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//hardware/board/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/CMSIS/Include" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/CMSIS/RTOS2/Include" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/service/device_init/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/middleware/glib/dmd" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/middleware/glib" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/emlib/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/middleware/glib/glib" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/driver/leddrv/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//hardware/driver/memlcd/src/ls013b7dh03" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//hardware/driver/memlcd/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//hardware/driver/memlcd/inc/memlcd_usart" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/micrium_os/common/source" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/micrium_os/common/include" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/micrium_os/cpu/include" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/micrium_os/ports/source" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/micrium_os" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/micrium_os/kernel/source" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/micrium_os/kernel/include" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//util/third_party/segger/systemview/SEGGER" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//util/third_party/segger/systemview/Sample/MicriumOSKernel" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/common/toolchain/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/service/system/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/service/sleeptimer/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/service/udelay/inc" -Os -Wall -Wextra -fno-builtin -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"autogen/sl_event_handler.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

autogen/sl_simple_led_instances.o: ../autogen/sl_simple_led_instances.c autogen/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32PG12B500F1024GL125=1' '-DSL_COMPONENT_CATALOG_PRESENT=1' '-DRTT_USE_ASM=0' '-DSEGGER_RTT_SECTION="SEGGER_RTT"' -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//hardware/kit/SLSTK3402A_EFM32PG12/config" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//hardware/kit/common/drivers" -I"C:\Users\Cyrus\SimplicityStudio\2022_Workspace\Wolfenstein_RTOS\config" -I"C:\Users\Cyrus\SimplicityStudio\2022_Workspace\Wolfenstein_RTOS\autogen" -I"C:\Users\Cyrus\SimplicityStudio\2022_Workspace\Wolfenstein_RTOS" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/Device/SiliconLabs/EFM32PG12B/Include" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/common/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//hardware/board/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/CMSIS/Include" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/CMSIS/RTOS2/Include" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/service/device_init/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/middleware/glib/dmd" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/middleware/glib" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/emlib/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/middleware/glib/glib" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/driver/leddrv/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//hardware/driver/memlcd/src/ls013b7dh03" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//hardware/driver/memlcd/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//hardware/driver/memlcd/inc/memlcd_usart" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/micrium_os/common/source" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/micrium_os/common/include" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/micrium_os/cpu/include" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/micrium_os/ports/source" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/micrium_os" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/micrium_os/kernel/source" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/micrium_os/kernel/include" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//util/third_party/segger/systemview/SEGGER" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//util/third_party/segger/systemview/Sample/MicriumOSKernel" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/common/toolchain/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/service/system/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/service/sleeptimer/inc" -I"C:/SiliconLabs/SimplicityStudio/v5_2/developer/sdks/gecko_sdk_suite/v3.2//platform/service/udelay/inc" -Os -Wall -Wextra -fno-builtin -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"autogen/sl_simple_led_instances.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


