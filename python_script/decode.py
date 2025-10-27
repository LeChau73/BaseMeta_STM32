import re
import os

def decode_itm_file(input_file, output_file=None, decode_mode='comprehensive'):
    """
    Giải mã file ITM với nhiều chế độ xử lý khác nhau
    
    Args:
        input_file (str): Đường dẫn file đầu vào
        output_file (str): Đường dẫn file đầu ra (tùy chọn)
        decode_mode (str): Chế độ giải mã
            - 'comprehensive': Xử lý tất cả ký tự đặc biệt
            - 'printable_only': Chỉ giữ lại ký tự có thể in
            - 'custom': Xử lý theo quy tắc tùy chỉnh
            - 'explained': Hiển thị và giải thích từng ký tự đặc biệt
    """
    
    try:
        # Đọc file ở chế độ binary để xử lý chính xác
        with open(input_file, 'rb') as f:
            raw_data = f.read()
        
        print(f"Đọc được {len(raw_data)} bytes từ file: {input_file}")
        print(f"Chế độ giải mã: {decode_mode}")
        
        if decode_mode == 'comprehensive':
            decoded_content = decode_comprehensive(raw_data)
            explanation = ""
        elif decode_mode == 'printable_only':
            decoded_content = decode_printable_only(raw_data)
            explanation = ""
        elif decode_mode == 'custom':
            decoded_content = decode_custom(raw_data)
            explanation = ""
        elif decode_mode == 'itm_clean':
            decoded_content = decode_itm_clean(raw_data)
            explanation = ""
        elif decode_mode == 'explained':
            decoded_content, explanation = decode_with_explanation(raw_data)
        elif decode_mode == 'visual':
            decoded_content = decode_visual_display(raw_data)
            explanation = ""
        else:
            raise ValueError("Chế độ giải mã không hợp lệ")
        
        # Hiển thị kết quả
        print("\n" + "="*50)
        print("NỘI DUNG ĐÃ GIẢI MÃ:")
        print("="*50)
        print(decoded_content)
        
        # Hiển thị giải thích nếu có
        if explanation:
            print("\n" + "="*50)
            print("GIẢI THÍCH CÁC KÝ TỰ ĐẶC BIỆT:")
            print("="*50)
            print(explanation)
        
        print("="*50)
        
        # Lưu file nếu được chỉ định
        if output_file:
            with open(output_file, 'w', encoding='utf-8') as f:
                f.write(decoded_content)
                if explanation:
                    f.write("\n\n" + "="*50 + "\n")
                    f.write("GIẢI THÍCH CÁC KÝ TỰ ĐẶC BIỆT:\n")
                    f.write("="*50 + "\n")
                    f.write(explanation)
            print(f"\nĐã lưu kết quả vào: {output_file}")
        
        return decoded_content
        
    except FileNotFoundError:
        print(f"Lỗi: Không tìm thấy file {input_file}")
        return None
    except Exception as e:
        print(f"Lỗi: {e}")
        return None

def decode_comprehensive(raw_data):
    """Giải mã toàn diện - xử lý tất cả ký tự đặc biệt"""
    result = []
    
    for byte in raw_data:
        if byte == 0x01:  # SOH
            continue  # Bỏ qua SOH
        elif byte == 0x00:  # NULL
            continue  # Bỏ qua NULL
        elif byte == 0x0A:  # Line Feed
            result.append('\n')
        elif byte == 0x0D:  # Carriage Return
            result.append('\r')
        elif byte == 0x09:  # Tab
            result.append('\t')
        elif byte == 0x08:  # Backspace
            if result:  # Xóa ký tự trước đó nếu có
                result.pop()
        elif 32 <= byte <= 126:  # Ký tự ASCII có thể in
            result.append(chr(byte))
        elif byte >= 128:  # Extended ASCII hoặc Unicode
            try:
                result.append(chr(byte))
            except:
                result.append(f'[0x{byte:02X}]')  # Hiển thị dưới dạng hex
        else:  # Các ký tự điều khiển khác
            result.append(f'[0x{byte:02X}]')
    
    return ''.join(result)

