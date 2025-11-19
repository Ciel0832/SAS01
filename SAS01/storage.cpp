// storagecpp.cpp
#include "storage.h"
#include <fstream>
#include <iostream>
// BẠN CẦN THƯ VIỆN NLOHMANN/JSON.HPP TẠI ĐÂY
#include "third_party/json.hpp" 
using json = nlohmann::json;

// --- JSON SERIALIZATION HELPERS ---

// Helper để chuyển AttendanceStatus sang string
std::string statusToString(AttendanceStatus s) {
    if (s == AttendanceStatus::PRESENT) return "PRESENT";
    if (s == AttendanceStatus::ABSENT) return "ABSENT";
    if (s == AttendanceStatus::LATE) return "LATE";
    if (s == AttendanceStatus::EXCUSED) return "EXCUSED";
    return "UNKNOWN";
}

// Helper để chuyển string sang AttendanceStatus
AttendanceStatus stringToStatus(const std::string& s) {
    if (s == "PRESENT") return AttendanceStatus::PRESENT;
    if (s == "ABSENT") return AttendanceStatus::ABSENT;
    if (s == "LATE") return AttendanceStatus::LATE;
    if (s == "EXCUSED") return AttendanceStatus::EXCUSED;
    return AttendanceStatus::UNKNOWN;
}
// --- END HELPERS ---

Storage::Storage(const std::string& filename) : filename_(filename) {}

bool Storage::load() {
    std::ifstream ifs(filename_);
    if (!ifs) return false;
    json j;
    try {
        ifs >> j;
        users.clear();
        courses.clear();
        sessions.clear();

        // Load Users
        for (auto& ju : j["users"]) {
            User u;
            u.id = ju.value("id", "");
            u.fullName = ju.value("fullName", "");
            u.email = ju.value("email", "");
            u.password = ju.value("password", "");
            int r = ju.value("role", 2);
            u.role = (Role)r;
            users.push_back(u);
        }

        // Load Courses
        for (auto& jc : j["courses"]) {
            Course c;
            c.courseId = jc.value("courseId", "");
            c.courseName = jc.value("courseName", "");
            c.lecturerId = jc.value("lecturerId", "");
            // Load studentIds (mảng string)
            if (jc.contains("studentIds") && jc["studentIds"].is_array()) {
                for (auto& s : jc["studentIds"]) c.studentIds.push_back(s.get<std::string>());
            }
            courses.push_back(c);
        }

        // Load Sessions
        for (auto& js : j["sessions"]) {
            AttendanceSession s;
            s.sessionId = js.value("sessionId", "");
            s.courseId = js.value("courseId", "");
            s.startTime = (std::time_t)js.value("startTime", 0LL);
            s.endTime = (std::time_t)js.value("endTime", 0LL);
            s.open = js.value("open", false);
            s.otp = js.value("otp", "");
            s.totpSecretBase32 = js.value("totpSecretBase32", "");
            s.qrFilename = js.value("qrFilename", "");

            // Load Records (map)
            if (js.contains("records") && js["records"].is_object()) {
                for (auto& jrec : js["records"].items()) {
                    AttendanceRecord rec;
                    rec.studentId = jrec.key(); // key is studentId
                    rec.status = stringToStatus(jrec.value().value("status", "UNKNOWN"));
                    rec.timestamp = (std::time_t)jrec.value().value("timestamp", 0LL);
                    s.records[rec.studentId] = rec;
                }
            }
            sessions.push_back(s);
        }

    }
    catch (const std::exception& e) {
        std::cerr << "JSON Load Error: " << e.what() << "\n";
        return false;
    }
    return true;
}

bool Storage::save() const {
    json j;
    j["users"] = json::array();
    j["courses"] = json::array();
    j["sessions"] = json::array();

    // Save Users
    for (auto& u : users) {
        json ju;
        ju["id"] = u.id;
        ju["fullName"] = u.fullName;
        ju["email"] = u.email;
        ju["password"] = u.password;
        ju["role"] = (int)u.role;
        j["users"].push_back(ju);
    }

    // Save Courses
    for (auto& c : courses) {
        json jc;
        jc["courseId"] = c.courseId;
        jc["courseName"] = c.courseName;
        jc["lecturerId"] = c.lecturerId;
        jc["studentIds"] = c.studentIds;
        j["courses"].push_back(jc);
    }

    // Save Sessions
    for (auto& s : sessions) {
        json js;
        js["sessionId"] = s.sessionId;
        js["courseId"] = s.courseId;
        js["startTime"] = (long long)s.startTime;
        js["endTime"] = (long long)s.endTime;
        js["open"] = s.open;
        js["otp"] = s.otp;
        js["totpSecretBase32"] = s.totpSecretBase32;
        js["qrFilename"] = s.qrFilename;

        // Save Records
        js["records"] = json::object();
        for (auto& rec : s.records) {
            json jrec;
            jrec["status"] = statusToString(rec.second.status);
            jrec["timestamp"] = (long long)rec.second.timestamp;
            js["records"][rec.first] = jrec; // Key is studentId
        }
        j["sessions"].push_back(js);
    }

    std::ofstream ofs(filename_);
    if (!ofs) return false;
    ofs << std::setw(4) << j << std::endl;
    return true;
}