// sas.h
#pragma once
#include "models.h" 
#include <unordered_map>
#include <optional>
#include <string> 
#include <vector>

class SAS {
public:
    SAS();

    // === 1. IMPORT & DATA ===
    bool loadData(const std::string& prefix = "data");
    bool saveData(const std::string& prefix = "data") const;
    bool importUsers(const std::string& filename);
    bool importCourses(const std::string& filename);

    // === 2. User & Course ===
    bool createUser(const User& user);
    bool removeUser(const std::string& userId);
    std::optional<User> login(const std::string& email, const std::string& password) const;
    std::optional<User> getUser(const std::string& userId) const;
    std::vector<User> listUsers() const;

    bool createCourse(const Course& course);
    bool addStudentToCourse(const std::string& courseId, const std::string& studentId);
    std::optional<Course> getCourse(const std::string& courseId) const;
    std::vector<Course> listCourses() const;

    // === 3. Session & Attendance ===
    std::string createSession(const std::string& courseId, int durationMinutes, bool useTotp);
    bool closeSession(const std::string& sessionId);

    // Điểm danh (Req 2.3 & Req 1 - Anti-fraud)
    bool studentCheckInWithOTP(const std::string& sessionId, const std::string& studentId, const std::string& otp);
    bool studentCheckInWithTOTP(const std::string& sessionId, const std::string& studentId, const std::string& totp_str);

    // [Cập nhật] Chỉnh sửa điểm danh (Req 2.4)
    bool updateAttendanceStatus(const std::string& sessionId, const std::string& studentId, AttendanceStatus newStatus);

    std::optional<AttendanceSession> getSession(const std::string& sessionId) const;
    std::vector<AttendanceSession> listSessions() const;
    std::vector<AttendanceSession> listSessionsByCourse(const std::string& courseId) const;

    // === 4. Báo cáo (Report) ===
    bool exportSessionReportCSV(const std::string& sessionId, const std::string& filename) const;
    bool exportCourseSummaryCSV(const std::string& courseId, const std::string& filename) const;

    double getStudentAttendancePercentage(const std::string& studentId, const std::string& courseId) const;

    // [Cập nhật] Lấy danh sách cảnh báo (Req 2.6)
    std::vector<std::string> getLowAttendanceWarnings(const std::string& studentId, double threshold = 70.0) const;

    // === 5. List Helper ===
    std::vector<Course> listCoursesOfLecturer(const std::string& lecturerId) const;
    std::vector<Course> listCoursesOfStudent(const std::string& studentId) const;
    std::vector<AttendanceRecord> listRecordsOfStudent(const std::string& studentId) const;

    // Helper chuỗi
    static std::vector<std::string> split(const std::string& s, char delimiter);

private:
    std::unordered_map<std::string, User> users;
    std::unordered_map<std::string, Course> courses;
    std::unordered_map<std::string, AttendanceSession> sessions;

    int nextUserSeq = 1;
    int nextCourseSeq = 1;
    int nextSessionSeq = 1;
};