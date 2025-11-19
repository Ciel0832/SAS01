// models.h
#pragma once
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <optional>

enum class Role { ADMIN, LECTURER, STUDENT };
enum class AttendanceStatus { UNKNOWN, PRESENT, ABSENT, LATE, EXCUSED };

struct User {
    std::string id;
    std::string fullName;
    std::string email;
    std::string password;
    Role role;

    User() = default;
    User(const std::string& i, const std::string& n, const std::string& e,
        const std::string& p, Role r)
        : id(i), fullName(n), email(e), password(p), role(r) {
    }
};

struct AttendanceRecord {
    std::string studentId;
    AttendanceStatus status = AttendanceStatus::UNKNOWN;
    std::time_t timestamp = 0;
};

struct Course {
    std::string courseId;
    std::string courseName;
    std::string lecturerId;
    std::vector<std::string> studentIds;
};

struct AttendanceSession {
    std::string sessionId;
    std::string courseId;
    std::time_t startTime = 0;
    std::time_t endTime = 0;
    bool open = false;
    std::string otp; // Static OTP
    std::string totpSecretBase32; // For QR/Google Auth
    std::string qrFilename; // Filename of the generated QR code
    std::map<std::string, AttendanceRecord> records;
};