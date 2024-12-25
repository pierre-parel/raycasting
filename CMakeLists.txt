# Configure CMake
cmake_minimum_required(VERSION 3.20)

## Establish Project ##

project(DOOMA)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})


## Build App ##
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
PRIVATE
    src/main.cpp
)

## Dependencies ##
### EXCLUDE_FROM_ALL prevents targets and configs from polluting upwards
# add_subdirectory(SDL EXCLUDE_FROM_ALL)
find_package(SDL3)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
target_compile_definitions(${PROJECT_NAME} PUBLIC SDL_MAIN_USE_CALLBACKS)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
