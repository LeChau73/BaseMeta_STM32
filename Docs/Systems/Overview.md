# Init of System

### HAL_Init
    > Call đến HAL_InitTick(15) : 15 là priority, tức là ngắt của systick là 15.
        - **Không, không phải 1 giây, mà là 1 millisecond (1ms).**

## Tính toán:

Với `SystemCoreClock = 16,000,000` (16MHz) và `HAL_TICK_FREQ_DEFAULT = 1` (1KHz = 1ms):

```c
ticks = SystemCoreClock / (1000U / uwTickFreq)
      = 16,000,000 / 1000
      = 16,000
```

**SysTick->LOAD** được set:
```c
SysTick->LOAD = ticks - 1 = 15,999
```

## Thời gian ngắt:

SysTick là **bộ đếm xuống** (countdown). Từ LOAD đến 0:
- Số xung clock = LOAD + 1 = 16,000 xung
- Thời gian = 16,000 xung / 16,000,000 Hz = **0.001 second = 1ms**



## 2. SystemClock_Config ,gọi thằng này cấu hình lại clock