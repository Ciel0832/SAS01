// main.cpp
#define _CRT_SECURE_NO_WARNINGS 
#include "sas.h"
#include "models.h"
#include <iostream>
#include <string> 
#include <limits>
#include <cstdlib> 
#include <iomanip>
#include <algorithm> 

// --- Hàm chung (clearScreen, pause, getRoleName, getStatusName) ---
void clearScreen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

void pause() {
    std::cout << "\nNhan Enter de tiep tuc...";
    if (std::cin.peek() == '\n') { std::cin.ignore(); }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string getRoleName(Role r) {
    if (r == Role::ADMIN) return "Admin";
    if (r == Role::LECTURER) return "Giang Vien";
    return "Sinh Vien";
}

std::string getStatusName(AttendanceStatus s) {
    if (s == AttendanceStatus::PRESENT) return "Co Mat";
    if (s == AttendanceStatus::ABSENT) return "Vang Mat";
    if (s == AttendanceStatus::LATE) return "Di Tre";
    if (s == AttendanceStatus::EXCUSED) return "Co Phep";
    return "Chua Diem Danh";
}

// --- MENU CON: QUẢN LÝ KHÓA HỌC (ADMIN) ---
void courseManagementMenu(SAS& sas) {
    while (true) {
        clearScreen();
        std::cout << "=== QUAN LY KHOA HOC (ADMIN) ===\n";
        std::cout << "1. Them mon hoc/lop hoc moi\n"; // Req 2.2
        std::cout << "2. Them sinh vien vao lop\n";   // Req 2.2 (phu)
        std::cout << "3. Xem danh sach tat ca khoa hoc\n"; // Req 2.2 (phu)
        std::cout << "0. Quay lai\n";
        std::cout << "Chon: ";
        int choice;
        if (!(std::cin >> choice)) { std::cin.clear(); std::cin.ignore(1000, '\n'); continue; }

        if (choice == 0) break;
        else if (choice == 1) { // Them mon hoc moi (Req 2.2)
            std::string id, name, lecturerId;
            std::cout << "Nhap Course ID (VD: C0002): "; std::cin >> id;
            std::cout << "Nhap Ten Mon hoc: "; std::cin.ignore(); std::getline(std::cin, name);
            std::cout << "Nhap ID Giang vien (VD: U0002): "; std::cin >> lecturerId;

            Course newCourse;
            newCourse.courseId = id;
            newCourse.courseName = name;
            newCourse.lecturerId = lecturerId;

            if (sas.createCourse(newCourse))
                std::cout << "Tao khoa hoc " << id << " THANH CONG.\n";
            else
                std::cout << "Tao khoa hoc THAT BAI (ID da ton tai?).\n";
            pause();
        }
        else if (choice == 2) { // Them sinh vien vao lop
            std::string courseId, studentId;
            std::cout << "Nhap Course ID: "; std::cin >> courseId;
            std::cout << "Nhap Student ID can them: "; std::cin >> studentId;

            if (sas.addStudentToCourse(courseId, studentId))
                std::cout << "Them SV " << studentId << " vao lop " << courseId << " THANH CONG.\n";
            else
                std::cout << "Them SV THAT BAI (Sai Course ID/Student ID, hoac SV da co trong lop).\n";
            pause();
        }
        else if (choice == 3) { // Xem danh sach tat ca khoa hoc
            auto courses = sas.listCourses();
            std::cout << "\n--- DANH SACH KHOA HOC (" << courses.size() << ") ---\n";
            for (const auto& c : courses) {
                std::cout << "ID: " << c.courseId << " | Ten: " << c.courseName
                    << " | GV: " << c.lecturerId << " | Si so: " << c.studentIds.size() << "\n";
            }
            pause();
        }
    }
}

// --- MENU CON: QUẢN LÝ NGƯỜI DÙNG (ADMIN) ---
void userManagementMenu(SAS& sas) {
    while (true) {
        clearScreen();
        std::cout << "=== QUAN LY NGUOI DUNG ===\n";
        std::cout << "1. Tao tai khoan moi\n"; // Req 2.1
        std::cout << "2. Xoa tai khoan (Theo ID)\n"; // Req 2.1
        std::cout << "3. Xem danh sach tat ca nguoi dung\n";
        std::cout << "0. Quay lai\n";
        std::cout << "Chon: ";
        int choice;
        if (!(std::cin >> choice)) { std::cin.clear(); std::cin.ignore(1000, '\n'); continue; }

        if (choice == 0) break;
        else if (choice == 1) { // Tao tai khoan moi (Req 2.1)
            std::string id, name, email, pass; int roleInt;
            std::cout << "Nhap ID (VD: U0007): "; std::cin >> id;
            std::cout << "Nhap Ten day du: "; std::cin.ignore(); std::getline(std::cin, name);
            std::cout << "Nhap Email: "; std::cin >> email;
            std::cout << "Nhap Mat khau: "; std::cin >> pass;
            std::cout << "Chon Vai tro (0=ADMIN, 1=GIANG VIEN, 2=SINH VIEN): ";
            if (!(std::cin >> roleInt) || roleInt < 0 || roleInt > 2) {
                std::cout << "Vai tro khong hop le.\n"; pause(); continue;
            }

            User newUser(id, name, email, pass, (Role)roleInt);
            if (sas.createUser(newUser))
                std::cout << "Tao tai khoan " << id << " THANH CONG.\n";
            else
                std::cout << "Tao tai khoan THAT BAI (ID da ton tai?).\n";
            pause();
        }
        else if (choice == 2) { // Xoa tai khoan (Req 2.1)
            std::string id;
            std::cout << "Nhap ID tai khoan can xoa: "; std::cin >> id;
            if (sas.removeUser(id))
                std::cout << "Xoa tai khoan " << id << " THANH CÔÓNG.\n";
            else
                std::cout << "Xoa tai khoan THAT BAI (Khong tim thay ID).\n";
            pause();
        }
        else if (choice == 3) { // Xem danh sach
            auto users = sas.listUsers();
            std::cout << "\n--- DANH SACH NGUOI DUNG (" << users.size() << ")\n";
            for (const auto& u : users) {
                std::cout << "ID: " << u.id << " | Ten: " << u.fullName
                    << " | Email: " << u.email << " | Role: " << getRoleName(u.role) << "\n";
            }
            pause();
        }
    }
}

// --- MENU ADMIN (QUẢN TRỊ VIÊN) ---
void adminMenu(SAS& sas, const User& user) {
    while (true) {
        clearScreen();
        std::cout << "=== MENU ADMIN (" << user.fullName << ") ===\n";
        std::cout << "1. Quan ly Nguoi dung (Tao/Xoa tai khoan)\n"; // Req 2.1
        std::cout << "2. Quan ly Khoa hoc (Them/Xem)\n";         // Req 2.2 ⬅️ Đã bổ sung
        std::cout << "3. Xuat bao cao tong hop\n";              // Req 2.5
        std::cout << "0. Dang xuat\n";
        std::cout << "Chon: ";
        int choice;
        if (!(std::cin >> choice)) { std::cin.clear(); std::cin.ignore(1000, '\n'); continue; }

        if (choice == 0) break;
        else if (choice == 1) { // Quan ly nguoi dung
            userManagementMenu(sas);
        }
        else if (choice == 2) { // Quan ly khoa hoc (Admin)
            courseManagementMenu(sas); // ⬅️ GỌI MENU QUẢN LÝ KHÓA HỌC
        }
        // ... (Các chức năng khác)
    }
}

// --- MENU SINH VIÊN (Cập nhật Req 2.6) ---
void studentMenu(SAS& sas, const User& user) {
    while (true) {
        clearScreen();
        std::cout << "=== MENU SINH VIEN (" << user.fullName << ") ===\n";

        // Req 2.6: Hiển thị cảnh báo
        std::vector<std::string> warnings = sas.getLowAttendanceWarnings(user.id, 70.0); // Mức cảnh báo 70%
        if (!warnings.empty()) {
            std::cout << "\n!!! CANH BAO DIEM DANH THAP !!!\n";
            std::cout << "Ty le duoi 70% o cac mon:\n";
            for (const auto& w : warnings) std::cout << " - " << w << "\n";
            std::cout << "---------------------------------\n";
        }

        std::cout << "1. Xem Khoa hoc & Ti le Diem danh\n";
        std::cout << "2. Diem danh (OTP)\n";
        std::cout << "3. Diem danh (TOTP - Chua trien khai day du)\n";
        std::cout << "0. Dang xuat\n";
        std::cout << "Chon: ";
        int choice;
        if (!(std::cin >> choice)) { std::cin.clear(); std::cin.ignore(1000, '\n'); continue; }

        if (choice == 0) break;
        else if (choice == 1) {
            auto courses = sas.listCoursesOfStudent(user.id);
            std::cout << "\n--- Danh sach khoa hoc ---\n";
            for (const auto& c : courses) {
                double pct = sas.getStudentAttendancePercentage(user.id, c.courseId);
                std::cout << "Mon: " << c.courseId << " - " << c.courseName
                    << " | Ti le: " << std::fixed << std::setprecision(1) << pct << "%\n";
            }
            pause();
        }
        else if (choice == 2) {
            std::string sid, otp;
            std::cout << "Nhap Session ID: "; std::cin >> sid;
            std::cout << "Nhap OTP giang vien cung cap: "; std::cin >> otp;
            if (sas.studentCheckInWithOTP(sid, user.id, otp))
                std::cout << "Diem danh THANH CONG!\n";
            else
                std::cout << "Diem danh THAT BAI (Sai Session/OTP hoac da het gio/da diem danh)!\n";
            pause();
        }
    }
}

// --- MENU GIẢNG VIÊN (Cập nhật Req 2.4 & 2.5) ---
void lecturerMenu(SAS& sas, const User& user) {
    while (true) {
        clearScreen();
        std::cout << "=== MENU GIANG VIEN (" << user.fullName << ") ===\n";
        std::cout << "1. Xem danh sach lop day\n";
        std::cout << "2. Tao phien diem danh moi (OTP/QR)\n";
        std::cout << "3. Dong phien diem danh\n";
        std::cout << "4. Chinh sua diem danh (Req 2.4)\n";
        std::cout << "0. Dang xuat\n";
        std::cout << "Chon: ";
        int choice;
        if (!(std::cin >> choice)) { std::cin.clear(); std::cin.ignore(1000, '\n'); continue; }

        if (choice == 0) break;
        else if (choice == 1) {
            auto courses = sas.listCoursesOfLecturer(user.id);
            std::cout << "\n--- Danh sach khoa hoc giang day ---\n";
            for (const auto& c : courses) {
                std::cout << "ID: " << c.courseId << " - " << c.courseName << " (Si so: " << c.studentIds.size() << ")\n";
            }
            pause();
        }
        else if (choice == 2) {
            std::string cid; int mins; int type;
            std::cout << "Nhap CourseID: "; std::cin >> cid;
            std::cout << "Thoi luong (phut): ";
            if (!(std::cin >> mins)) { std::cin.clear(); std::cin.ignore(1000, '\n'); continue; }

            std::cout << "Chon phuong thuc (1: OTP tinh, 2: QR/TOTP): ";
            if (!(std::cin >> type) || (type != 1 && type != 2)) {
                std::cout << "Lua chon khong hop le.\n"; pause(); continue;
            }

            bool useQr = (type == 2);
            std::string sid = sas.createSession(cid, mins, useQr);

            if (!sid.empty()) {
                auto sess = sas.getSession(sid);
                std::cout << "\nDa tao phien: " << sid << "\n";
                if (sess) {
                    if (useQr) {
                        std::cout << "=== DIEM DANH QR/TOTP ===\n";
                        std::cout << "1. File QR Code: " << sess->qrFilename
                            << " da duoc tao (trong thu muc chay).\n";
                        std::cout << "2. Sinh vien quet ma nay de co TOTP Code.\n";
                        std::cout << "Phien se ket thuc luc: " << std::ctime(&sess->endTime);
                    }
                    else {
                        std::cout << "OTP CODE: " << sess->otp << "\n(Cung cap ma nay cho SV)\n";
                        std::cout << "Phien se ket thuc luc: " << std::ctime(&sess->endTime);
                    }
                }
            }
            else {
                std::cout << "Tao phien THAT BAI (Khong tim thay CourseID).\n";
            }
            pause();
        }
        else if (choice == 4) { // Req 2.4 - Chỉnh sửa điểm danh
            std::string sid, stid; int st;
            std::cout << "Nhap Session ID: "; std::cin >> sid;
            std::cout << "Nhap Student ID can sua: "; std::cin >> stid;
            std::cout << "Trang thai moi (1=Co Mat, 2=Vang Mat, 3=Di Tre, 4=Co Phep): ";
            if (!(std::cin >> st)) { std::cin.clear(); std::cin.ignore(1000, '\n'); continue; }

            AttendanceStatus stat = AttendanceStatus::UNKNOWN;
            if (st == 1) stat = AttendanceStatus::PRESENT;
            else if (st == 2) stat = AttendanceStatus::ABSENT;
            else if (st == 3) stat = AttendanceStatus::LATE;
            else if (st == 4) stat = AttendanceStatus::EXCUSED;

            if (sas.updateAttendanceStatus(sid, stid, stat))
                std::cout << "Cap nhat thanh cong cho SV " << stid << " thanh: " << getStatusName(stat) << "!\n";
            else
                std::cout << "Cap nhat THAT BAI (Khong tim thay Phien/Sai StudentID).\n";
            pause();
        }
    }
}

// --- MAIN ---
int main() {
    SAS sas;

    // Khởi tạo dữ liệu: Cố gắng tải từ JSON trước. Nếu thất bại, nạp từ CSV.
    if (!sas.loadData("data")) {
        std::cout << "Khong tim thay data cu (data.json), dang khoi tao tu CSV...\n";

        // 1. Nạp người dùng từ data_users.csv
        if (sas.importUsers("data_users.csv")) {
            std::cout << "-> Da nap thanh cong du lieu nguoi dung.\n";
        }
        else {
            std::cerr << "!!! LOI: Khong the mo hoac nap data_users.csv\n";
        }

        // 2. Nạp khóa học từ data_courses.csv
        if (sas.importCourses("data_courses.csv")) {
            std::cout << "-> Da nap thanh cong du lieu khoa hoc.\n";
        }
        else {
            std::cerr << "!!! LOI: Khong the mo hoac nap data_courses.csv\n";
        }

        // 3. Lưu dữ liệu đã nạp vào JSON để sử dụng cho lần sau
        if (sas.saveData("data")) {
            std::cout << "-> Da luu du lieu moi vao data.json.\n";
        }
        else {
            std::cerr << "!!! LOI: Khong the luu du lieu vao data.json. Kiem tra thu vien JSON.\n";
        }

        pause(); // Tạm dừng để người dùng thấy thông báo
    }

    while (true) {
        clearScreen();
        std::cout << "=== HE THONG DIEM DANH SAS ===\n";
        std::cout << "1. Dang nhap\n0. Thoat\nChon: ";
        int ch;
        if (!(std::cin >> ch)) { std::cin.clear(); std::cin.ignore(1000, '\n'); continue; }

        if (ch == 0) { sas.saveData("data"); break; }

        if (ch == 1) {
            std::string email, pass;
            std::cout << "Email/Ma so: "; std::cin >> email;
            std::cout << "Password: "; std::cin >> pass;

            auto u = sas.login(email, pass);
            if (u.has_value()) {
                std::cout << "Dang nhap thanh cong! Vai tro: " << getRoleName(u->role) << "\n";
                // Gọi Admin Menu
                if (u->role == Role::ADMIN) {
                    adminMenu(sas, *u);
                }
                else if (u->role == Role::LECTURER) lecturerMenu(sas, *u);
                else studentMenu(sas, *u);
            }
            else {
                std::cout << "Dang nhap that bai (Sai tai khoan/mat khau)!\n";
                pause();
            }
        }
    }
    return 0;
}