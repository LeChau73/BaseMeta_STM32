# Mới nhất:
    + Có thể in được ITM ra ngoài cả file cả console
    + Nhưng hiện tại không thể dùng hàm print được
    + Tự viết lại hàm print của riêng mình

# Cách chạy
    + Chỉ cần vào debug ,run đúng task là có thể debug (file lauch đã setting đầy đủ hết rồi)


# BaseMeta_STM32
Lean Base Meta Register For Stm32f411veT6

# How to use:
    - run make connect to connect to board(windows)
    - run make debug to debug in gdb at other teminal(windows)
D:\STMicroelectronics\STM32Cube\STM32CubeCLT_1.16.0\GNU-tools-for-STM32\bin

MUỐN IN ĐƯỢC ITM CẦN PHẢI 
monitor tpiu config internal output_itm.txt uart off 16000000 2000000
+ Project này support cả debug bằng makefile và vscode


# script decode.py : để convert lại data khi đọc từ itm sang cho đẹp(bỏ ký tự 0x01 đi)
# run "make convert" để conevert


# Explantion tree folder for RTOS:
    - FreeRTOS
        - include: 
            - FreeRTOS.h
            - portmacro.h
            - task.h
            
        - FreeRTOS/Source
        - FreeRTOS/Source/portable

# TODO:
    + Đọc tiếp debug,hiểu,và debug các thanh ghi
    + Tự viết hàm write ,print để debug dễ hơn
    + Hiểu xong ITM => GPIO => UART ....
    + Từng bước một,chậm thôi


19/7
    + DWT
        => 4 bộ so sánh 
            - hardware watchpoint 
            - ETM trigger
            - PC sample event triggle
            - data address sample event trigger
        Và cũng có thể clock cycle counter ,CYCCNT
            - data compare

//TODO: //FIXME //HACK //DEBUG //BUG //XXX //[ ] //[x] //INFOR