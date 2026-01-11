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

struct Entry {
    size_t doc_id;
    size_t count;

    bool operator==(const Entry& other) const {
        return (doc_id == other.doc_id && count == other.count);
    }
};

class InvertedIndex {
public:
    InvertedIndex() = default;

    void UpdateDocumentBase(std::vector<std::string> input_docs);
    std::vector<Entry> GetWordCount(const std::string& word);

    // Возвращает частоту слова (число документов, где оно встречается)
    size_t GetWordFrequency(const std::string& word) const;

    // Возвращает список вхождений слова (doc_id + count)
    std::vector<Entry> GetWordEntries(const std::string& word) const;

private:
    std::vector<std::string> docs;
    std::map<std::string, std::vector<Entry>> freq_dictionary;
    std::vector<std::string> SplitIntoWords(const std::string& text);
};
