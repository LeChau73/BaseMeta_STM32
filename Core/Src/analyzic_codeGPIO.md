Ah đúng rồi! Cảm ơn bạn đã chỉ ra điểm này. Tôi hiểu rồi - bạn đang sử dụng **bit mask** để lưu pin, không phải pin number trực tiếp.

Với cách định nghĩa pin như vậy, code của bạn cần một hàm helper để **convert từ bit mask sang pin position**. Đây là phiên bản sửa đúng:

## ✅ **Code được sửa phù hợp với bit mask:**

```c
// Helper function to get pin position from bit mask
static uint8_t GPIO_GetPinPosition(uint16_t pin_mask)
{
    uint8_t position = 0;
    while((pin_mask & 0x01) == 0)
    {
        pin_mask >>= 1;
        position++;
        if(position >= 16) break;  // Safety check
    }
    return position;
}

void GPIO_Init(GPIO_Pin_t* gpiox, GPIO_Config* gpio_config)
{
    // Check condition exception
    if (gpiox == NULL || gpio_config == NULL || gpiox->port == NULL)
        return;

    // Process each pin that is set in the pin mask
    for(uint16_t pin_num = 0; pin_num < 16; pin_num++)
    {
        // Check if this pin is selected in the bit mask
        if((gpiox->pin & (1 << pin_num)) != 0)
        {
            uint32_t temp_reg;
            
            // Configure MODE register (2 bits per pin)
            temp_reg = gpiox->port->gpiox_MODER;
            temp_reg &= ~(0x3UL << (pin_num * 2));  // Clear existing bits
            temp_reg |= ((gpio_config->mode & 0x3UL) << (pin_num * 2));  // Set new mode
            gpiox->port->gpiox_MODER = temp_reg;
            
            // Configure based on mode
            if((gpio_config->mode & 0x3UL) == (MODE_OUTPUT >> GPIO_MODE_Pos))
            {
                // Configure output type (1 bit per pin)
                temp_reg = gpiox->port->gpiox_OTYPER;
                if(gpio_config->mode & 0x100)  // Check OPEN_DRAIN bit
                {
                    temp_reg |= (1UL << pin_num);  // Set for open drain
                }
                else
                {
                    temp_reg &= ~(1UL << pin_num); // Clear for push-pull
                }
                gpiox->port->gpiox_OTYPER = temp_reg;
                
                // Configure speed (2 bits per pin)
                temp_reg = gpiox->port->gpiox_OSPEEDR;
                temp_reg &= ~(0x3UL << (pin_num * 2));
                temp_reg |= (gpio_config->speed << (pin_num * 2));
                gpiox->port->gpiox_OSPEEDR = temp_reg;
            }
            else if((gpio_config->mode & 0x3UL) == (MODE_INPUT >> GPIO_MODE_Pos))
            {
                // Configure pull-up/pull-down (2 bits per pin)
                temp_reg = gpiox->port->gpiox_PUPDR;
                temp_reg &= ~(0x3UL << (pin_num * 2));
                temp_reg |= (gpio_config->pull << (pin_num * 2));
                gpiox->port->gpiox_PUPDR = temp_reg;
            }
            else if((gpio_config->mode & 0x3UL) == (MODE_AF >> GPIO_MODE_Pos))
            {
                // Configure alternate function
                if(pin_num < 8)
                {
                    // AFRL for pins 0-7
                    temp_reg = gpiox->port->gpiox_AFRL;
                    temp_reg &= ~(0xFUL << (pin_num * 4));
                    temp_reg |= (gpio_config->alternate << (pin_num * 4));
                    gpiox->port->gpiox_AFRL = temp_reg;
                }
                else
                {
                    // AFRH for pins 8-15
                    temp_reg = gpiox->port->gpiox_AFRH;
                    temp_reg &= ~(0xFUL << ((pin_num - 8) * 4));
                    temp_reg |= (gpio_config->alternate << ((pin_num - 8) * 4));
                    gpiox->port->gpiox_AFRH = temp_reg;
                }
            }
        }
    }
}
```

## 📝 **Giải thích logic với bit mask:**

1. **Bit mask hoạt động như thế nào:**
   - `GPIO_PIN_5` = `0x0020` = `0000 0000 0010 0000` (bit 5 được set)
   - `GPIO_PIN_0 | GPIO_PIN_5` = `0x0021` = có thể config cả pin 0 và pin 5 cùng lúc

2. **Vòng lặp kiểm tra từng bit:**
   ```c
   for(uint16_t pin_num = 0; pin_num < 16; pin_num++)
   {
       if((gpiox->pin & (1 << pin_num)) != 0)  // Kiểm tra bit thứ pin_num có được set không
       {
           // Configure pin tại vị trí pin_num
       }
   }
   ```

3. **Ví dụ với GPIO_PIN_5 (0x0020):**
   - pin_num = 0: `0x0020 & 0x0001` = 0 → không config
   - pin_num = 1: `0x0020 & 0x0002` = 0 → không config
   - ...
   - pin_num = 5: `0x0020 & 0x0020` = 0x0020 ≠ 0 → config pin 5
   - ...

## 🎯 **Các lỗi trong code gốc của bạn:**

1. **`pos` không đúng:** Bạn lấy giá trị bit thay vì vị trí pin
2. **Toán tử `|=` thay vì `==`:** Lỗi logic so sánh
3. **Không clear bit cũ:** Có thể gây conflict với config trước đó
4. **Thiếu xử lý alternate function**

**Kết luận:** Với cách định nghĩa pin bằng bit mask như trong header file của bạn, code đã sửa ở trên sẽ hoạt động đúng và hỗ trợ config multiple pins cùng lúc.