// storage.h
#pragma once
#include "models.h" // Cần thiết để định nghĩa User, Course, etc.
#include <string>
#include <vector>
#include <optional> // Yêu cầu C++17

// storage: save/load entire state to JSON file
class Storage {
public:
    // Constructor
    Storage(const std::string& filename);

    // Methods
    bool load();    // load state from JSON file (returns true if ok)
    bool save() const; // save state to JSON file

    // Public members to hold the application state (read/write access)
    std::vector<User> users;
    std::vector<Course> courses;
    std::vector<AttendanceSession> sessions;

private:
    std::string filename_;
}; // ⬅️ DẤU CHẤM PHẨY KẾT THÚC CLASS NÀY LÀ CỰC KỲ QUAN TRỌNG VÀ CẦN PHẢI CÓ!