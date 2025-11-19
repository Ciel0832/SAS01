# Dockerfile

# 1. CHỌN IMAGE CƠ SỞ (BASE IMAGE)
# Sử dụng Image của GCC (GNU Compiler Collection) có sẵn trình biên dịch C++
FROM gcc:11.4-bullseye

# 2. THIẾT LẬP THƯ MỤC LÀM VIỆC
# Đặt thư mục làm việc mặc định bên trong container
WORKDIR /app

# 3. SAO CHÉP MÃ NGUỒN VÀ DỮ LIỆU
# Sao chép tất cả các file (bao gồm .cpp, .h, .csv, /third_party)
# từ máy tính của bạn (dấu chấm .) vào thư mục /app trong container.
COPY . /app

# 4. BIÊN DỊCH ỨNG DỤNG
# Thực thi lệnh biên dịch. Đây là bước quan trọng nhất:
# - sas_app: Tên file thực thi đầu ra.
# - Dùng cờ -std=c++17 là BẮT BUỘC để hỗ trợ std::optional.
# - -Wall: Bật tất cả các cảnh báo.
RUN g++ -o sas_app main.cpp sas.cpp storage.cpp qr.cpp -std=c++17 -Wall

# 5. ĐỊNH NGHĨA LỆNH CHẠY
# Lệnh sẽ được chạy tự động khi Container khởi động, mở ứng dụng console của bạn.
CMD ["./sas_app"]