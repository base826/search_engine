#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <filesystem>
#include <map>
#include <cmath>
#include <algorithm>
#include <cctype>
#include <unordered_map>
#include <set>
#include <thread>
#include <mutex>
#include <future>
#include <string>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "server.h"

/**
*  Класс для работы с JSON-файлами
*/
class ConverterJSON {
public:
    ConverterJSON();
    std::vector<std::string> GetTextDocuments();
    int GetResponsesLimit();
    std::vector<std::string> GetRequests();
    void putAnswers(const std::vector<std::vector<std::pair<int, float>>>& answers);

private:
    nlohmann::json config;
    const std::string configPath = "C:/Users/base8/Documents/Skillbox/search_engine/config/config.json";
    const std::string requestPath = "C:/Users/base8/Documents/Skillbox/search_engine/config/requests.json";
    const std::string answerPath = "C:/Users/base8/Documents/Skillbox/search_engine/config/answers.json";
    bool fileExists(const std::string& path);
    nlohmann::json readJsonFile(const std::string& path);
    void validateConfig();
};
