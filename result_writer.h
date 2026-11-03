#ifndef RESULT_WRITER_H
#define RESULT_WRITER_H

#include <string>
#include <fstream>
#include <mutex>

// 全局写入锁，保证多线程追加时不交错
inline std::mutex& getResultWriterMutex() {
    static std::mutex mtx;
    return mtx;
}

// 向文件末尾追加一行文本（自动换行）
// 返回 true 表示写入成功
inline bool appendLineToFile(const std::string& line,
                             const std::string& filename = "result.txt") {
    std::lock_guard<std::mutex> lock(getResultWriterMutex());

    // std::ios::app 保证每次写到文件末尾，文件不存在则自动创建
    std::ofstream fout(filename, std::ios::app);
    if (!fout.is_open()) {
        return false;
    }

    fout << line << '\n';
    fout.flush();  // 立即落盘，防止程序异常退出丢数据
    return fout.good();
}

// 便捷封装：写入 "UID: xxx | 用户名: xxx"
inline bool appendBiliUserToResult(const std::string& uid,
                                   const std::string& name,
                                   const std::string& filename = "result.txt") {
    std::string line = "UID: " + uid + " | 用户名: " + name;
    return appendLineToFile(line, filename);
}

#endif