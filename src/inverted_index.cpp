#include "inverted_index.h"

// Реализация методов InvertedIndex
void InvertedIndex::UpdateDocumentBase(std::vector<std::string> input_docs) {
    docs = std::move(input_docs);
    freq_dictionary.clear();

    size_t numDocs = docs.size();
    if (numDocs == 0) return;

    // Количество потоков (можно настроить)
    size_t numThreads = std::thread::hardware_concurrency();
    numThreads = std::min(numThreads, numDocs);

    std::vector<std::thread> threads;
    std::mutex mtx;

    // Функция для индексации подмножества документов
    auto processDocs = [this, &mtx](size_t start, size_t end) {
        for (size_t doc_id = start; doc_id < end; ++doc_id) {
            const std::string& doc_text = docs[doc_id];
            std::vector<std::string> words = SplitIntoWords(doc_text);


            std::map<std::string, size_t> word_count;
            for (const std::string& word : words) {
                word_count[word]++;
            }

            // Добавляем в общий индекс
            for (const auto& [word, count] : word_count) {
                Entry entry{ doc_id, count };
                std::lock_guard<std::mutex> lock(mtx);
                freq_dictionary[word].push_back(entry);
            }
        }
        };

    // Разбиваем документы на блоки для потоков
    size_t blockSize = numDocs / numThreads;
    size_t remainder = numDocs % numThreads;

    size_t start = 0;
    for (size_t i = 0; i < numThreads; ++i) {
        size_t end = start + blockSize + (i < remainder ? 1 : 0);
        threads.emplace_back(processDocs, start, end);
        start = end;
    }

    // Ждём завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }

    for (auto& [word, entries] : freq_dictionary) {
        std::sort(entries.begin(), entries.end(),
            [](const Entry& a, const Entry& b) {
                return a.doc_id < b.doc_id;  // сортировка по doc_id по возрастанию
            });
    }

}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string& word) {
    auto it = freq_dictionary.find(word);
    if (it != freq_dictionary.end()) {
        return it->second;
    }
    return {};
}

std::vector<std::string> InvertedIndex::SplitIntoWords(const std::string& text) {
    std::vector<std::string> words;
    std::string word;

    for (char ch : text) {
        if (std::isalnum(ch)) {
            word += std::tolower(ch);
        }
        else {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        }
    }

    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

size_t InvertedIndex::GetWordFrequency(const std::string& word) const {
    auto it = freq_dictionary.find(word);
    return it != freq_dictionary.end() ? it->second.size() : 0;
}

std::vector<Entry> InvertedIndex::GetWordEntries(const std::string& word) const {
    auto it = freq_dictionary.find(word);
    if (it != freq_dictionary.end()) {
        return it->second;
    }
    return {};
}