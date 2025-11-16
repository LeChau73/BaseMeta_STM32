#include "log.h"

// Hàm chuyển đổi int sang chuỗi (cơ số 10)
char *int_to_string(int num, char *buffer) {
    if (num == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return buffer;
    }

    int is_negative = 0;
    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    // Tính số chữ số
    int temp = num;
    int len = 0;
    while (temp > 0) {
        temp /= 10;
        len++;
    }

    // Thêm ký tự kết thúc chuỗi
    buffer[len + is_negative] = '\0';

    // Chuyển đổi từng chữ số
    int i = len + is_negative - 1;
    while (num > 0) {
        buffer[i--] = (num % 10) + '0';
        num /= 10;
    }

    // Thêm dấu trừ nếu là số âm
    if (is_negative) {
        buffer[0] = '-';
    }

    return buffer;
}


/**
 * @brief In giá trị số nguyên ra log (ITM)
 * @param value Số nguyên cần in
 */
void print_int(int value)
{
    char buffer[MAX_SIZE_BUFF];

    char* str = int_to_string(value, buffer);

    ITM_Init(false);

    if ( str != (void*)0 )
    {
        ITM_SendString(str);
    }
    else
    {
        // Dùng cho debug, nếu k chuyển thành công thì bật đèn 12 lên để nhận biết
        void debug_by_led();
    }

}

/**
 * @brief Chau Code ^-^
 * @details bởi vì máy sẽ lưu dạng bit nên kiểu int thực ra nếu view cx là hex : 0x2001ffbc
 * @param value 
 * @param buffer 
 * @return char* 
 */
char* convert_hex_to_string(uint32_t value, char* buffer)
{
    //[x]: Code cho chạy đã
    //[ ]: Optimazit cách tối ưu
    //: Lưu vào int : (0x2001ffbc)
    // Dịch 4bit một để tách 0xc
    // Sau đó dùng cơ chế chuyển từ số sang unicode or utf-8 
    // số thì + '0' || chữ thì + 'a', số đó phải % 10 để lấy dư
    // lưu nó vào buffer => in ra thôi
    buffer[0] = '0';
    buffer[1] = 'x';
    uint8_t haft_byte;

    //4byte => [4byte] => 8 haftbyte[1111]
    for (uint8_t i = 9; i > 1; i--)
    {
        haft_byte = 0;
        haft_byte |= (value & 0x0F);
        value = value >> 4;

        if ( haft_byte >= 10 )
        {
            buffer[i] = (haft_byte % 10) + 'a';
        }
        else
        {
            buffer[i] = (haft_byte % 10) + '0';
        }

    }

    buffer[10] = '\0';
}

/**
 * @brief AI code =)))
 * @param value 
 * @param buffer 
 */
void hex_to_ascii_bytes(uint32_t value, char* buffer) {
    // Lấy từng byte từ giá trị 32-bit và chuyển thành ký tự
    buffer[0] = (char)((value >> 24) & 0xFF); // Byte cao nhất
    buffer[1] = (char)((value >> 16) & 0xFF);
    buffer[2] = (char)((value >> 8) & 0xFF);
    buffer[3] = (char)(value & 0xFF);         // Byte thấp nhất
    buffer[4] = '\0'; // Kết thúc chuỗi
}



/**
 * @brief Variadic function
 * @param 
 * @todo Test variadic function có hoạt động trên arm được k
 * @details
 * %d  : Số nguyên có dấu (hệ 10)
 * %x  : Số nguyên không dấu (hệ 16, chữ thường)
 * %c  : Ký tự
 * %s  : Chuỗi ký tự
 * %p  : Địa chỉ con trỏ (pointer)
 * %%  : In ra ký tự %
 */
//TODO: Debug cho cách code mới
void myPrintf ( const char* fmt, ... )
{
    char* find_charac = fmt;
    bool check_condition = true;
    char* token;

    va_list list_va;
    va_start(list_va, fmt);

    while(*fmt)
    {
        if (*fmt == '%')
        {
            fmt++;
            switch (*fmt)
            {
                //@: Xử lý tại ký tự sau % tức là : "%d" => đang xử lý tại d
                case 'd': {
                    int i = va_arg(list_va , int );
                    print_int(i);
                    break;
                }
                case 'x': {
                    int i = va_arg(list_va , int );
                    char buffer_temp[10];
                    convert_hex_to_string(i, buffer_temp);
                    ITM_SendString(buffer_temp);
                    break;
                }
                case 'c': {
                    char c = va_arg(list_va , char );
                    ITM_SendChar(c);
                    break;
                }
                case 's': {
                    const char* str = va_arg( list_va, char* );
                    ITM_SendString(str);
                    break;
                }
                case 'p': {
                    int i = va_arg(list_va , int );
                    char buffer_temp[10];
                    convert_hex_to_string(i, buffer_temp);
                    print_int(i);
                    break;
                }
                case '%':
                    ITM_SendString((char[]){"%%\0"});
                    break;
                default:
                    ITM_SendChar('%');
                    // CHeck đoạn code này
                    //INFOR: (char[]){*fmt, 0} caller compound literal
                    ITM_SendString((char[]){*fmt, 0});
                    break;
            }
        } else {
            ITM_SendString((char[]){*fmt, 0});
        }
        fmt++;
    }

    va_end(list_va);

}












/**
 * @brief Dùng cho debug
 * 
 */
void debug_by_led()
{
    gpio_init();
    led_on(12);
}



void led_on(uint8_t pin) 
{
    GPIOD_BSRR = (1 << pin);         // Set pin (ON)
}

void led_off(uint8_t pin) 
{
    GPIOD_BSRR = (1 << (pin + 16));  // Reset pin (OFF)
}