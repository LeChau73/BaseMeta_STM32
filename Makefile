# Toolchain
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)as
LD = $(PREFIX)ld
OBJCOPY = $(PREFIX)objcopy
OBJDUMP = $(PREFIX)objdump
SIZE = $(PREFIX)size

# Thư mục dự án
SRC_DIR = \
	Core/Src \
	Drivers/Src \
	Drivers/STM32F411VE_Driver/Src

INC_DIR = \
	Core/Inc \
	Drivers/Core/ \
	Drivers/STM32F411VE_Driver/Inc

BUILD_DIR = build

# Linker script
LINKER_SCRIPT = STM32F411VETx_FLASH.ld

# Cờ biên dịch
CPU = -mcpu=cortex-m4
FPU = -mfpu=fpv4-sp-d16 -mfloat-abi=hard
CFLAGS = $(CPU) -mthumb $(FPU) \
         -Wall -Wextra \
         -O0 -g3 \
         $(foreach dir,$(INC_DIR),-I$(dir)) \
         -std=gnu11

# Cờ liên kết
LDFLAGS = $(CPU) -mthumb $(FPU) \
          -T$(LINKER_SCRIPT) \
          -Wl,-Map=$(BUILD_DIR)/output.map \
          --specs=nano.specs -lc

# Tìm tất cả các file nguồn
SRCS = $(foreach dir,$(SRC_DIR),$(wildcard $(dir)/*.c))
SRCS += startup_stm32f411xe.c

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

# Dọn dẹp
clean:
	rm -rf $(BUILD_DIR)

# Phân tích mã assembly
dump: $(TARGET).elf
	$(OBJDUMP) -d $< > $(TARGET)_asm.txt

PATH_OCD := "d:/STMicroelectronics/OpenOCD-20240916-0.12.0"
OPEN_OCD = $(PATH_OCD)/bin/openocd.exe
STLINK_PATH = $(PATH_OCD)/share/openocd/scripts/interface
TARGET_PATH = $(PATH_OCD)/share/openocd/scripts/target

# Mục tiêu flash
flash: $(TARGET).bin
	$(OPEN_OCD) -f "$(STLINK_PATH)/stlink.cfg" -f "$(TARGET_PATH)/stm32f4x.cfg" -c "program \"$(TARGET).bin\" reset exit 0x08000000"


connect:
	 $(OPEN_OCD) -f $(STLINK_PATH)/stlink.cfg -f $(TARGET_PATH)/stm32f4x.cfg


# Mục tiêu debug: Khởi chạy GDB client, yêu cầu OpenOCD chạy ở terminal khác
debug: $(TARGET).elf
	@echo "--------------------------------------------------------------------"
	@echo " IMPORTANT: Make sure OpenOCD is running in a SEPARATE terminal:"
	@echo " $(OPEN_OCD) -f \"$(STLINK_PATH)/stlink.cfg\" -f \"$(TARGET_PATH)/stm32f4x.cfg\""
	@echo "--------------------------------------------------------------------"
	@echo "Starting GDB client and connecting to localhost:3333..."
	arm-none-eabi-gdb $(CURDIR)/$(TARGET).elf \
		-ex "set confirm off" \
		-ex "directory $(CURDIR)/Core/Src" \
		-ex "target remote localhost:3333" \
		-ex "monitor reset halt" \
		-ex "load" \
		-ex "break main" \
		-ex "continue"

.PHONY: all clean size dump flash debug