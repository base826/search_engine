cmake_minimum_required(VERSION 3.14)
#set(CMAKE_POLICY_VERSION_MINIMUM 3.11)
cmake_policy(SET CMP0135 NEW)
# get JSON from github
include(FetchContent)
#FetchContent_Declare(json
#        GIT_REPOSITORY https://github.com/nlohmann/json
#        GIT_TAG v3.10.5
#        )
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

configure_file(${CMAKE_SOURCE_DIR}/config/config.json ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_SOURCE_DIR}/config/requests.json ${CMAKE_CURRENT_BINARY_DIR})


add_executable(search_server main.cpp converter.cpp inverted_index.cpp server.cpp)

target_include_directories(search_server PUBLIC  ${INCLUDE_DIR})

target_link_libraries(search_server PRIVATE nlohmann_json::nlohmann_json)

target_include_directories(search_server PUBLIC ${INCLUDE_DIR} nlohmann_json)