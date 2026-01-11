#include "server.h"

// –еализаци€ методов SearchServer
std::vector<std::string> SearchServer::SplitQuery(const std::string& query) {
    std::vector<std::string> words;
    std::string word;
    for (char c : query) {
        if (std::isspace(c)) {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        }
        else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }
    return words;
}

float SearchServer::CalculateAbsoluteRelevance(
    const std::map<size_t, size_t>& doc_counts, size_t doc_id) {
    auto it = doc_counts.find(doc_id);
    return it != doc_counts.end() ? static_cast<float>(it->second) : 0.0f;
}

std::vector<std::vector<RelativeIndex>> SearchServer::search(
    const std::vector<std::string>& queries_input) {

    std::vector<std::vector<RelativeIndex>> result;

    for (const std::string& query : queries_input) {
        std::vector<std::string> words = SplitQuery(query);
        std::set<std::string> unique_words(words.begin(), words.end());
        std::vector<std::string> sorted_words(unique_words.begin(), unique_words.end());

        // —ортируем по частоте (от редких к частым)
        std::sort(sorted_words.begin(), sorted_words.end(),
            [this](const std::string& a, const std::string& b) {
                size_t freq_a = _index.GetWordFrequency(a);
                size_t freq_b = _index.GetWordFrequency(b);
                return freq_a < freq_b;
            });

        std::unordered_map<size_t, size_t> doc_counts;

        for (const std::string& word : sorted_words) {
            std::vector<Entry> entries = _index.GetWordEntries(word);
            for (const Entry& entry : entries) {
                doc_counts[entry.doc_id] += entry.count;
            }
        }

        if (doc_counts.empty()) {
            result.push_back({});
            continue;
        }

        std::vector<RelativeIndex> rel_indices;
        float max_relevance = 0.0f;

        for (const auto& [doc_id, total_count] : doc_counts) {
            float abs_relevance = static_cast<float>(total_count);
            rel_indices.push_back({ doc_id, abs_relevance });
            if (abs_relevance > max_relevance) {
                max_relevance = abs_relevance;
            }
        }

        for (RelativeIndex& ri : rel_indices) {
            ri.rank /= max_relevance;
        }

        std::sort(rel_indices.begin(), rel_indices.end(),
            [](const RelativeIndex& a, const RelativeIndex& b) {
                if (a.rank > b.rank) {
                    return true;  // a идЄт раньше, если rank больше
                }
                if (a.rank < b.rank) {
                    return false;  // a идЄт позже, если rank меньше
                }
                // rank равны Ч сортируем по doc_id по возрастанию
                return a.doc_id < b.doc_id;
            });

        if (rel_indices.size() > responseLimit) {
            rel_indices.resize(responseLimit);
        }

        result.push_back(rel_indices);
    }

    return result;
}
