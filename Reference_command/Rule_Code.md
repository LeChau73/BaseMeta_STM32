### Coding Rules cho Dự Án Bare Metal

#### 1. Đặt tên biến và hàm
- **Tên có ý nghĩa**: Tên biến/hàm phải mô tả rõ chức năng.  
  - ✅ `temperature_celsius`, `init_timer()`  
  - ❌ `x`, `temp`, `f1()`
- **Phong cách đặt tên**: Sử dụng **snake_case** cho biến/hàm, **UPPER_CASE** cho hằng số.  
  - ✅ `read_adc_value()`, `MAX_BUFFER_SIZE`  
  - ❌ `readADCValue`, `maxbuffersize`
- **Tiền tố cho thanh ghi/con trỏ**: Thêm tiền tố để rõ loại dữ liệu.  
  - ✅ `volatile uint32_t *reg_adc`, `buffer_ptr`  
  - ❌ `r`, `p`
- **Ngắn gọn, đủ ý**: Tên không quá dài nhưng phải rõ ràng.  
  - ✅ `init_periph_clock`  
  - ❌ `function_to_initialize_the_peripheral_clock`

#### 2. Tổ chức mã nguồn
- **Chia file theo chức năng**: Tách code thành các file `.c`/`.h` theo ngoại vi.  
  - ✅ `uart.c`, `timer.h`, `main.c`  
  - ❌ Đặt tất cả trong một file `main.c`
- **Hàm ngắn gọn**: Mỗi hàm chỉ làm một việc.  
  - ✅ `init_uart()`, `send_byte()`  
  - ❌ `setup_everything()`
- **Header file rõ ràng**: Định nghĩa struct, macro, prototype trong `.h`.  
  - ✅ 
    ```c
    // uart.h
    #ifndef UART_H
    #define UART_H
    void uart_init(uint32_t baud_rate);
    #endif
    ```

#### 3. Viết comment
- **Comment mục đích**: Giải thích "tại sao" thay vì "làm gì".  
  - ✅ `// Enable Timer 1 for 1ms interrupt`  
  - ❌ `// Set TCCR1B to 0x02`
- **Comment hàm**: Mô tả chức năng, tham số, giá trị trả về.  
  - ✅ 
    ```c
    // Initialize UART with specified baud rate
    // @param baud_rate: Baud rate (e.g., 9600)
    void uart_init(uint32_t baud_rate);
    ```
- **Phân chia code**: Dùng comment để nhóm code liên quan.  
  - ✅ 
    ```c
    // GPIO Setup
    DDRB |= (1 << PB0);
    // Timer Setup
    TCCR0A |= (1 << WGM01);
    ```

#### 4. Thực hành tốt
- **Dùng `#define` tránh số ma thuật**:  
  - ✅ `#define LED_PIN (1 << PB5)`  
  - ❌ `PORTB |= (1 << 5)`
- **Kiểm tra lỗi**: Thêm kiểm tra và comment khi xử lý lỗi.  
  - ✅ 
    ```c
    // Check if UART buffer is ready
    if (UCSR0A & (1 << UDRE0)) {
        UDR0 = data;
    }
    ```
- **Dùng typedef/struct**: Tăng tính rõ ràng cho cấu trúc phức tạp.  
  - ✅ 
    ```c
    typedef struct {
        uint32_t baud_rate;
        uint8_t parity;
    } uart_config_t;
    ```

#### 5. Định dạng code
- **Căn chỉnh nhất quán**: Sử dụng 4 khoảng trắng (spaces) để thụt lề.  
  - ✅ 
    ```c
    void init_timer(void) {
        TCCR0A = 0x00; // Clear register
    }
    ```
  - ❌ Trộn tab và space, thụt lề lung tung.
- **Công cụ hỗ trợ**: Dùng `clang-format` hoặc `astyle` để tự động định dạng.

---

### Ghi chú vào dự án
Bạn có thể copy đoạn văn bản dưới đây vào một file `CODING_RULES.md` hoặc `README.md` trong dự án:

```markdown
# Coding Rules cho Dự Án Bare Metal

## 1. Đặt tên
- Tên biến/hàm: Dùng **snake_case**, mô tả rõ chức năng (VD: `init_timer`, `led_state`).
- Hằng số: Dùng **UPPER_CASE** (VD: `MAX_BUFFER_SIZE`).
- Thanh ghi/con trỏ: Thêm tiền tố (VD: `reg_adc`, `buffer_ptr`).
- Tên ngắn gọn, đủ ý, tránh quá dài hoặc mơ hồ.

## 2. Tổ chức code
- Tách file theo chức năng: `uart.c`, `timer.h`, `main.c`.
- Hàm ngắn gọn, làm một việc duy nhất.
- Header file rõ ràng, chứa prototype và định nghĩa.

## 3. Comment
- Comment mục đích, không mô tả code hiển nhiên.
- Mỗi hàm có comment: chức năng, tham số, giá trị trả về.
- Dùng comment để phân nhóm code (VD: `// GPIO Setup`).


## 4. Thực hành tốt
- Dùng `#define` thay cho số ma thuật (VD: `#define LED_PIN (1 << PB5)`).
- Kiểm tra lỗi và comment khi xử lý.
- Dùng `typedef`/`struct` cho cấu trúc phức tạp.

## 5. Định dạng
- Thụt lề 4 spaces, không dùng tab.
- Dùng công cụ như `clang-format` để định dạng tự động.


# Coding Guidelines

## Đặt tên
- Biến/hàm: Dùng **snake_case**, mô tả rõ chức năng (VD: `led_toggle`, `uart_init`).
- Hằng số: Dùng **UPPER_CASE** (VD: `LED_PIN`, `UART_PARITY_NONE`).
- Struct: Thêm hậu tố `_t` (VD: `uart_config_t`).
- Tránh số ma thuật, dùng `#define` (VD: `#define LED_PIN (1 << PB0)`).

## Comment
- Đầu file: Mô tả mục đích file (VD: `// Implementation of UART functions`).
- Hàm: Comment chức năng, tham số, giá trị trả về:
@param <tên_tham_số> <mô_tả>.

@file: Mô tả tên file.
@brief: Mô tả ngắn gọn về hàm, file, hoặc struct.
@param: Mô tả tham số của hàm.
@note: Ghi chú thêm thông tin quan trọng.
@author, @date: Thông tin về tác giả và ngày tạo.
/**<**: Dùng để mô tả ngay sau thành viên của struct.


