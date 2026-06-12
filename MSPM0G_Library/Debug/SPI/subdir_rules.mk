################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
SPI/%.o: ../SPI/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccstheia151/ccs/tools/compiler/ti-cgt-armllvm_4.0.0.LTS/bin/tiarmclang.exe" -c -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"D:/Metron_firmware/MSPM0G_Library" -I"D:/Metron_firmware/MSPM0G_Library/Debug" -I"C:/ti/mspm0_sdk_2_00_00_03/source/third_party/CMSIS/Core/Include" -I"C:/ti/mspm0_sdk_2_00_00_03/source" -I"D:/Metron_firmware/MSPM0G_Library/Include" -D__MSPM0G3507__ -gdwarf-3 -MMD -MP -MF"SPI/$(basename $(<F)).d_raw" -MT"$(@)" -I"D:/Metron_firmware/MSPM0G_Library/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


