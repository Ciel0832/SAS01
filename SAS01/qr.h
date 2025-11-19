// qr.h
#pragma once
#include <string>

// create QR image using external tool `qrencode` if available.
// text -> output PNG path (returns true if success).
bool generate_qr_png(const std::string& text, const std::string& out_png);

// fallback: create ASCII QR (very small) or return false.
bool generate_qr_ascii(const std::string& text, std::string& ascii_out);
