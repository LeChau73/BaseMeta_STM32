# Alignment X,Y,Z Tính Toán và Các Công Đoạn

## Tóm Tắt Nhanh
- **Alignment** = Căn chỉnh vị trí wafer/chip trên stage để probe card chạm đúng vào điểm cần kiểm tra
- **X, Y, Z** = 3 chiều không gian:
  - **X, Y**: Vị trí ngang (horizontal) - tìm chip đúng vị trí
  - **Z**: Chiều cao (vertical height) - điều chỉnh độ sâu tiếp xúc của probe
- **Công Đoạn Chính**: Khoảng 3-6 giai đoạn tùy thuộc loại alignment

## Chi Tiết Từng Công Đoạn Alignment

### 1. **PRE-ALIGNMENT (Căn Chỉnh Sơ Cấp)**
- **Mục đích**: Dịch chuyển stage gần vị trí gần đúng
- **Phương pháp**: Sử dụng camera hoặc cảm biến để tìm wafer markers (notch/orientation mark)
- **Kết quả**: Xác định big rotation (θ - theta) của wafer, offset X,Y gần đúng
- **Tệp**: `aAlignProc.c` - Xử lý logic alignment
- **GPIB Command**: `AA001` (Start Alignment)

### 2. **ROUGH ALIGNMENT (Căn Chỉnh Thô)**
- **Mục đích**: Di chuyển stage đến chip reference (reference die)
- **Phương pháp**: 
  - Tính offset từ wafer center đến reference die
  - Di chuyển stage theo offset này
  - Camera hoặc PMI (Probe Mark Image) xác định vị trí
- **Tính Toán**:
  ```
  Stage_X = Wafer_Center_X + RefDie_Offset_X
  Stage_Y = Wafer_Center_Y + RefDie_Offset_Y
  ```
- **Lỗi Tolerance**: ±10-50 micron (có thể cấu hình)

### 3. **FINE ALIGNMENT (Căn Chỉnh Tinh)**
- **Mục đích**: Micro-adjustment cuối cùng sử dụng probe pad hoặc visual detect
- **Phương pháp**:
  - N-Shot (multiple measurements at N points on die)
  - Tính toán least-square fit cho rotation correction
- **Tính Toán**:
  ```
  Actual_Pad_Position = Camera_Measure(pad_1, pad_2, pad_3...)
  Calculated_Position = Expected_Pad_Position
  
  Error_dX = Actual - Calculated (X error)
  Error_dY = Actual - Calculated (Y error)
  Error_dTheta = Rotation correction
  
  Offset_X = Error_dX + Stage_Move_X
  Offset_Y = Error_dY + Stage_Move_Y
  ```
- **Lỗi Tolerance**: ±2-5 micron

### 4. **HEIGHT ALIGNMENT (Căn Chỉnh Chiều Cao - Z)**
- **Mục đích**: Điều chỉnh Z để probe card tiếp xúc đúng độ
- **Phương pháp**:
  - Dùng contact detection (dò điểm chạm đầu tiên)
  - Hoặc dùng capacitance sensor / displacement measurement
- **Tính Toán**:
  ```
  Z_Contact_Point = Initial_Z + Contact_Force_Adjustment
  Z_Test_Position = Z_Contact_Point - Overtravel_Distance
  
  Offset_Z = Wafer_Thickness_Variation + Chuck_Flatness_Offset + Thermal_Offset
  Final_Z = Z_Test_Position + Offset_Z
  ```
- **Các Loại Offset Z**:
  - **Wafer Thickness Correction**: Nếu wafer dày hơn bình thường
  - **Chuck Flatness**: Chuck có thể không hoàn toàn phẳng
  - **Thermal Offset**: Nhiệt độ cao thay đổi khoảng cách
  - **Preheat Offset**: Sau preheat cần điều chỉnh thêm

### 5. **ALIGN BY DIE (Căn Chỉnh Theo Từng Chip)**
- **Mục đích**: Mỗi chip có vị trí khác nhau, cần micro-adjustment
- **Phương pháp**:
  - Tính toán interpolation từ map (trong configuration)
  - Dịch chuyển stage nhỏ theo từng chip
- **Tính Toán**:
  ```
  Die_Position_XY = Reference_Die_Position + Map_Offset[die_index]
  
  Tính ví dụ:
  - Chip 1 (reference): X=1000, Y=1000
  - Chip 2: X=1000+150, Y=1000+0
  - Chip 3: X=1000+300, Y=1000+100
  ...
  ```

