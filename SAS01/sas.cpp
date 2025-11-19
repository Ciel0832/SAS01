// sas.cpp
#include "sas.h"
// #include "totp.h" // Uncomment this if you implement TOTP logic
#include "qr.h"   
#include "storage.h" // Sử dụng Storage để tải/lưu dữ liệu
#include <fstream>
#include <sstream>
#include <iomanip>
#include <random> // Dùng cho tạo OTP
#include <chrono>
#include <iostream> 
#include <algorithm> 
#include <cmath>
#include <stdexcept>

// --- Helper Functions (Hàm phụ trợ) ---

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (std::string::npos == first) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

std::vector<std::string> SAS::split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    return tokens;
}

std::string getStatusString(AttendanceStatus s) {
    if (s == AttendanceStatus::PRESENT) return "PRESENT";
    if (s == AttendanceStatus::ABSENT) return "ABSENT";
    if (s == AttendanceStatus::LATE) return "LATE";
    if (s == AttendanceStatus::EXCUSED) return "EXCUSED";
    return "UNKNOWN";
}

// Constructor
SAS::SAS() {}

// === 1. DATA PERSISTENCE & IMPORT ===

bool SAS::loadData(const std::string& prefix) {
    Storage storage(prefix + ".json");
    if (!storage.load()) return false;

    // Chuyển dữ liệu từ vector của Storage sang map của SAS
    users.clear(); courses.clear(); sessions.clear();
    for (const auto& u : storage.users) users[u.id] = u;
    for (const auto& c : storage.courses) courses[c.courseId] = c;
    for (const auto& s : storage.sessions) sessions[s.sessionId] = s;

    // Cập nhật sequence
    int maxU = 0, maxC = 0, maxS = 0;
    for (const auto& kv : users) if (kv.first[0] == 'U') maxU = std::max(maxU, std::stoi(kv.first.substr(1)));
    for (const auto& kv : courses) if (kv.first[0] == 'C') maxC = std::max(maxC, std::stoi(kv.first.substr(1)));
    for (const auto& kv : sessions) if (kv.first[0] == 'S') maxS = std::max(maxS, std::stoi(kv.first.substr(1)));
    nextUserSeq = maxU + 1;
    nextCourseSeq = maxC + 1;
    nextSessionSeq = maxS + 1;

    return true;
}

bool SAS::saveData(const std::string& prefix) const {
    Storage storage(prefix + ".json");

    // Chuyển dữ liệu từ map của SAS sang vector của Storage
    storage.users.clear(); storage.courses.clear(); storage.sessions.clear();
    for (const auto& kv : users) storage.users.push_back(kv.second);
    for (const auto& kv : courses) storage.courses.push_back(kv.second);
    for (const auto& kv : sessions) storage.sessions.push_back(kv.second);

    return storage.save();
}

bool SAS::importUsers(const std::string& filename) {
    std::ifstream ifs(filename);
    if (!ifs) return false;

    std::string line;
    std::getline(ifs, line); // Bỏ qua dòng tiêu đề

    while (std::getline(ifs, line)) {
        std::vector<std::string> parts = split(line, ',');
        if (parts.size() >= 5) {
            User u;
            u.id = parts[0];
            u.fullName = parts[1];
            u.email = parts[2];
            u.password = parts[3];

            try {
                int roleInt = std::stoi(parts[4]);
                if (roleInt == 0) u.role = Role::ADMIN;
                else if (roleInt == 1) u.role = Role::LECTURER;
                else u.role = Role::STUDENT;

                users[u.id] = u;
            }
            catch (...) { continue; }
        }
    }
    return true;
}

bool SAS::importCourses(const std::string& filename) {
    std::ifstream ifs(filename);
    if (!ifs) return false;

    std::string line;
    std::getline(ifs, line); // Bỏ qua dòng tiêu đề

    while (std::getline(ifs, line)) {
        std::vector<std::string> parts = split(line, ',');
        if (parts.size() >= 4) {
            Course c;
            c.courseId = parts[0];
            c.courseName = parts[1];
            c.lecturerId = parts[2];

            std::vector<std::string> studentList = split(parts[3], ';');
            for (const auto& sid : studentList) {
                if (!sid.empty()) {
                    c.studentIds.push_back(sid);
                }
            }
            courses[c.courseId] = c;
        }
    }
    return true;
}

