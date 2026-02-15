cmake_minimum_required(VERSION 3.16)

project(BWorld LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(BWorld)

add_executable(${PROJECT_NAME} entry_point.cpp)

add_subdirectory(BEngine)
add_subdirectory(VkCore)

# target_include_directories(${PROJECT_NAME} PUBLIC BEngine VkCore)

target_link_libraries(${PROJECT_NAME} PRIVATE BEngine VkCore)
