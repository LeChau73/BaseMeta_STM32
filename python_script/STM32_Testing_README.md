# STM32F411 Function Testing Framework

## 📋 Tổng quan

Framework này được tạo để test các hàm custom trong dự án STM32F411 của bạn. Nó sử dụng Python để mô phỏng (simulate) các hardware registers và test logic của các hàm mà không cần phần cứng thực tế.

## 🎯 Các hàm được test

### 📁 Core/Src/main.c
- `printValueRegister()` - Inline function performance test
- `printValueRegister1()` - Static function performance test  
- `DWT_DataMaching()` - DWT data matching configuration
- `ConfigClockHSE16MHZ()` - Clock configuration logic
- `configGpio()` - GPIO configuration sequence

### 📁 Drivers/STM32F411VE_Driver/Src/stm32f4xx_gpio.c
- `GPIO_Init()` - GPIO initialization
- `GPIO_Toogle()` - GPIO toggle functionality

### 📁 Drivers/STM32F411VE_Driver/Src/log.c
- `int_to_string()` - Integer to string conversion
- `convert_hex_to_string()` - Hex to string conversion
- `hex_to_ascii_bytes()` - Hex to ASCII bytes conversion
- `myPrintf()` - Custom printf implementation logic

### 📁 Drivers/STM32F411VE_Driver/Src/debugFunc.c
- `timeStart()` - Timer start function
- `timeEnd()` - Timer end function
- `conculateTime()` - Time calculation function

## 🚀 Cách sử dụng

### 1. Chạy tất cả tests
```bash
python stm32_test_framework.py
```

### 2. Sử dụng test runner
```bash
# Chạy tất cả tests
python run_tests.py

# Xem danh sách tests có sẵn
python run_tests.py --list

# Chạy test cụ thể
python run_tests.py TestSTM32Functions.test_gpio_initialization

# Xem help
python run_tests.py --help
```

### 3. Chạy test cụ thể với unittest
```bash
# Test GPIO functions
python -m unittest stm32_test_framework.TestSTM32Functions.test_gpio_initialization -v

# Test string conversion functions
python -m unittest stm32_test_framework.TestStringConversionFunctions -v

# Test performance comparison
python -m unittest stm32_test_framework.TestMainFunctionLogic.test_performance_comparison_loop -v
```

## 📊 Các loại test

### 1. **Performance Testing**
- So sánh performance giữa inline và static functions
- Đo thời gian thực thi
- Đếm cycles

```python
def test_printValueRegister_performance(self):
    cycles_inline = self.simulator.simulate_printValueRegister(1000)
    cycles_static = self.simulator.simulate_printValueRegister1(1000)
    self.assertLess(cycles_inline, cycles_static)
```

### 2. **Hardware Register Testing**
- Mock STM32 registers (GPIO, RCC, DWT, ITM)
- Verify register configurations
- Test bit manipulations

```python
def test_gpio_initialization(self):
    result = self.simulator.simulate_GPIO_Init('GPIOD', 12, 1, 2)
    self.assertTrue(result)
    # Verify registers were set correctly
```

### 3. **Logic Testing**
- Test algorithm correctness
- Edge case handling
- Input validation

```python
def test_int_to_string_conversion(self):
    test_cases = [(0, "0"), (123, "123"), (-456, "-456")]
    for input_val, expected in test_cases:
        result = self.simulate_int_to_string(input_val)
        self.assertEqual(result, expected)
```

## 🔧 Framework Architecture

### STM32RegisterMock
- Mô phỏng tất cả STM32 hardware registers
- GPIO, RCC, DWT, ITM registers
- Read/Write operations
- State tracking

### STM32FunctionSimulator  
- Simulate function execution
- Performance measurement
- Hardware interaction simulation
- Cycle counting

### Test Classes
- `TestSTM32Functions` - Core STM32 function tests
- `TestStringConversionFunctions` - String utility tests
- `TestMainFunctionLogic` - Main function logic tests

## 📈 Kết quả mong đợi

```
STM32F411 FUNCTION TESTING FRAMEWORK
====================================
test_clock_configuration_logic ... OK
test_dwt_data_matching ... OK
test_gpio_initialization ... OK
test_gpio_toggle ... OK
test_printValueRegister_performance ... OK
test_timing_functions ... OK
test_gpio_configuration_sequence ... OK
test_hex_conversion ... OK
test_hex_to_ascii_bytes ... OK
test_int_to_string_conversion ... OK
test_performance_comparison_loop ... OK

====================================
TEST SUMMARY
====================================
Tests run: 11
Failures: 0
Errors: 0
✅ ALL TESTS PASSED!
```

## 🎯 Lợi ích của framework

### ✅ **Verify Logic trước khi Flash**
- Test algorithm correctness
- Catch bugs early
- No hardware required

### ✅ **Performance Analysis**
- Compare function implementations
- Optimize code before deployment
- Measure execution time

### ✅ **Regression Testing**
- Ensure changes don't break existing functionality
- Automated testing
- Continuous integration ready

### ✅ **Hardware Simulation**
- Mock STM32 registers
- Test hardware interactions
- Validate configurations

## 🔍 Debug và Troubleshooting

### Logging
Framework sử dụng Python logging để debug:
```python
import logging
logging.basicConfig(level=logging.DEBUG)
```

### Custom Test Cases
Thêm test cases mới:
```python
def test_my_custom_function(self):
    """Test your custom function"""
    result = self.simulator.simulate_my_function(params)
    self.assertEqual(result, expected_value)
```

### Mock Register Values
Kiểm tra register values:
```python
# Read register
value = self.simulator.register_mock.read_register('GPIOD_MODER')

# Write register  
self.simulator.register_mock.write_register('GPIOD_MODER', 0x12345678)
```

## 📝 Mở rộng Framework

### Thêm hàm mới để test
1. Tạo simulation function trong `STM32FunctionSimulator`
2. Thêm test case trong appropriate test class
3. Run tests để verify

### Thêm register mới
1. Thêm vào `STM32RegisterMock.__init__()`
2. Update `read_register()` và `write_register()`
3. Thêm test cases

## 🤝 Đóng góp

Framework này có thể được mở rộng để test thêm nhiều functions khác trong dự án STM32 của bạn. Hãy thêm test cases mới khi bạn viết thêm functions!

## 📞 Support

Nếu có vấn đề với framework, hãy:
1. Check logs để xem error details
2. Verify Python dependencies
3. Ensure test logic matches your C implementation
4. Add debug prints nếu cần

---
**Happy Testing! 🚀**