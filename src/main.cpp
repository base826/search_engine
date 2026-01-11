#include <iostream>
#include "converter.h"
#include "server.h"
#include "inverted_index.h"
#include <fstream>
#include <filesystem>

#ifndef RUNNING_TESTS

int main() {
    try {

        std::cout << "Current path: " << std::filesystem::current_path() << std::endl;

        // 1. Инициализируем конвертер для работы с JSON
        ConverterJSON converter;

        // 2. Получаем документы из config.json
        std::vector<std::string> docs = converter.GetTextDocuments();
        if (docs.empty()) {
            std::cerr << "Error: documents not found into config.json!\n";
            return 1;
        }

        // 3. Строим инвертированный индекс
        InvertedIndex index;
        index.UpdateDocumentBase(docs);

        // 4. Создаём поисковый сервер
        SearchServer server(index);
        server.responseLimit = converter.GetResponsesLimit();

        // 5. Получаем поисковые запросы из requests.json
        std::vector<std::string> requests = converter.GetRequests();
        if (requests.empty()) {
            std::cout << "Warning: there are no requests in requests.json. Completing the work.\n";
            return 0;
        }

        // 6. Выполняем поиск по всем запросам
        std::vector<std::vector<RelativeIndex>> results = server.search(requests);

        // 7. Преобразуем RelativeIndex в формат для JSON:
        //    { { {doc_id, rank}, ... }, ... }
        std::vector<std::vector<std::pair<int, float>>> answers_for_json;
        for (const auto& result_list : results) {
            std::vector<std::pair<int, float>> current_answers;
            for (const RelativeIndex& ri : result_list) {
                current_answers.emplace_back(static_cast<int>(ri.doc_id), ri.rank);
            }
            answers_for_json.push_back(current_answers);
        }

        // 8. Записываем результаты в answers.json
        converter.putAnswers(answers_for_json);

        std::cout << "The search is completed. The results are saved in answers.json.\n";

        // 9. Дополнительно: выводим в консоль (для отладки)
        for (size_t i = 0; i < requests.size(); ++i) {
            std::cout << "Request: \"" << requests[i] << "\"\n";
            if (results[i].empty()) {
                std::cout << "  No results\n";
            }
            else {
                for (const RelativeIndex& ri : results[i]) {
                    std::cout << "  doc_id=" << ri.doc_id
                        << ", rank=" << ri.rank << "\n";
                }
            }
        }

    }
    catch (const std::exception& e) {
        std::cerr << "Execution error: " << e.what() << "\n";
        return 1;
    }

}
#endif // RUNNING_TESTS