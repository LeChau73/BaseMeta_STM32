#!/usr/bin/env python3
"""
Example: How to add custom tests for your STM32 functions
This shows how to extend the testing framework for new functions
"""

import unittest
from stm32_test_framework import STM32FunctionSimulator, STM32RegisterMock

class TestCustomFunctions(unittest.TestCase):
    """Example test class for custom functions"""
    
    def setUp(self):
        """Setup for each test"""
        self.simulator = STM32FunctionSimulator()
        
    def test_custom_led_function(self):
        """Example: Test a custom LED function"""
        # Simulate led_on function from log.c
        pin = 12
        
        # Before: BSRR should be 0
        initial_bsrr = self.simulator.register_mock.read_register('GPIOD_BSRR')
        self.assertEqual(initial_bsrr, 0, "BSRR should start at 0")
        
        # Simulate led_on(12) - sets bit 12 in BSRR
        expected_bsrr = 1 << pin
        self.simulator.register_mock.write_register('GPIOD_BSRR', expected_bsrr)
        
        # Verify LED was turned on
        final_bsrr = self.simulator.register_mock.read_register('GPIOD_BSRR')
        self.assertEqual(final_bsrr, expected_bsrr, f"LED pin {pin} should be set")
        
    def test_custom_delay_function(self):
        """Example: Test a custom delay function"""
        # Simulate a delay function that uses a loop
        delay_count = 1000
        
        # Measure execution time
        start_time = self.simulator.simulate_timeStart()
        
        # Simulate delay loop (similar to your for loop delays)
        simulated_cycles = delay_count * 4  # Assume 4 cycles per iteration
        self.simulator.execution_cycles += simulated_cycles
        
        end_time = self.simulator.simulate_timeEnd()
        
        # Verify delay took some time
        time_diff = end_time - start_time
        self.assertGreater(time_diff, 0, "Delay should take some time")
        
    def test_custom_bit_manipulation(self):
        """Example: Test bit manipulation functions"""
        # Test SET_BIT macro from main.c
        register_value = 0x00000000
        bit_position = 3
        
        # Simulate SET_BIT(reg, bit) => reg |= (1 << bit)
        register_value |= (1 << bit_position)
        
        # Verify bit was set
        expected_value = 0x00000008  # Bit 3 set
        self.assertEqual(register_value, expected_value, f"Bit {bit_position} should be set")
        
        # Test multiple bits
        register_value |= (1 << 5)  # Set bit 5
        register_value |= (1 << 7)  # Set bit 7
        
        expected_value = 0x000000A8  # Bits 3, 5, 7 set
        self.assertEqual(register_value, expected_value, "Multiple bits should be set")
        
    def test_custom_printf_formatting(self):
        """Example: Test custom printf formatting logic"""
        # Test hex formatting like in myPrintf
        test_values = [
            (0x1234, "0x00001234"),
            (0xABCD, "0x0000abcd"),
            (0xFFFF, "0x0000ffff"),
        ]
        
        for input_val, expected in test_values:
            with self.subTest(input_val=input_val):
                # Simulate hex formatting
                result = f"0x{input_val:08x}"
                self.assertEqual(result, expected, f"Hex format of 0x{input_val:x} failed")
                
    def test_custom_register_configuration(self):
        """Example: Test register configuration sequences"""
        # Test a typical STM32 register configuration sequence
        
        # 1. Enable clock
        rcc_ahb1enr = self.simulator.register_mock.read_register('RCC_AHB1ENR')
        rcc_ahb1enr |= 0x08  # Enable GPIOD clock
        self.simulator.register_mock.write_register('RCC_AHB1ENR', rcc_ahb1enr)
        
        # 2. Configure GPIO mode
        gpio_moder = self.simulator.register_mock.read_register('GPIOD_MODER')
        gpio_moder |= (0x1 << (12 * 2))  # Set pin 12 as output
        self.simulator.register_mock.write_register('GPIOD_MODER', gpio_moder)
        
        # 3. Configure GPIO speed
        gpio_ospeedr = self.simulator.register_mock.read_register('GPIOD_OSPEEDR')
        gpio_ospeedr |= (0x2 << (12 * 2))  # Set pin 12 as high speed
        self.simulator.register_mock.write_register('GPIOD_OSPEEDR', gpio_ospeedr)
        
        # Verify configuration
        final_ahb1enr = self.simulator.register_mock.read_register('RCC_AHB1ENR')
        final_moder = self.simulator.register_mock.read_register('GPIOD_MODER')
        final_ospeedr = self.simulator.register_mock.read_register('GPIOD_OSPEEDR')
        
        self.assertEqual(final_ahb1enr & 0x08, 0x08, "GPIOD clock should be enabled")
        self.assertEqual((final_moder >> (12 * 2)) & 0x3, 0x1, "Pin 12 should be output")
        self.assertEqual((final_ospeedr >> (12 * 2)) & 0x3, 0x2, "Pin 12 should be high speed")

def run_custom_tests():
    """Run the custom tests"""
    print("Running custom function tests...")
    
    # Create test suite
    loader = unittest.TestLoader()
    suite = loader.loadTestsFromTestCase(TestCustomFunctions)
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    if result.wasSuccessful():
        print("\n*** All custom tests passed! ***")
    else:
        print(f"\n*** {len(result.failures + result.errors)} custom tests failed ***")
    
    return result.wasSuccessful()

if __name__ == "__main__":
    success = run_custom_tests()
    exit(0 if success else 1)