def decode_printable_only(raw_data):
    """Chỉ giữ lại ký tự có thể in và xuống dòng"""
    result = []
    
    for byte in raw_data:
        if byte == 0x0A:  # Line Feed
            result.append('\n')
        elif byte == 0x0D:  # Carriage Return
            result.append('\r')
        elif byte == 0x09:  # Tab
            result.append('\t')
        elif 32 <= byte <= 126:  # Ký tự ASCII có thể in
            result.append(chr(byte))
        # Bỏ qua tất cả các ký tự khác
    
    return ''.join(result)

def decode_visual_display(raw_data):
    """Chế độ hiển thị trực quan - dễ đọc, chỉ hiển thị byte dưới dạng gọn gàng"""
    result = []
    line_buffer = []
    position = 0
    
    for byte in raw_data:
        # Xử lý các ký tự có thể in bình thường
        if 32 <= byte <= 126:  # Ký tự ASCII có thể in
            line_buffer.append(chr(byte))
        elif byte == 0x0A:  # Line Feed
            line_buffer.append('\\n')
        elif byte == 0x0D:  # Carriage Return
            line_buffer.append('\\r')
        elif byte == 0x09:  # Tab
            line_buffer.append('\\t')
        elif byte == 0x20:  # Space
            line_buffer.append(' ')
        else:
            # Hiển thị byte dưới dạng hex gọn gàng
            if byte < 16:
                line_buffer.append(f'\\x0{byte:X}')
            else:
                line_buffer.append(f'\\x{byte:02X}')
        
        position += 1
        
        # Xuống dòng mỗi 80 ký tự để dễ đọc
        if position % 80 == 0:
            result.append(''.join(line_buffer))
            line_buffer = []
    
    # Thêm phần còn lại
    if line_buffer:
        result.append(''.join(line_buffer))
    
    return '\n'.join(result)

