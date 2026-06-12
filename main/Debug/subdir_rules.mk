################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccstheia151/ccs/tools/compiler/ti-cgt-armllvm_4.0.0.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"D:/Metron_firmware/main" -I"D:/Metron_firmware/MSPM0G_Library/Include" -I"D:/Metron_firmware/main/Debug" -I"C:/ti/mspm0_sdk_2_00_00_03/source/third_party/CMSIS/Core/Include" -I"C:/ti/mspm0_sdk_2_00_00_03/source" -gdwarf-3 -Wall -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-1372189215: ../empty.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"C:/ti/sysconfig_1.19.0/sysconfig_cli.bat" --script "D:/Metron_firmware/main/empty.syscfg" -o "." -s "C:/ti/mspm0_sdk_2_00_00_03/.metadata/product.json" --compiler ticlang
	@echo 'Finished building: "$<"'
	@echo ' '

device_linker.cmd: build-1372189215 ../empty.syscfg
device.opt: build-1372189215
device.cmd.genlibs: build-1372189215
ti_msp_dl_config.c: build-1372189215
ti_msp_dl_config.h: build-1372189215
Event.dot: build-1372189215

%.o: ./%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccstheia151/ccs/tools/compiler/ti-cgt-armllvm_4.0.0.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"D:/Metron_firmware/main" -I"D:/Metron_firmware/MSPM0G_Library/Include" -I"D:/Metron_firmware/main/Debug" -I"C:/ti/mspm0_sdk_2_00_00_03/source/third_party/CMSIS/Core/Include" -I"C:/ti/mspm0_sdk_2_00_00_03/source" -gdwarf-3 -Wall -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

startup_mspm0g350x_ticlang.o: C:/ti/mspm0_sdk_2_00_00_03/source/ti/devices/msp/m0p/startup_system_files/ticlang/startup_mspm0g350x_ticlang.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccstheia151/ccs/tools/compiler/ti-cgt-armllvm_4.0.0.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"D:/Metron_firmware/main" -I"D:/Metron_firmware/MSPM0G_Library/Include" -I"D:/Metron_firmware/main/Debug" -I"C:/ti/mspm0_sdk_2_00_00_03/source/third_party/CMSIS/Core/Include" -I"C:/ti/mspm0_sdk_2_00_00_03/source" -gdwarf-3 -Wall -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


