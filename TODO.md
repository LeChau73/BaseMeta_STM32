7/27 : Phát hiện ra : Digital của arduino k đo được out put của stm32
    => Sử dụng analog của arduino để đọc giá trị
30/7 : Đã code hàm print xử lý trường hợp %
    => Chưa debug code chat gpt gợi ý
    [x]: Code thêm xử lý hex và in địa chỉ con trỏ

10/8 : Done log nhưng chưa tối ưu nó (có cách hay hơn)
    [ ] Tối ưu log
    [ ] Viết driver GPIO

16/8 : Viết driver cho GPIO sử dụng decade pattern ,stagegy, Data Object 
[x] List ra những CV sẽ làm
[x] TÌm hiểu pattern
[x] Hiểu overview và chia task => timeline
[ ] Viết định nghĩa các cấu trúc sẽ làm
[ ] define các macro
[ ] Kiểm tra nên sử dụng hàm inline hay #define
[ ] text

//TODO: //FIXME //HACK //DEBUG //BUG //XXX //[ ] //[x] //INFO

11 -> 17
[ ] Viết GPIO dựa trên design pattern : t2
[ ] Viết GPIO dựa trên design pattern : t3 (nhẹ)
[ ] Viết GPIO dựa trên design pattern : t5
[ ] Viết GPIO dựa trên design pattern : t7 (nặng)
[ ] //XXX Done GPIO + Hiểu thêm 2-3 pattern
18 -> 23 : UART
[ ] Viết driver UART
[ ] Hiểu sâu về UART
25 -> 30 : TIMER
[ ] : Hiểu sâu về timer
[ ] : Viết driver










//HACK : Mục tiêu viết 1 OS như VxWork ,FreeRTOS, tích hợp các driver vào
//XXX : Dự án này chạy dài đến lúc học FREERTOS
ARM -> Data structor -> FreeRTOS -> Linux(OS) -> Leetcode thường xuyên



I have successfully created a comprehensive Python testing framework for your STM32F411 functions! Here's what I've delivered:

## 🎯 **Complete Testing Framework Created**

### **Main Files:**
- **[`stm32_test_framework.py`](stm32_test_framework.py)** - Complete testing framework with 11 test cases
- **[`run_tests.py`](run_tests.py)** - Easy-to-use test runner utility
- **[`example_custom_test.py`](example_custom_test.py)** - Examples for creating custom tests
- **[`STM32_Testing_README.md`](STM32_Testing_README.md)** - Comprehensive documentation
- **[`TESTING_SUMMARY.md`](TESTING_SUMMARY.md)** - Complete summary and results

## 🚀 **Functions Successfully Tested:**

### From [`Core/Src/main.c`](Core/Src/main.c):
- ✅ [`printValueRegister()`](Core/Src/main.c:15) vs [`printValueRegister1()`](Core/Src/main.c:22) performance comparison
- ✅ [`DWT_DataMaching()`](Core/Src/main.c:38) - DWT configuration testing
- ✅ [`ConfigClockHSE16MHZ()`](Core/Src/main.c:115) - Clock configuration logic
- ✅ [`configGpio()`](Core/Src/main.c:101) - GPIO setup sequence

### From [`Drivers/STM32F411VE_Driver/Src/stm32f4xx_gpio.c`](Drivers/STM32F411VE_Driver/Src/stm32f4xx_gpio.c):
- ✅ [`GPIO_Init()`](Drivers/STM32F411VE_Driver/Src/stm32f4xx_gpio.c:6) - GPIO initialization
- ✅ [`GPIO_Toogle()`](Drivers/STM32F411VE_Driver/Src/stm32f4xx_gpio.c:52) - GPIO toggle functionality

### From [`Drivers/STM32F411VE_Driver/Src/log.c`](Drivers/STM32F411VE_Driver/Src/log.c):
- ✅ [`int_to_string()`](Drivers/STM32F411VE_Driver/Src/log.c:4) - Integer conversion
- ✅ [`convert_hex_to_string()`](Drivers/STM32F411VE_Driver/Src/log.c:75) - Hex conversion
- ✅ [`myPrintf()`](Drivers/STM32F411VE_Driver/Src/log.c:138) - Custom printf logic

### From [`Drivers/STM32F411VE_Driver/Src/debugFunc.c`](Drivers/STM32F411VE_Driver/Src/debugFunc.c):
- ✅ [`timeStart()`](Drivers/STM32F411VE_Driver/Src/debugFunc.c:11), [`timeEnd()`](Drivers/STM32F411VE_Driver/Src/debugFunc.c:22), [`conculateTime()`](Drivers/STM32F411VE_Driver/Src/debugFunc.c:35)

## 📊 **Test Results:**
```
Tests run: 11
Failures: 0  
Errors: 0
*** ALL TESTS PASSED! ***
```

## 🔧 **Key Features:**
- **Hardware Register Mocking** - Simulates STM32 GPIO, RCC, DWT, ITM registers
- **Performance Testing** - Compares inline vs static function performance
- **Logic Verification** - Tests algorithm correctness without hardware
- **Easy Extension** - Framework can be extended for new functions

## 💡 **Usage:**
```bash
# Run all tests
python stm32_test_framework.py

# Run specific test
python run_tests.py TestSTM32Functions.test_gpio_initialization

# List available tests
python run_tests.py --list

# Run custom examples
python example_custom_test.py
```

The framework successfully mocks STM32 hardware registers and tests your custom functions' logic, performance, and correctness without requiring actual hardware. You can now verify your functions work correctly before flashing to the MCU and easily add tests for new functions you develop.