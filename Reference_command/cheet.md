Ok, mình liệt kê thêm mấy **ký tự đặc biệt hay dùng trong macro C** để bạn dễ nhớ nhé:

### 1. `#` – Stringize (chuỗi hóa tham số)

* Biến tham số macro thành **chuỗi**.


#define TO_STRING(x) #x

printf("%s\n", TO_STRING(Hello RTT)); 
// Output: Hello RTT

### 2. `##` – Token Pasting (ghép chuỗi token)

* Dùng để **nối** các token lại với nhau.


#define MAKE_VAR(name, num) name##num

int var1 = 100;
printf("%d\n", MAKE_VAR(var, 1)); 
// -> var1 => 100

### 3. `__VA_ARGS__` – Variadic Macro (macro tham số biến đổi)

* Đại diện cho **danh sách tham số còn lại**.


#define LOG(fmt, ...) printf("LOG: " fmt "\n", __VA_ARGS__)

LOG("%d + %d = %d", 2, 3, 5);
// Output: LOG: 2 + 3 = 5

### 4. `#define` với macro nhiều dòng (`\`)

* Cho phép viết macro trên nhiều dòng, thường dùng cho code dài.


#define SQUARE_AND_PRINT(x)   \
    do {                      \
        int y = (x) * (x);    \
        printf("%d\n", y);    \
    } while(0)

### 5. Một số macro sẵn có trong C (predefined macros):

* `__FILE__` → tên file hiện tại.
* `__LINE__` → số dòng trong file.
* `__DATE__` → ngày biên dịch.
* `__TIME__` → giờ biên dịch.


printf("Compiled at %s on %s:%d\n", __TIME__, __FILE__, __LINE__);

👉 Như vậy trong macro có 3 "ký tự quyền lực":

* `#` → biến tham số thành chuỗi
* `##` → nối token lại với nhau
* `__VA_ARGS__` → đại diện cho danh sách tham số còn lại

