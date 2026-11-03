#ifndef BILI_API_H
#define BILI_API_H

#include <iostream>
#include <string>
#include <curl/curl.h>
#include <cstdlib>

struct BiliUserInfo {
    long long mid = 0;
    std::string name = "";
    std::string sex = "";
    std::string sign = "";
    int level = -1;
    int follower = -1;
    int http_code = 0;
    bool is_success = false;
    std::string error_msg = "";
};

inline size_t WriteCallback(void* contents, size_t size,
                            size_t nmemb, void* userp) {
    size_t total = size * nmemb;
    ((std::string*)userp)->append((char*)contents, total);
    return total;
}

// 简单提取 JSON 字符串字段
inline std::string getJsonString(const std::string& json,
                                 const std::string& key) {
    std::string target = "\"" + key + "\"";
    size_t pos = json.find(target);
    if (pos == std::string::npos) return "";

    pos = json.find(':', pos);
    if (pos == std::string::npos) return "";

    pos = json.find('"', pos);
    if (pos == std::string::npos) return "";

    size_t end = json.find('"', pos + 1);
    if (end == std::string::npos) return "";

    return json.substr(pos + 1, end - pos - 1);
}

inline long long getJsonNumber(const std::string& json,
                               const std::string& key,
                               long long defaultValue = -1) {
    std::string target = "\"" + key + "\"";
    size_t pos = json.find(target);
    if (pos == std::string::npos) return defaultValue;

    pos = json.find(':', pos);
    if (pos == std::string::npos) return defaultValue;

    ++pos;
    while (pos < json.size() && json[pos] == ' ') ++pos;

    char* end = NULL;
    long long value = std::strtoll(json.c_str() + pos, &end, 10);

    if (end == json.c_str() + pos) return defaultValue;
    return value;
}

inline BiliUserInfo getBilibiliUserInfo(const std::string& uid) {
    BiliUserInfo info;
    std::string response;

    CURL* curl = curl_easy_init();
    if (!curl) {
        info.error_msg = "curl 初始化失败";
        return info;
    }

    std::string url =
        "https://bili.shoyu.top/api/bili4.php?type=card&uid=" + uid;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res = curl_easy_perform(curl);

    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    info.http_code = (int)httpCode;

    if (res != CURLE_OK) {
        info.error_msg = curl_easy_strerror(res);
        curl_easy_cleanup(curl);
        return info;
    }

    if (httpCode == 429) {
        info.error_msg = "HTTP 429：请求过于频繁";
        curl_easy_cleanup(curl);
        return info;
    }

    if (httpCode != 200) {
        info.error_msg = "HTTP 状态码异常";
        curl_easy_cleanup(curl);
        return info;
    }

    std::string mid_str = getJsonString(response, "mid");
    if (!mid_str.empty()) {
        info.mid = std::stoll(mid_str);
    } else {
        info.mid = getJsonNumber(response, "mid", 0);
    }
    info.name = getJsonString(response, "name");
    info.sex = getJsonString(response, "sex");
    info.sign = getJsonString(response, "sign");
    info.level = (int)getJsonNumber(response, "current_level", -1);
    info.follower = (int)getJsonNumber(response, "follower", -1);

    if (info.mid > 0 && !info.name.empty()
        && info.level >= 0 && info.follower >= 0) {
        info.is_success = true;
    } else {
        info.error_msg = "API 返回数据不完整或格式不符合预期";
    }

    curl_easy_cleanup(curl);
    return info;
}

#endif