def decode_with_explanation(raw_data):
    """Giải mã và tạo giải thích chi tiết cho từng ký tự đặc biệt"""
    result = []
    explanations = {}
    special_chars_found = set()
    
    # Bảng mapping với giải thích chi tiết
    ascii_control_chars = {
        0x00: ("NULL", "Ký tự NULL - kết thúc chuỗi trong C"),
        0x01: ("SOH", "Start of Heading - Đầu tiêu đề, thường là timestamp trong ITM"),
        0x02: ("STX", "Start of Text - Bắt đầu văn bản"),
        0x03: ("ETX", "End of Text - Kết thúc văn bản"),
        0x04: ("EOT", "End of Transmission - Kết thúc truyền"),
        0x05: ("ENQ", "Enquiry - Yêu cầu phản hồi"),
        0x06: ("ACK", "Acknowledge - Xác nhận"),
        0x07: ("BEL", "Bell - Tiếng chuông/báo hiệu"),
        0x08: ("BS", "Backspace - Xóa lùi"),
        0x09: ("HT", "Horizontal Tab - Tab ngang"),
        0x0A: ("LF", "Line Feed - Xuống dòng (Unix/Linux)"),
        0x0B: ("VT", "Vertical Tab - Tab dọc"),
        0x0C: ("FF", "Form Feed - Sang trang mới"),
        0x0D: ("CR", "Carriage Return - Về đầu dòng (Windows \\r)"),
        0x0E: ("SO", "Shift Out - Chuyển bộ ký tự"),
        0x0F: ("SI", "Shift In - Trở về bộ ký tự gốc"),
        0x10: ("DLE", "Data Link Escape - Thoát liên kết dữ liệu, thường dùng trong ITM protocol"),
        0x11: ("DC1", "Device Control 1 - Điều khiển thiết bị 1 (XON)"),
        0x12: ("DC2", "Device Control 2 - Điều khiển thiết bị 2"),
        0x13: ("DC3", "Device Control 3 - Điều khiển thiết bị 3 (XOFF)"),
        0x14: ("DC4", "Device Control 4 - Điều khiển thiết bị 4"),
        0x15: ("NAK", "Negative Acknowledge - Từ chối/Lỗi"),
        0x16: ("SYN", "Synchronous Idle - Đồng bộ hóa, thường là sync pattern trong ITM"),
        0x17: ("ETB", "End of Transmission Block - Kết thúc khối truyền"),
        0x18: ("CAN", "Cancel - Hủy bỏ"),
        0x19: ("EM", "End of Medium - Kết thúc phương tiện"),
        0x1A: ("SUB", "Substitute - Thay thế"),
        0x1B: ("ESC", "Escape - Thoát"),
        0x1C: ("FS", "File Separator - Phân tách file"),
        0x1D: ("GS", "Group Separator - Phân tách nhóm"),
        0x1E: ("RS", "Record Separator - Phân tách bản ghi"),
        0x1F: ("US", "Unit Separator - Phân tách đơn vị"),
        0x7F: ("DEL", "Delete - Xóa")
    }
    
    for byte in raw_data:
        if byte in ascii_control_chars:
            name, description = ascii_control_chars[byte]
            result.append(f'[{name}]')
            special_chars_found.add(byte)
            explanations[byte] = (name, description)
        elif byte == 0x0A:  # Line Feed - hiển thị bình thường
            result.append('\n')
        elif byte == 0x0D:  # Carriage Return - hiển thị bình thường  
            result.append('\r')
        elif byte == 0x09:  # Tab - hiển thị bình thường
            result.append('\t')
        elif 32 <= byte <= 126:  # Ký tự ASCII có thể in
            result.append(chr(byte))
        elif byte >= 128:  # Extended ASCII hoặc các byte đặc biệt
            result.append(f'[0x{byte:02X}]')
            special_chars_found.add(byte)
            if byte == 0xF0:
                explanations[byte] = ("0xF0", "Byte đặc biệt - có thể là ITM packet header hoặc timestamp marker")
            elif byte >= 0xF0:
                explanations[byte] = (f"0x{byte:02X}", f"Extended byte 0x{byte:02X} - thường là ITM protocol overhead hoặc timestamp data")
            else:
                explanations[byte] = (f"0x{byte:02X}", f"Extended ASCII hoặc binary data: {byte}")
        else:
            result.append(f'[0x{byte:02X}]')
            special_chars_found.add(byte)
            explanations[byte] = (f"0x{byte:02X}", f"Control character: {byte}")
    
    # Tạo phần giải thích
    explanation_text = []
    
    if special_chars_found:
        explanation_text.append("📋 CÁC KÝ TỰ ĐẶC BIỆT ĐƯỢC TÌM THẤY:\n")
        
        # Sắp xếp theo byte value
        for byte in sorted(special_chars_found):
            if byte in explanations:
                name, description = explanations[byte]
                explanation_text.append(f"• {name} (0x{byte:02X} = {byte:3d}): {description}")
        
        explanation_text.append(f"\n📊 THỐNG KÊ:")
        explanation_text.append(f"• Tổng số ký tự đặc biệt: {len(special_chars_found)}")
        
        # Phân loại
        control_chars = [b for b in special_chars_found if b < 32]
        extended_chars = [b for b in special_chars_found if b >= 128]
        
        if control_chars:
            explanation_text.append(f"• Ký tự điều khiển ASCII (0x00-0x1F): {len(control_chars)}")
        if extended_chars:
            explanation_text.append(f"• Byte mở rộng (0x80-0xFF): {len(extended_chars)}")
            
        explanation_text.append(f"\n💡 NHẬN ĐỊNH CHO ITM TRACE:")
        if 0x10 in special_chars_found:  # DLE
            explanation_text.append("• DLE (0x10) xuất hiện → Đây có thể là ITM protocol framing")
        if 0x16 in special_chars_found:  # SYN
            explanation_text.append("• SYN (0x16) xuất hiện → Có sync pattern, điển hình của ITM stream")
        if any(b >= 0xF0 for b in special_chars_found):
            explanation_text.append("• Byte 0xF0-0xFF xuất hiện → Có thể là timestamp hoặc packet header")
        if 0x01 in special_chars_found:  # SOH
            explanation_text.append("• SOH (0x01) xuất hiện → Có thể là timestamp markers")
            
        explanation_text.append(f"\n🔧 GỢ Ý XỬ LÝ:")
        explanation_text.append("• Để lấy chỉ text thuần: chọn chế độ 'itm_clean'")
        explanation_text.append("• Để debug protocol: giữ nguyên chế độ này")
        explanation_text.append("• DLE thường bao quanh data payload trong ITM")
        explanation_text.append("• SYN thường là sync pattern giữa các packet")
        
    else:
        explanation_text.append("✅ Không tìm thấy ký tự đặc biệt nào - dữ liệu đã sạch!")
    
    return ''.join(result), '\n'.join(explanation_text)
    """Chế độ dành riêng cho ITM trace - chỉ lấy text thuần túy"""
    result = []
    
    for byte in raw_data:
        # Chỉ giữ lại ký tự có thể in và một số ký tự whitespace cơ bản
        if byte == 0x0A:  # Line Feed
            result.append('\n')
        elif byte == 0x0D:  # Carriage Return  
            result.append('\r')
        elif byte == 0x09:  # Tab
            result.append('\t')
        elif byte == 0x20:  # Space
            result.append(' ')
        elif 33 <= byte <= 126:  # Ký tự có thể in (không bao gồm space)
            result.append(chr(byte))
        # Tất cả các byte khác đều bị bỏ qua hoàn toàn
    
    # Làm sạch kết quả: loại bỏ khoảng trắng thừa và dòng trống
    lines = ''.join(result).split('\n')
    cleaned_lines = []
    
    for line in lines:
        # Loại bỏ khoảng trắng ở đầu và cuối dòng
        cleaned_line = line.strip()
        # Chỉ giữ dòng có nội dung
        if cleaned_line:
            cleaned_lines.append(cleaned_line)
    
    return '\n'.join(cleaned_lines)
    """Giải mã tùy chỉnh - Tối ưu cho ITM trace của STM32"""
    result = []
    
    # Bảng mapping tùy chỉnh cho ITM
    custom_mapping = {
        0x01: '',          # SOH -> bỏ qua (có thể là timestamp)
        0x02: '',          # STX -> bỏ qua
        0x03: '',          # ETX -> bỏ qua
        0x04: '',          # EOT -> bỏ qua
        0x05: '',          # ENQ -> bỏ qua
        0x06: '',          # ACK -> bỏ qua
        0x07: '',          # Bell -> bỏ qua
        0x08: '',          # Backspace -> bỏ qua
        0x0C: '',          # Form Feed -> bỏ qua
        0x0E: '',          # Shift Out -> bỏ qua
        0x0F: '',          # Shift In -> bỏ qua
        0x10: '',          # DLE -> bỏ qua (thường là control byte của ITM)
        0x11: '',          # DC1 -> bỏ qua
        0x12: '',          # DC2 -> bỏ qua
        0x13: '',          # DC3 -> bỏ qua
        0x14: '',          # DC4 -> bỏ qua
        0x15: '',          # NAK -> bỏ qua
        0x16: '',          # SYN -> bỏ qua
        0x17: '',          # ETB -> bỏ qua
        0x18: '',          # CAN -> bỏ qua
        0x19: '',          # EM -> bỏ qua
        0x1A: '',          # SUB -> bỏ qua
        0x1B: '',          # ESC -> bỏ qua
        0x1C: '',          # FS -> bỏ qua
        0x1D: '',          # GS -> bỏ qua
        0x1E: '',          # RS -> bỏ qua
        0x1F: '',          # US -> bỏ qua
        0xF0: '',          # Extended byte -> bỏ qua
        0xF1: '',          # Extended byte -> bỏ qua
        0xF2: '',          # Extended byte -> bỏ qua
        0xF3: '',          # Extended byte -> bỏ qua
        0xF4: '',          # Extended byte -> bỏ qua
        0xF5: '',          # Extended byte -> bỏ qua
        0xF6: '',          # Extended byte -> bỏ qua
        0xF7: '',          # Extended byte -> bỏ qua
        0xF8: '',          # Extended byte -> bỏ qua
        0xF9: '',          # Extended byte -> bỏ qua
        0xFA: '',          # Extended byte -> bỏ qua
        0xFB: '',          # Extended byte -> bỏ qua
        0xFC: '',          # Extended byte -> bỏ qua
        0xFD: '',          # Extended byte -> bỏ qua
        0xFE: '',          # Extended byte -> bỏ qua
        0xFF: '',          # Extended byte -> bỏ qua
    }
    
    for byte in raw_data:
        if byte in custom_mapping:
            result.append(custom_mapping[byte])
        elif byte == 0x0A:  # Line Feed
            result.append('\n')
        elif byte == 0x0D:  # Carriage Return
            result.append('\r')
        elif byte == 0x09:  # Tab
            result.append('\t')
        elif 32 <= byte <= 126:  # Ký tự ASCII có thể in
            result.append(chr(byte))
        else:
            continue  # Bỏ qua các byte khác
    
    return ''.join(result)

