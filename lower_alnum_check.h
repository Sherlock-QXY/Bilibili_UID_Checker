#ifndef LOWER_ALNUM_CHECK_H
#define LOWER_ALNUM_CHECK_H

#include <string>
#include <cctype>

// 判断字符是否为 小写字母 或 数字
inline bool isLowerOrDigit(char c) {
    return (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9');
}

// 严格版：只允许 小写字母 + 数字，且不能为空
inline bool isAllLowerAlnum(const std::string& s) {
    if (s.empty()) return false;
    for (char c : s) {
        if (!isLowerOrDigit(static_cast<unsigned char>(c))) return false;
    }
    return true;
}

// 宽松版：空串视为通过（适合「可选的」字段校验）
inline bool isAllLowerAlnumAllowEmpty(const std::string& s) {
    for (char c : s) {
        if (!isLowerOrDigit(static_cast<unsigned char>(c))) return false;
    }
    return true;
}

// 只要「不含大写字母」且「不含其它非法字符」其实和上面等价；
// 如果你想要的是「小写字母 + 数字，且至少含一个字母」可用这个：
inline bool isAllLowerAlnumWithAlpha(const std::string& s) {
    if (s.empty()) return false;
    bool hasAlpha = false;
    for (char c : s) {
        if (c >= 'a' && c <= 'z') {
            hasAlpha = true;
        } else if (c >= '0' && c <= '9') {
            // ok
        } else {
            return false;
        }
    }
    return hasAlpha;
}

#endif