import re

def clean_itm_file(input_file, output_file=None):
    # Đọc nội dung file
    with open(input_file, 'r', encoding='ascii', errors='ignore') as f:
        content = f.read()
    
    # Loại bỏ ký tự SOH (\x01)
    cleaned_content = re.sub(r'\x01', '', content)
    
    # In ra nội dung đã làm sạch
    print("Cleaned content:")
    print(cleaned_content)
    
    # Lưu vào file mới nếu output_file được chỉ định
    if output_file:
        with open(output_file, 'w', encoding='ascii') as f:
            f.write(cleaned_content)
    
    return cleaned_content

# Sử dụng hàm
input_file = "D:/STM32_WORKSPACE/STM32F411_Myproject/output_itm.txt"
output_file = "D:/STM32_WORKSPACE/STM32F411_Myproject/ITM_cleaned.txt"
clean_itm_file(input_file, output_file)