def analyze_file_bytes(input_file):
    """Phân tích các byte trong file để hiểu cấu trúc"""
    try:
        with open(input_file, 'rb') as f:
            raw_data = f.read()
        
        print(f"\nPHÂN TÍCH FILE: {input_file}")
        print(f"Tổng số byte: {len(raw_data)}")
        
        # Đếm tần suất các byte
        byte_count = {}
        for byte in raw_data:
            byte_count[byte] = byte_count.get(byte, 0) + 1
        
        # Hiển thị các byte không phải ASCII thường
        special_bytes = {}
        for byte, count in byte_count.items():
            if byte < 32 or byte > 126:
                special_bytes[byte] = count
        
        print("\nCác byte đặc biệt (không phải ASCII thường):")
        for byte in sorted(special_bytes.keys()):
            count = special_bytes[byte]
            if byte < 128:
                name = get_ascii_name(byte)
                print(f"  0x{byte:02X} ({byte:3d}) - {name}: {count} lần")
            else:
                print(f"  0x{byte:02X} ({byte:3d}) - Extended: {count} lần")
        
    except Exception as e:
        print(f"Lỗi phân tích file: {e}")

def get_ascii_name(byte):
    """Trả về tên của ký tự ASCII điều khiển"""
    ascii_names = {
        0: "NULL", 1: "SOH", 2: "STX", 3: "ETX", 4: "EOT", 5: "ENQ",
        6: "ACK", 7: "BELL", 8: "BS", 9: "TAB", 10: "LF", 11: "VT",
        12: "FF", 13: "CR", 14: "SO", 15: "SI", 16: "DLE", 17: "DC1",
        18: "DC2", 19: "DC3", 20: "DC4", 21: "NAK", 22: "SYN", 23: "ETB",
        24: "CAN", 25: "EM", 26: "SUB", 27: "ESC", 28: "FS", 29: "GS",
        30: "RS", 31: "US", 127: "DEL"
    }
    return ascii_names.get(byte, "UNKNOWN")

