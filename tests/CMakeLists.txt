cmake_minimum_required(VERSION 3.14)

cmake_policy(SET CMP0135 NEW)

project(server_test)

# Получаем GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Собираем тестовый исполняемый файл
add_executable(server_test
    index_tests.cpp
    server_tests.cpp
    ${CMAKE_SOURCE_DIR}/src/inverted_index.cpp
    ${CMAKE_SOURCE_DIR}/src/server.cpp
)

# Указываем пути к заголовкам:
target_include_directories(server_test PUBLIC
    ${gtest_SOURCE_DIR}/include
    ${INCLUDE_DIR}
    ${json_SOURCE_DIR}/single_include  # ← путь к nlohmann/json.hpp
)

# Линкуем библиотеки
target_link_libraries(server_test PRIVATE
    gtest_main
    nlohmann_json::nlohmann_json  # ← линковка JSON-библиотеки
)

# Регистрация теста для ctest
add_test(
    NAME server_test
    COMMAND $<TARGET_FILE:server_test>
)