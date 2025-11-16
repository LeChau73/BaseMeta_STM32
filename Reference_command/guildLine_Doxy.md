Để sử dụng **Doxygen Documentation Generator** trong Visual Studio Code (VSCode), bạn cần cài đặt extension này và cấu hình để tự động tạo các comment Doxygen (như `@param`, `@brief`, `@return`) cho mã nguồn, đặc biệt phù hợp với dự án bare metal C/C++. Dưới đây là hướng dẫn chi tiết và dễ hiểu để bạn bắt đầu:

---

### 1. Cài đặt Doxygen Documentation Generator
1. **Mở VSCode**:
   - Khởi động VSCode trên máy tính của bạn.

2. **Tìm và cài đặt extension**:
   - Nhấn **Ctrl+Shift+X** (hoặc vào biểu tượng **Extensions** ở thanh bên trái).
   - Tìm kiếm `Doxygen Documentation Generator` (tác giả: Christoph Schlosser).
   - Nhấn **Install** để cài đặt extension.[](https://github.com/cschlosser/doxdocgen)

3. **Kiểm tra cài đặt**:
   - Sau khi cài đặt, bạn sẽ thấy extension trong danh sách **Installed Extensions**.
   - Không cần cài thêm Doxygen trên máy nếu bạn chỉ dùng extension để tạo comment (nhưng nếu muốn tạo tài liệu HTML/PDF, bạn cần cài Doxygen như hướng dẫn trước).

---

### 2. Sử dụng Doxygen Documentation Generator
Extension này giúp tự động tạo comment Doxygen khi bạn gõ một lệnh đặc biệt (thường là `/**` hoặc `///`) trước hàm, lớp, hoặc biến. Dưới đây là cách dùng:

#### Cách tạo comment Doxygen
1. **Viết mã nguồn**:
   - Ví dụ, bạn có một hàm trong file `uart.c`:
     ```c
     void uart_init(uint32_t baud_rate, uint8_t parity);
     ```

2. **Kích hoạt generator**:
   - Đặt con trỏ ngay phía trên** hàm (hoặc lớp, struct).
   - Gõ `/**` và nhấn **Enter**. Extension sẽ tự động tạo một block comment Doxygen với các thẻ như `@brief`, `@param`, v.v.
   - Ví dụ kết quả:
     ```c
     /**
      * @brief 
      * @param baud_rate 
      * @param parity 
      */
     void uart_init(uint32_t baud_rate, uint8_t parity);
     ```

3. **Điền mô tả**:
   - Sau khi comment được tạo, bạn chỉ cần điền mô tả cho từng thẻ:
     ```c
     /**
      * @brief Initialize UART with specified baud rate and parity
      * @param baud_rate Baud rate (e.g., 9600, 115200)
      * @param parity Parity setting (0 = none, 1 = even, 2 = odd)
      */
     void uart_init(uint32_t config);
     ```

4. **Tùy chỉnh comment**:
   - Nếu hàm có giá trị trả về (như `int`), extension sẽ tự động thêm `@return`:
     ```c
     /**
      * @brief Read a byte from UART
      * @return uint8_t Received byte
      */
     uint8_t uart_read_byte(void);
     ```

#### Phím tắt
- Mặc định, bạn gõ `/**` và **Enter** để tạo comment.
- Nếu muốn dùng phím tắt khác:
  - Nhấn **Ctrl+Shift+P** để mở Command Palette.
  - Gõ `Doxygen: Generate` và nhấn Enter để tạo comment tại vị trí con trỏ.[](https://github.com/trond-snekvik/vscode-doxygen-generator)
  - Lưu ý: Phím tắt `Alt+Q` có thể được dùng trong một số phiên bản khác, nhưng bạn nên kiểm tra cấu hình extension.[](https://github.com/trond-snekvik/vscode-doxygen-generator)

---

### 3. Cấu hình extension (Tùy chọn)
Bạn có thể tùy chỉnh cách extension tạo comment bằng cách chỉnh sửa file `settings.json` trong VSCode.

1. **Mở settings**:
   - Nhấn **Ctrl+,** để mở Settings.
   - Tìm `Doxygen` hoặc vào **Extensions > Doxygen Documentation Generator > Extension Settings**.

2. **Một số tùy chỉnh phổ biến**:
   - **Định dạng comment**:
     ```json
     "doxdocgen.c.firstLine": "/**",
     "doxdocgen.c.lastLine": " */",
     "doxdocgen.c.commentPrefix": " * ",
     ```
     Điều này đảm bảo comment có dạng:
     ```c
     /**
      * @brief ...
      */
     ```

   - **Mô tả thông minh**:
     ```json
     "doxdocgen.generic.generateSmartText": true,
     "doxdocgen.generic.splitCasingSmartText": true
     ```
     Tự động tạo mô tả dựa trên tên hàm, ví dụ: `getValue` sẽ tạo `@brief Get the value`.

   - **Thêm thông tin tác giả**:
     ```json
     "doxdocgen.generic.authorName": "Your Name",
     "doxdocgen.generic.authorEmail": "you@example.com",
     "doxdocgen.generic.authorTag": "@author {author} ({email})"
     ```

   - **Thứ tự các thẻ**:
     ```json
     "doxdocgen.generic.order": ["brief", "param", "return", "author", "date"]
     ```
     Quy định thứ tự xuất hiện của các thẻ trong comment.

3. **Ví dụ file `settings.json`**:
   ```json
   {
       "doxdocgen.c.firstLine": "/**",
       "doxdocgen.c.lastLine": " */",
       "doxdocgen.c.commentPrefix": " * ",
       "doxdocgen.generic.authorName": "Your Name",
       "doxdocgen.generic.briefTemplate": "@brief {text}",
       "doxdocgen.generic.paramTemplate": "@param {param} ",
       "doxdocgen.generic.returnTemplate": "@return {type} "
   }
   ```

4. **Áp dụng**:
   - Sau khi chỉnh sửa, extension sẽ tạo comment theo cấu hình mới khi bạn gõ `/**` và Enter.

---

### 4. Tích hợp với Doxygen để tạo tài liệu
Nếu bạn muốn tạo tài liệu HTML/PDF từ các comment Doxygen:
1. **Cài Doxygen**:
   - Tải và cài Doxygen như hướng dẫn trước (dùng lệnh `doxygen --version` để kiểm tra).

2. **Tạo Doxyfile**:
   - Chạy:
     ```bash
     doxygen -g Doxyfile
     ```
   - Chỉnh sửa `Doxyfile`:
     ```plaintext
     PROJECT_NAME = "My Bare Metal Project"
     OUTPUT_DIRECTORY = docs/
     INPUT = ./
     RECURSIVE = YES
     GENERATE_HTML = YES
     ```

3. **Chạy Doxygen trong VSCode**:
   - Cài extension **Doxygen Runner** từ VSCode Marketplace.[](https://marketplace.visualstudio.com/items?itemName=betwo.vscode-doxygen-runner)
   - Thêm cấu hình trong `settings.json`:
     ```json
     "doxygen_runner.configuration_file_override": "${workspaceFolder}/Doxyfile"
     ```
   - Nhấn **Ctrl+Shift+P**, gõ `Generate Doxygen documentation`, và chọn lệnh để chạy Doxygen.[](https://github.com/fbaeuerlein/cpp-vscode-guide/blob/master/doc/Documentation.md)
   - Tài liệu sẽ được tạo trong thư mục `docs/html/`.

---

### 5. Ví dụ thực tế
Giả sử bạn có hàm sau trong `led.c`:
```c
void led_toggle(void);
```

1. Đặt con trỏ phía trên hàm, gõ `/**` và nhấn **Enter**:
   ```c
   /**
    * @brief 
    */
   void led_toggle(void);
   ```
   /**
    * @brief Toggle LED state (on/off)
    */
   void led_toggle(void);
   ```

3. Nếu hàm có tham số:
   ```c
   void set_led_brightness(uint8_t level);
   ```
   Gõ `/**` và Enter:
   ```c
   /**
    * @brief 
    * @param level 
    */
   void set_led_brightness(uint8_t level);

   /**
    * @brief Set LED brightness level
    * @param level Brightness value (0-255)
    */
   void set_led_brightness(uint8_t level);
   ```

4. Chạy Doxygen để tạo tài liệu:
   - Dùng Doxygen Runner hoặc lệnh `doxygen Doxyfile` để sinh HTML.

---

### 6. Mẹo và lưu ý
- **Tương thích bare metal**:
  - Extension hoạt động tốt với C/C++, phù hợp cho code bare metal.
  - Comment thanh ghi rõ ràng, ví dụ:
    ```c
    /**
     * @brief Set Timer 1 prescaler
     * @param value Prescaler value (0-7)
     */
    TCCR1B |= (value << CS10);
    ```

- **Hiển thị comment trong VSCode**:
  - Cài extension **C/C++** (của Microsoft) để hỗ trợ hiển thị Doxygen comment khi hover chuột qua hàm.[](https://devblogs.microsoft.com/cppblog/visual-studio-code-c-extension-july-2020-update-doxygen-comments-and-logpoints/)
  - Đảm bảo bạn dùng `/**` hoặc `///` để VSCode nhận diện.

- **Cập nhật comment**:
  - Nếu hàm thay đổi (thêm/thay đổi tham số), đặt con trỏ trong comment hiện có, chạy lệnh `Doxygen: Generate` để cập nhật danh sách `@param`.[](https://github.com/trond-snekvik/vscode-doxygen-generator)

- **Sao lưu cấu hình**:
  - Lưu file `settings.json` và `Doxyfile` vào repository để đồng bộ với dự án.

- **Hạn chế**:
  - Extension không hỗ trợ đầy đủ các ngôn ngữ ngoài C/C++.
  - Một số cấu hình phức tạp (như template C++) có thể không nhận diện chính xác.[](https://github.com/trond-snekvik/vscode-doxygen-generator)

---

### 7. Nếu gặp vấn đề
- **Comment không tạo**:
  - Kiểm tra xem bạn đã cài extension đúng chưa (tìm `Doxygen Documentation Generator` trong Installed Extensions).
  - Đảm bảo gõ `/**` ngay phía trên hàm, không có dòng trống.

- **Tài liệu Doxygen không tạo**:
  - Kiểm tra đường dẫn trong `Doxyfile` (INPUT, OUTPUT_DIRECTORY).
  - Chạy `doxygen Doxyfile` từ terminal để xem lỗi.

- **Cần hỗ trợ thêm**:
  - Nếu bạn cần cấu hình cụ thể cho vi điều khiển (như AVR, STM32) hoặc thêm ví dụ, hãy cho mình biết!

---

Hy vọng hướng dẫn này giúp bạn sử dụng Doxygen Documentation Generator trong VSCode một cách hiệu quả cho dự án bare metal! Nếu bạn muốn mình làm ví dụ chi tiết hơn hoặc tích hợp với công cụ khác, cứ hỏi nhé!