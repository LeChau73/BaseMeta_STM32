Các milestonds cho tạo ra RTOS custom đơn giản

1) Khi tạo 1 task phải làm gì?

//TODO:
Case Test: Check xem allocate vào đúng heap tự custom chưa
Check địa chỉ start đúng chưa
Check linker list đúng chưa
Check khởi tạo stack ban đầu đúng chưa

2) Khởi tạo Systick và viết hàm swtich MSP to PSP

3) Push và Pop stack khi context switch

4) Viết schedule 
    - Có tính năng gì
    - Sử dụng thuật toán nào

=> Mục tiêu là ít nhất 2 task switch qua lại được là được đầu tiên