// === 2. User & Course ===
bool SAS::createUser(const User& user) {
    if (users.count(user.id)) return false;
    users[user.id] = user;
    return true;
}
bool SAS::removeUser(const std::string& userId) {
    return users.erase(userId) > 0;
}
std::optional<User> SAS::getUser(const std::string& userId) const {
    if (users.count(userId)) return users.at(userId);
    return std::nullopt;
}
std::vector<User> SAS::listUsers() const {
    std::vector<User> out;
    for (const auto& kv : users) out.push_back(kv.second);
    return out;
}

std::optional<User> SAS::login(const std::string& email, const std::string& password) const {
    for (const auto& kv : users) {
        if ((kv.second.email == email || kv.second.id == email) && kv.second.password == password) {
            return kv.second;
        }
    }
    return std::nullopt;
}

bool SAS::createCourse(const Course& course) {
    if (courses.count(course.courseId)) return false;
    courses[course.courseId] = course;
    return true;
}
std::optional<Course> SAS::getCourse(const std::string& courseId) const {
    if (courses.count(courseId)) return courses.at(courseId);
    return std::nullopt;
}
std::vector<Course> SAS::listCourses() const {
    std::vector<Course> out;
    for (const auto& kv : courses) out.push_back(kv.second);
    return out;
}
bool SAS::addStudentToCourse(const std::string& courseId, const std::string& studentId) {
    if (courses.count(courseId) && users.count(studentId) && users.at(studentId).role == Role::STUDENT) {
        auto& students = courses[courseId].studentIds;
        if (std::find(students.begin(), students.end(), studentId) == students.end()) {
            students.push_back(studentId);
            return true;
        }
    }
    return false;
}

// === 3. Session & Attendance ===

std::string SAS::createSession(const std::string& courseId, int durationMinutes, bool useTotp) {
    if (courses.find(courseId) == courses.end()) return "";

    AttendanceSession sess;
    sess.sessionId = "S" + std::to_string(nextSessionSeq++);
    sess.courseId = courseId;
    sess.startTime = std::time(nullptr);
    sess.endTime = sess.startTime + (durationMinutes * 60);
    sess.open = true;

    if (useTotp) {
        // 1. Giả lập tạo TOTP Secret (Cần thư viện bên ngoài để làm thực tế)
        // Lưu ý: Trong một hệ thống thực tế, secret này phải được sinh ngẫu nhiên.
        sess.totpSecretBase32 = "ABCDEFGH12345678"; // <--- KEY GIẢ LẬP

        // 2. Tạo nội dung (URL) để mã hóa thành QR
        // Format chuẩn: otpauth://totp/SAS:CourseID?secret=SECRET&issuer=SAS
        std::string qr_content = "otpauth://totp/SAS:" + sess.courseId +
            "?secret=" + sess.totpSecretBase32 +
            "&issuer=SAS_System";

        sess.qrFilename = sess.sessionId + "_" + sess.courseId + ".png";

        // 3. Gọi hàm tạo QR (từ qr.cpp)
        // Nếu generate_qr_png thất bại (do thiếu qrencode trên máy), nó sẽ dùng ASCII fallback.
        if (generate_qr_png(qr_content, sess.qrFilename)) {
            // File QR PNG đã được tạo thành công
            std::cout << "\n[QR] Da tao file: " << sess.qrFilename << std::endl;
        }
        else {
            // Fallback: In ra mã ASCII
            std::string ascii_output;
            generate_qr_ascii(qr_content, ascii_output);
            std::cerr << "\n[QR] Khong the tao file PNG. ASCII fallback:\n" << ascii_output << "\n";
        }
    }
    else {
        // Tạo OTP tĩnh đơn giản 6 chữ số
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(100000, 999999);
        sess.otp = std::to_string(distrib(gen));
    }

    sessions[sess.sessionId] = sess;
    return sess.sessionId;

}

bool SAS::closeSession(const std::string& sessionId) {
    if (sessions.count(sessionId)) {
        sessions[sessionId].open = false;
        return true;
    }
    return false;
}

std::optional<AttendanceSession> SAS::getSession(const std::string& sessionId) const {
    if (sessions.count(sessionId)) {
        return sessions.at(sessionId);
    }
    return std::nullopt;
}

std::vector<AttendanceSession> SAS::listSessions() const {
    std::vector<AttendanceSession> out;
    for (const auto& kv : sessions) out.push_back(kv.second);
    return out;
}

