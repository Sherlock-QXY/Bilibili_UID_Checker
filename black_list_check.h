#ifndef BLACK_LIST_CHECK_H
#define BLACK_LIST_CHECK_H

#include <string>
#include <vector>
#include <fstream>
#include <mutex>

// 全局黑名单缓存（懒加载）
struct BlackListCache {
    bool loaded = false;
    std::vector<std::string> words;
    std::mutex mtx;
};

inline BlackListCache& getBlackListCache() {
    static BlackListCache cache;
    return cache;
}

// 去除首尾空白（含 \r，兼容 Windows 换行）
inline std::string trimBlackListLine(const std::string& s) {
    size_t begin = 0;
    size_t end = s.size();
    while (begin < end && (s[begin] == ' ' || s[begin] == '\t' ||
                           s[begin] == '\r' || s[begin] == '\n')) {
        ++begin;
    }
    while (end > begin && (s[end - 1] == ' ' || s[end - 1] == '\t' ||
                           s[end - 1] == '\r' || s[end - 1] == '\n')) {
        --end;
    }
    return s.substr(begin, end - begin);
}

// 读取黑名单文件（每行一个词，忽略空行与 # 开头的注释行）
inline void loadBlackList(const std::string& filename = "black_list_substr.txt") {
    BlackListCache& cache = getBlackListCache();
    std::lock_guard<std::mutex> lock(cache.mtx);

    cache.words.clear();
    cache.loaded = true;

    std::ifstream fin(filename);
    if (!fin.is_open()) {
        return;  // 文件不存在则视为空黑名单
    }

    std::string line;
    while (std::getline(fin, line)) {
        std::string word = trimBlackListLine(line);
        if (word.empty()) continue;
        if (word[0] == '#') continue;  // 注释行
        cache.words.push_back(word);
    }
}

// 检查 text 中是否包含任意黑名单词
// 返回命中的词；若未命中返回空字符串
inline std::string findBlackListWord(const std::string& text,
                                     const std::string& filename = "black_list_substr.txt") {
    BlackListCache& cache = getBlackListCache();

    // 懒加载
    {
        std::lock_guard<std::mutex> lock(cache.mtx);
        if (!cache.loaded) {
            // 注意：loadBlackList 内部还会再锁一次，这里先解锁再调用
        } else {
            // 已加载，直接查
            for (const auto& w : cache.words) {
                if (text.find(w) != std::string::npos) {
                    return w;
                }
            }
            return "";
        }
    }

    // 首次加载（在锁外调用，避免死锁）
    loadBlackList(filename);

    std::lock_guard<std::mutex> lock(cache.mtx);
    for (const auto& w : cache.words) {
        if (text.find(w) != std::string::npos) {
            return w;
        }
    }
    return "";
}

// 便捷布尔版：是否命中黑名单
inline bool containsBlackListWord(const std::string& text,
                                  const std::string& filename = "black_list_substr.txt") {
    return !findBlackListWord(text, filename).empty();
}

// 手动重载黑名单（例如文件被修改后想刷新）
inline void reloadBlackList(const std::string& filename = "black_list_substr.txt") {
    BlackListCache& cache = getBlackListCache();
    {
        std::lock_guard<std::mutex> lock(cache.mtx);
        cache.loaded = false;
        cache.words.clear();
    }
    loadBlackList(filename);
}

#endif