# CHƯƠNG TRÌNH CHÍNH
if __name__ == "__main__":
    # Cấu hình đường dẫn
    input_file = "D:/STM32_WORKSPACE/STM32F411_Myproject/output_itm.hex"
    output_file = "D:/STM32_WORKSPACE/STM32F411_Myproject/ITM_decoded.txt"
    
    # Kiểm tra file tồn tại
    if not os.path.exists(input_file):
        print(f"File không tồn tại: {input_file}")
        print("Vui lòng kiểm tra đường dẫn và thử lại.")
        exit(1)
    
    # Phân tích file trước
    analyze_file_bytes(input_file)
    
    print("\n" + "="*60)
    print("CHỌN CHE ĐỘ GIẢI MÃ:")
    print("1. comprehensive - Xử lý toàn diện")
    print("2. printable_only - Chỉ ký tự có thể in")
    print("3. custom - Tùy chỉnh (bỏ qua control bytes)")
    print("4. itm_clean - ITM sạch (chỉ text thuần)")
    print("5. explained - Hiển thị + Giải thích chi tiết (khuyến nghị cho debug)")
    print("6. visual - Hiển thị trực quan (hex + text, dễ đọc)")
    print("="*60)
    
    choice = input("Nhập lựa chọn (1-6) hoặc Enter để chọn mặc định (5): ").strip()
    
    mode_map = {
        '1': 'comprehensive', 
        '2': 'printable_only', 
        '3': 'custom', 
        '4': 'itm_clean',
        '5': 'explained',
        '6': 'visual',
        '': 'explained'
    }
    decode_mode = mode_map.get(choice, 'explained')
    
    # Thực hiện giải mã
    result = decode_itm_file(input_file, output_file, decode_mode)
    
    if result:
        print(f"\nHoàn thành! Kết quả đã được lưu vào: {output_file}")
    else:
        print("\nCó lỗi xảy ra trong quá trình xử lý.")