std::vector<AttendanceSession> SAS::listSessionsByCourse(const std::string& courseId) const {
    std::vector<AttendanceSession> out;
    for (const auto& kv : sessions) {
        if (kv.second.courseId == courseId) {
            out.push_back(kv.second);
        }
    }
    return out;
}

// [Cập nhật] studentCheckInWithOTP (Chống gian lận)
bool SAS::studentCheckInWithOTP(const std::string& sessionId, const std::string& studentId, const std::string& otp) {
    if (sessions.find(sessionId) == sessions.end()) return false;
    auto& sess = sessions[sessionId];

    // Chống gian lận: Kiểm tra session còn mở không và chưa hết giờ
    if (!sess.open || std::time(nullptr) > sess.endTime) {
        if (sess.open) sess.open = false;
        return false;
    }
    // Kiểm tra sinh viên đã điểm danh chưa
    if (sess.records.count(studentId) && sess.records.at(studentId).status != AttendanceStatus::UNKNOWN) return false;

    // Validate OTP
    if (sess.otp != otp) return false;

    // Ghi nhận
    AttendanceRecord rec;
    rec.studentId = studentId;
    rec.status = AttendanceStatus::PRESENT;
    rec.timestamp = std::time(nullptr);
    sess.records[studentId] = rec;

    return true;
}
bool SAS::studentCheckInWithTOTP(const std::string& sessionId, const std::string& studentId, const std::string& totp_str) {
    // Logic TOTP: Cần triển khai đầy đủ TOTP.H/CPP. Hiện tại chỉ là placeholder.
    std::cerr << "TOTP not fully implemented.\n";
    return false;
}

// [Cập nhật] updateAttendanceStatus (Req 2.4 - Chỉnh sửa điểm danh)
bool SAS::updateAttendanceStatus(const std::string& sessionId, const std::string& studentId, AttendanceStatus newStatus) {
    if (sessions.find(sessionId) == sessions.end()) return false;

    AttendanceSession& sess = sessions[sessionId];

    // Tạo record nếu chưa có, hoặc update nếu đã có
    AttendanceRecord& record = sess.records[studentId];
    record.studentId = studentId;
    record.status = newStatus;
    record.timestamp = std::time(nullptr);
    return true;
}

// === 4. Báo cáo & Cảnh báo ===

std::vector<Course> SAS::listCoursesOfLecturer(const std::string& lecturerId) const {
    std::vector<Course> out;
    for (const auto& kv : courses) {
        if (kv.second.lecturerId == lecturerId) out.push_back(kv.second);
    }
    return out;
}

std::vector<Course> SAS::listCoursesOfStudent(const std::string& studentId) const {
    std::vector<Course> out;
    for (const auto& kv : courses) {
        auto it = std::find(kv.second.studentIds.begin(), kv.second.studentIds.end(), studentId);
        if (it != kv.second.studentIds.end()) {
            out.push_back(kv.second);
        }
    }
    return out;
}

double SAS::getStudentAttendancePercentage(const std::string& studentId, const std::string& courseId) const {
    auto sList = listSessionsByCourse(courseId);
    int totalSessions = 0;
    int presentCount = 0;

    for (const auto& s : sList) {
        // Chỉ tính các buổi đã kết thúc
        if (s.endTime < std::time(nullptr) || !s.open) {
            totalSessions++;
            if (s.records.count(studentId)) {
                AttendanceStatus status = s.records.at(studentId).status;
                if (status == AttendanceStatus::PRESENT || status == AttendanceStatus::LATE || status == AttendanceStatus::EXCUSED) {
                    presentCount++;
                }
            }
        }
    }

    if (totalSessions == 0) return 100.0;
    return (double)presentCount / totalSessions * 100.0;
}

// [Cập nhật] getLowAttendanceWarnings (Req 2.6 - Cảnh báo điểm danh)
std::vector<std::string> SAS::getLowAttendanceWarnings(const std::string& studentId, double threshold) const {
    std::vector<std::string> warnings;
    std::vector<Course> studentCourses = listCoursesOfStudent(studentId);

    for (const auto& course : studentCourses) {
        double pct = getStudentAttendancePercentage(studentId, course.courseId);
        if (pct < threshold) {
            warnings.push_back(course.courseName + " (" + std::to_string((int)pct) + "%)");
        }
    }
    return warnings;
}

// ... (Các hàm export report khác)