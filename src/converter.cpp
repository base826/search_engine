#include "converter.h"

// Реализация методов ConverterJSON
ConverterJSON::ConverterJSON() {
    try {
        std::cout << "Reading config from: " << configPath << std::endl;  // Выведите путь
        config = readJsonFile(configPath);

        // Отладочная печать
        std::cout << "Config loaded:" << std::endl;
        std::cout << config.dump(2) << std::endl;  // Красиво выводит JSON

        validateConfig();
        std::cout << "Starting " << config["config"]["name"].get<std::string>() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::vector<std::string> ConverterJSON::GetTextDocuments() {
    std::vector<std::string> documents;

    if (!config.contains("files")) {
        return documents;
    }

    for (const auto& filePath : config["files"]) {
        const std::string path = filePath.get<std::string>();
        if (!fileExists(path)) {
            std::cerr << "Warning: File not found - " << path << std::endl;
            continue;
        }

        std::ifstream file(path);
        std::string content((std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());
        documents.push_back(content);
    }

    return documents;
}

int ConverterJSON::GetResponsesLimit() {
    if (config["config"].contains("max_responses")) {
        return config["config"]["max_responses"].get<int>();
    }
    return 5;
}

std::vector<std::string> ConverterJSON::GetRequests() {
    nlohmann::json requestsJson = readJsonFile(requestPath);
    std::vector<std::string> requests;

    // Проверяем, что есть ключ "requests" и он является массивом
    if (!requestsJson.contains("requests")) {
        throw std::runtime_error("JSON does not contain 'requests' key");
    }

    if (!requestsJson["requests"].is_array()) {
        throw std::runtime_error("'requests' must be an array");
    }

    // Итерируем по элементам массива requestsJson["requests"]
    for (const auto& request : requestsJson["requests"]) {
        // Проверяем, что элемент — строка
        if (request.is_string()) {
            requests.push_back(request.get<std::string>());
        }
        else {
            std::cerr << "Warning: Skipping non-string value in 'requests': "
                << request.dump() << std::endl;
        }
    }

    return requests;
}

void ConverterJSON::putAnswers(const std::vector<std::vector<std::pair<int, float>>>& answers) {
    nlohmann::json answersJson;
    answersJson["answers"] = nlohmann::json::object();

    for (size_t i = 0; i < answers.size(); ++i) {
        std::string requestKey = "request" + std::to_string(1000 + i + 1).substr(1);  // request001, request002...

        nlohmann::json requestObj;

        if (answers[i].empty()) {
            requestObj["result"] = "false";
        }
        else {
            requestObj["result"] = "true";

            // Сортируем по убыванию ранга (второе значение пары)
            std::vector<std::pair<int, float>> sortedItems = answers[i];
            std::sort(sortedItems.begin(), sortedItems.end(),
                [](const std::pair<int, float>& a, const std::pair<int, float>& b) {
                    return a.second > b.second;  // Убывание
                });

            requestObj["relevance"] = nlohmann::json::object();
            for (const auto& item : sortedItems) {
                std::string docKey = "docid:" + std::to_string(item.first);
                //requestObj["relevance"][docKey] = item.second;
                requestObj["relevance"][docKey] = std::round(item.second * 1000) / 1000.0;
            }
        }

        answersJson["answers"][requestKey] = requestObj;
    }

    // Запись в файл
    std::ofstream file(answerPath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open answer file: " + answerPath);
    }
    file << std::setw(4) << answersJson << std::endl;
    std::cout << "Answers saved to " << answerPath << std::endl;
}

void ConverterJSON::validateConfig() {
    if (config.empty()) {
        throw std::runtime_error("config file is empty");
    }

    if (!config.contains("config")) {
        throw std::runtime_error("config file is missing 'config' section");
    }

    const std::string appVersion = "0.1";
    if (!config["config"].contains("version")) {
        throw std::runtime_error("config.json is missing 'version' field");
    }

    if (config["config"]["version"] != appVersion) {
        throw std::runtime_error("config.json has incorrect file version. Expected: " + appVersion);
    }
}

bool ConverterJSON::fileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

nlohmann::json ConverterJSON::readJsonFile(const std::string& path) {
    if (!fileExists(path)) {
        throw std::runtime_error("File not found: " + path);
    }

    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + path);
    }

    nlohmann::json jsonData;
    file >> jsonData;
    return jsonData;
}
