# Toolchain
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)as
LD = $(PREFIX)ld
OBJCOPY = $(PREFIX)objcopy
ADDR2 = $(PREFIX)addr2line
#OBJDUMP = $(PREFIX)objdump
SIZE = $(PREFIX)size
DEBUG = 1

BUILD_TIME := $(shell date +%Y%m%d__%H%M%S)

# Thư mục dự án
SRC_DIR = \
	Core/Src \
	Core/Src/DMA \
	D:/STM32_WORKSPACE/STM32F411_Myproject/Drivers/STM32F411VE_Driver/Src/stm32f4xx_hal_cortex.c \
	D:/STM32_WORKSPACE/STM32F411_Myproject/Drivers/STM32F411VE_Driver/Src/stm32f4xx_hal.c \
	Drivers/STM32F411VE_Driver/Src \
	Core/lib

INC_DIR = \
	Core/Inc \
	Drivers/Core/ \
	Drivers/STM32F411VE_Driver/Inc \
	Drivers/Core/Inc \

BUILD_DIR = build

# Linker script
LINKER_SCRIPT = STM32F411VETx_FLASH.ld

# Cờ biên dịch
CPU = -mcpu=cortex-m4
FPU = -mfpu=fpv4-sp-d16 -mfloat-abi=hard
CFLAGS = $(CPU) -mthumb $(FPU) \
         -Wall -Wextra \
         -O0 -g \
         $(foreach dir,$(INC_DIR),-I$(dir)) \
         -std=gnu11

semihosting = 0

# Cờ liên kết
LDFLAGS = $(CPU) -mthumb $(FPU) \
          -T$(LINKER_SCRIPT) \
          -Wl,-Map=$(BUILD_DIR)/output.map

ifeq ($(semihosting),0)
LDFLAGS += -nostdlib -specs=nano.specs -lgcc
else
LDFLAGS += -specs=rdimon.specs -lc -lrdimon
endif

