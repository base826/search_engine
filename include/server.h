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
#include "inverted_index.h"

struct RelativeIndex {
    size_t doc_id;
    float rank;

    bool operator==(const RelativeIndex& other) const {
        return (doc_id == other.doc_id && rank == other.rank);
    }
};

class SearchServer {
public:
    explicit SearchServer(InvertedIndex& idx) : _index(idx) {}
    int responseLimit = 5;
    std::vector<std::vector<RelativeIndex>> search(
        const std::vector<std::string>& queries_input);

private:
    InvertedIndex& _index;
    std::vector<std::string> SplitQuery(const std::string& query);
    float CalculateAbsoluteRelevance(
        const std::map<size_t, size_t>& doc_counts, size_t doc_id);
};