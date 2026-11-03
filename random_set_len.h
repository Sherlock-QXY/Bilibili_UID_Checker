
#ifndef RANDOM_SET_LEN_H
#define RANDOM_SET_LEN_H

#include <random>
#include <string>

inline int getRandomInt(int min, int max) {
    static std::mt19937 engine(std::random_device{}());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(engine);
}

inline std::string getRandomString(int length) {
    if (length <= 0) return "";

    std::string result;
    result.reserve(length);

    for (int i = 0; i < length; ++i) {
        result += static_cast<char>(getRandomInt(0, 9) + '0');
    }

    return result;
}

#endif
