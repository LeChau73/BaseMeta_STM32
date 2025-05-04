# BaseMeta_STM32
Lean Base Meta Register For Stm32f411veT6

# How to use:
    - run make connect to connect to board(windows)
    - run make debug to debug in gdb at other teminal(windows)
D:\STMicroelectronics\STM32Cube\STM32CubeCLT_1.16.0\GNU-tools-for-STM32\bin



# TODO:
    Tích hợp RTOS vào project
    Tạo một task để print ITM

# Explantion tree folder for RTOS:
    - FreeRTOS
        - include: 
            - FreeRTOS.h
            - portmacro.h
            - task.h
            
        - FreeRTOS/Source
        - FreeRTOS/Source/portable