### 6. **MICRO ALIGNMENT (Alignment Cuối Cùng)**
- **Mục đích**: Ultra-fine adjust dùng WAPP (Wafer Alignment Probe Pad)
- **Phương pháp**: 
  - Giải pháp dùng các probe pad nhỏ để detect position
  - Visual adjustment qua camera image
- **Tính Toán**:
  ```
  Final_Offset_XY = Measured_Pad_Position - Expected_Pad_Position
  Stage_Move = -Final_Offset (âm vì stage phải di chuyển ngược)
  ```

## Các Thuật Toán Chính

### 1. **Least-Square Fit (Bình Phương Tối Thiểu)**
```
Khi có N điểm đo đạc (N-shot alignment):
- Tìm line of best fit qua N điểm
- Tính rotation angle θ
- Tính offset X, Y

Công thức:
  θ = atan2(ΣXiYi / ΣXi²)  [rotation từ N điểm]
  dX = Average(Measured_X - Expected_X)
  dY = Average(Measured_Y - Expected_Y)
```

### 2. **Interpolation (Nội Suy)**
```
Khi biết position của reference die và các neighbor dies:
  Die_Position[i,j] = Ref_Die + (i*DieSpace_X, j*DieSpace_Y)
  
Cho wafer có grid như:
  [0,0]  [1,0]  [2,0]
  [0,1]  [1,1]  [2,1]  <- [1,1] là reference die
  [0,2]  [1,2]  [2,2]
  
Position của [i,j] = Offset_X(i) + Offset_Y(j)
```

### 3. **Thermal Compensation**
```
Z_offset = Base_Z_offset + Temperature_Coefficient * (Current_Temp - Reference_Temp)
```

## Các Thành Phần X, Y, Z trong Code (APPLI Structures)

```c
// File: APPL_wafData.h / aApMmiCom.c

typedef struct {
    intl  stage_x;       // X position (in motor steps)
    intl  stage_y;       // Y position (in motor steps)
    intl  stage_z;       // Z position (in motor steps)
    intl  stage_theta;   // Rotation angle (in degree * 1000)
} StagePosition;

typedef struct {
    intl  offset_x;      // X offset từ reference die (micron)
    intl  offset_y;      // Y offset từ reference die (micron)
    intl  offset_z;      // Z offset từ theo parameter (micron)
} AlignmentOffset;
```

## Tolerance Values (Cấu Hình trong Recipe)

Từ [xmldef.alf](../RecipesExportApplication/xmldef.alf):
```
AP60_7: Probe Alignment - Changeable Tolerance X = ±X micron (default 5-10)
AP60_8: Probe Alignment - Changeable Tolerance Y = ±Y micron (default 5-10)
AP60_9: Probe Alignment - Changeable Tolerance Z = ±Z micron (default 10-20)
AP60_10: Probe Alignment Retry Count = 5 lần (nếu fail)
```

## Flow Diagram

```
Wafer Load
    ↓
→ PRE-ALIGN (Camera find notch) → dX, dY, θ
    ↓
→ DIEINDEXING (Move to ref die) 
    ↓
→ ROUGH-ALIGN (Camera on ref die pad)
    ↓
→ FINE-ALIGN (N-shot, least-square fit) → final dX, dY, dθ
    ↓
→ HEIGHT-ALIGN (Z contact detect) → final dZ
    ↓
→ ALIGN-BY-DIE (Move to next die) → interpolation X, Y
    ↓
→ MICRO-ALIGN (WAPP probe pad check)
    ↓
→ TEST (Probe contact with calculated positions)
```

## Các File Chính Liên Quan

| File | Chức Năng |
|------|-----------|
| `aAlignProc.c` | Main alignment process routine |
| `aSendGpSrq.c` | GpStartAlign() - Start alignment |
| `stgWrkArea.c` | GetWafMicNptChkAdrXYZ() - Coordinate calc |
| `aDurTestNew.c` | Die index, alignment per wafer |
| `aApMmiComSend.c` | Parameter sending, offset calc |

## Điều Chỉnh Khi Alignment Bị Lỗi

Nếu alignment fail:
1. Check camera focus (PRE-ALIGN fail?)
2. Check reference die position (ROUGH-ALIGN fail?)
3. Check tolerance values (quá chặt?)
4. Check thermal offset (nhiệt độ?)
5. Check wafer position (có chuyển động?)
6. Check probe card height (Z alignment OK?)
