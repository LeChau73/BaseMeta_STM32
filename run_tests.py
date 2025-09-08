#!/usr/bin/env python3
"""
Simple test runner for STM32 functions
Usage: python run_tests.py [test_name]
"""

import sys
import subprocess
import os

def run_specific_test(test_name):
    """Run a specific test"""
    cmd = [sys.executable, '-m', 'unittest', f'stm32_test_framework.{test_name}', '-v']
    return subprocess.run(cmd, capture_output=False)

def run_all_tests():
    """Run all tests"""
    cmd = [sys.executable, 'stm32_test_framework.py']
    return subprocess.run(cmd, capture_output=False)

def list_available_tests():
    """List all available tests"""
    tests = [
        "TestSTM32Functions.test_printValueRegister_performance",
        "TestSTM32Functions.test_gpio_initialization", 
        "TestSTM32Functions.test_gpio_toggle",
        "TestSTM32Functions.test_timing_functions",
        "TestSTM32Functions.test_clock_configuration_logic",
        "TestSTM32Functions.test_dwt_data_matching",
        "TestStringConversionFunctions.test_int_to_string_conversion",
        "TestStringConversionFunctions.test_hex_conversion",
        "TestStringConversionFunctions.test_hex_to_ascii_bytes",
        "TestMainFunctionLogic.test_performance_comparison_loop",
        "TestMainFunctionLogic.test_gpio_configuration_sequence"
    ]
    
    print("Available tests:")
    for i, test in enumerate(tests, 1):
        print(f"  {i}. {test}")
    return tests

def main():
    if len(sys.argv) == 1:
        # No arguments - run all tests
        print("Running all STM32 function tests...")
        result = run_all_tests()
        sys.exit(result.returncode)
    
    elif sys.argv[1] == "--list":
        # List available tests
        list_available_tests()
        
    elif sys.argv[1] == "--help":
        # Show help
        print("STM32 Test Runner")
        print("Usage:")
        print("  python run_tests.py                    # Run all tests")
        print("  python run_tests.py --list             # List available tests")
        print("  python run_tests.py <test_name>        # Run specific test")
        print("  python run_tests.py --help             # Show this help")
        print()
        print("Examples:")
        print("  python run_tests.py TestSTM32Functions.test_gpio_initialization")
        print("  python run_tests.py TestStringConversionFunctions")
        
    else:
        # Run specific test
        test_name = sys.argv[1]
        print(f"Running specific test: {test_name}")
        result = run_specific_test(test_name)
        sys.exit(result.returncode)

if __name__ == "__main__":
    main()