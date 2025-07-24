#include "Printf_Log.h"




// Hàm chuyển đổi int sang chuỗi (cơ số 10)
char *int_to_string(int num, char *buffer, int* size) {
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

    *size = len + is_negative;
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


void Printf_Int( int value )
{
    char buffer[MAX_SIZE_BUFF];
    int size;
    char* str = int_to_string(value, buffer, &size);

    

}