# Tìm tất cả các file nguồn
SRCS = $(foreach dir,$(SRC_DIR),$(wildcard $(dir)/*.c))
SRCS += startup_stm32f411xe.c
# Nếu dùng semihosting, không cần syscalls.c
ifeq ($(semihosting),1)
SRCS := $(filter-out %syscalls.c, $(SRCS))
endif



# Remove syscalls.c if exists (to avoid conflicts)
#SRCS := $(filter-out %syscalls.c,$(SRCS))

# Chỉ định nơi tìm file nguồn (Thư mục hiện tại và Core/Src)
vpath %.c $(SRC_DIR) .

# Tạo danh sách các file object (chỉ lấy tên file, không lấy đường dẫn)
OBJS = $(addprefix $(BUILD_DIR)/, $(notdir $(SRCS:.c=.o)))

# Tên file đầu ra
TARGET = $(BUILD_DIR)/firmware

# Mục tiêu mặc định
all: $(BUILD_DIR) $(TARGET).elf $(TARGET).hex $(TARGET).bin size
	@echo "-----BUILD COMPLETE FOR STM32F411VE $(BUILD_TIME)------"

# Tạo thư mục build
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Biên dịch file .c thành .o (sử dụng vpath để tìm nguồn)
$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Liên kết các file object thành file elf
$(TARGET).elf: $(OBJS)
	@echo "SRC_DIR: $(SRC_DIR)"
	@echo "SRC_DIR: $(SRCS)"
	@echo "Linking: $(OBJS)"
	$(CC) $(LDFLAGS) $(OBJS) -o $@
	$(OBJDUMP) -h -S $@ > $(TARGET).list

# Tạo file hex từ elf
$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

# Tạo file binary từ elf
$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# Hiển thị kích thước
size: $(TARGET).elf
	$(SIZE) $< 
	@echo "Saving size information to $(TARGET)_size.txt"
	$(SIZE) $< > $(TARGET)_size.txt

# Dọn dẹp
clean:
	rm -rf $(BUILD_DIR)

# Phân tích mã assembly
# make dump TARGET="myprogram.elf" OPTIONS="-S -d"
FILE_DUMP ?= build/stm32f4xx_gpio.o
OBJDUMP ?= $(PREFIX)objdump
OPTIONS ?= -S

dump_asm:
	@echo "Dumping $(FILE_DUMP) with options: $(OPTIONS)"
	$(OBJDUMP) $(OPTIONS) $(TARGET) > $(TARGET)_asm.asm

trace_line:
	@if [ -z "$(ADDR)" ]; then \
        read -p "Enter address (hex, e.g., 0x08001234): " ADDR; \
    fi; \
    $(ADDR2) -e $(TARGET) $$ADDR


PATH_OCD := "d:/STMicroelectronics/OpenOCD-20240916-0.12.0"
OPEN_OCD = openocd.exe
interface_PATH = $(PATH_OCD)/share/openocd/scripts/interface

TARGET_PATH = $(PATH_OCD)/share/openocd/scripts/target
OPEN_OCDLINUX = openocd
PYTHON = python.exe

# Mục tiêu flash
#flash: $(TARGET).bin
#	$(OPEN_OCD) -f "$(interface_PATH)/stlink.cfg" -f "$(TARGET_PATH)/stm32f4x.cfg" -c "program \"$(TARGET).bin\" reset exit 0x08000000"


connect_window:
	$(OPEN_OCD) -f interface/jlink.cfg -f $(CURDIR)/openocd.cfg

connect:
	$(OPEN_OCD) -f $(CURDIR)/openocd.cfg

convert:
	$(PYTHON) *.py
	code *.txt

jlink_server:

JLINK      = JLink

BUILDDIR   = build
TARGET     = $(BUILDDIR)/firmware.elf
	
flash: $(TARGET)
	$(JLINK) -CommanderScript flash.jlink

connect_linux:
	$(OPEN_OCDLINUX) -f /usr/share/openocd/scripts/interface/stlink.cfg -f /usr/share/openocd/scripts/target/stm32f4x.cfg


# Mục tiêu debug: Khởi chạy GDB client, yêu cầu OpenOCD chạy ở terminal khác
# NOTE: Dùng JLinkGDBServer thì port 2331 còn OpenOcd thì port 3333
# GDB ↔ JLinkGDBServer ↔ J-Link probe ↔ STM32
debug: $(TARGET).elf
	@echo "--------------------------------------------------------------------"
	@echo " IMPORTANT: Make sure OpenOCD is running in a SEPARATE terminal:"
	@echo " $(OPEN_OCD) -f \"$(STLINK_PATH)/stlink.cfg\" -f \"$(TARGET_PATH)/stm32f4x.cfg\""
	@echo "--------------------------------------------------------------------"
	@echo "Starting GDB client and connecting to localhost:3333..."
	arm-none-eabi-gdb $(CURDIR)/$(TARGET).elf \
		-ex "set confirm off" \
		-ex "directory $(CURDIR)/Core/Src" \
		-ex "target remote localhost:2331" \  
		-ex "monitor tpiu config internal output_itm.txt uart off 16000000 2000000" \
		-ex "monitor reset halt" \
		-ex "load"
		-ex "break main" \
		-ex "continue" \
		-ex "break main" \
		-ex "continue"

# Mục tiêu debug trên Linux/WSL: Khởi chạy GDB client, yêu cầu OpenOCD chạy ở terminal khác
debug_linux: $(TARGET).elf
	@echo "--------------------------------------------------------------------"
	@echo " IMPORTANT: Make sure OpenOCD is running in a SEPARATE terminal:"
	@echo " $(OPEN_OCD) -f \"$(STLINK_PATH)/stlink.cfg\" -f \"$(TARGET_PATH)/stm32f4x.cfg\""
	@echo "--------------------------------------------------------------------"
	@echo "Starting GDB client and connecting to localhost:3333..."
	gdb-multiarch $(CURDIR)/$(TARGET).elf \
		-ex "set confirm off" \
		-ex "directory $(CURDIR)/Core/Src" \
		-ex "target remote localhost:2331" \
		-ex "monitor reset halt" \
		-ex "load" \
		-ex "break main" \
		-ex "continue"

reset:
	gdb -x restart.gdb $(TARGET).elf

cleanfile:
	@> output_itm.txt

.PHONY: all clean size dump flash debug