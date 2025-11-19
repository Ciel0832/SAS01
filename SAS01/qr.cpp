// qr.cpp
#include "qr.h"
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>

bool generate_qr_png(const std::string& text, const std::string& out_png) {
    // Thử gọi lệnh ngoài `qrencode` (cần cài đặt)
    // qrencode -o out.png "text"
    std::ostringstream cmd;
    cmd << "qrencode -o \\\"" << out_png << "\\\" \\\"" << text << "\\\"";
    // Sử dụng std::system để gọi lệnh (Chỉ dùng được nếu qrencode đã cài)
    int rc = std::system(cmd.str().c_str());
    return rc == 0;
}

bool generate_qr_ascii(const std::string& text, std::string& ascii_out) {
    // Fallback: chỉ hiển thị nội dung text
    std::ostringstream ss;
    ss << "----- QR (text) -----\n";
    ss << text << "\n";
    ss << "---------------------\n";
    ascii_out = ss.str();
    return true;
}