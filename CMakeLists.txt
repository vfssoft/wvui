cmake_minimum_required(VERSION 3.16)
project(wvui VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(WEBVIEW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(WEBVIEW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(WEBVIEW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(WEBVIEW_BUILD_SHARED_LIBRARY OFF CACHE BOOL "" FORCE)
set(WEBVIEW_BUILD_STATIC_LIBRARY OFF CACHE BOOL "" FORCE)
add_subdirectory(external/webview)

set(HTTPLIB_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(HTTPLIB_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(external/cpp-httplib)

set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
set(JSON_Install OFF CACHE BOOL "" FORCE)
add_subdirectory(external/json)

# Generate API code from YAML
set(GENERATED_API_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/generated)
file(MAKE_DIRECTORY ${GENERATED_API_DIR})

add_custom_command(
    OUTPUT ${GENERATED_API_DIR}/api_interface.h ${GENERATED_API_DIR}/api_server.h
    COMMAND ${CMAKE_COMMAND} -E env
        PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR}/external/yaml-cpp
        python ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_api.py
        ${CMAKE_CURRENT_SOURCE_DIR}/api_definition.yaml
        ${GENERATED_API_DIR}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/api_definition.yaml
            ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_api.py
    COMMENT "Generating API code from api_definition.yaml"
)

add_custom_target(generate_api ALL
    DEPENDS ${GENERATED_API_DIR}/api_interface.h ${GENERATED_API_DIR}/api_server.h
)

add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/rest_server.cpp
)
add_dependencies(${PROJECT_NAME} generate_api)

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    webview::core
    httplib::httplib
    nlohmann_json::nlohmann_json
)

