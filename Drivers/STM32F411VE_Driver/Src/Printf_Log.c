#include "Printf_Log.h"




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

char* convert_hex_to_string(uint32_t value, char* buffer)
{
    //: Lưu vào int
    // Dịch 4bit một để tách
    // Sau đó dùng cơ chế chuyển từ số sang hex 
    // số thì + '0' || chữ thì + 'a', số đó phải % 10 để lấy dư
    // lưu nó vào buffer => in ra thôi

    uint8_t haft_byte;


    for (uint8_t i = 9; i > 0; i--)
    {
        haft_byte &= 0x0F;
        haft_byte |= (value >> 4);

        if ( haft_byte >= 10 )
        {
            buffer[i] = (haft_byte % 10) + 'a';
        }
        else
        {
            buffer[i] = (haft_byte % 10) + '0';
        }
    }
    
}


//TODO: Thử custom hàm va_start xem 
//date: 24/7 Thứ 5 (Tối)

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
void myprintf ( const char* fmt, ... )
{
    char* find_charac = fmt;
    bool check_condition = true;
    char* token;

    
    while( ( find_charac = strchr(find_charac, '%') ) != NULL )
    {
        find_charac++;
        if (strchr("dxscp", *find_charac)) {
            // Đúng nếu là một trong các ký tự trên
            check_condition = false;
            break;
        }
    }
    
    if ( check_condition == true )
    {
        ITM_SendString(fmt);
        return;
    }


    // Đối tượng cho variadic
    va_list list_va;

    // Cho phép truy cập đến các biến argument 
    // Truy cập vào các biến trong ...(lưu vào list_va) được chỉ dẫn bởi fmt 
    va_start(list_va, fmt);

    //const char* delim = "%";


    token = strtok(fmt , "%");

    while ( token )
    {
        
        if ( *token == 'd' )
        {
            int i = va_arg( list_va, int );
            print_int(i);
        }
        else if ( *token == 's' )
        {
            const char* str = va_arg( list_va, char* );
            ITM_SendString(str);
        }
        else if ( *token == 'p' )
        {
            // TC : myprintf("%x", &val);
            uint32_t temp = va_arg( list_va, uint32_t );
           // ITM_SendString(temp);
            //print_int(temp);

        }
        

        token = strtok(NULL , "%");
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