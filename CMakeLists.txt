cmake_minimum_required(VERSION 3.20)  # 3.20+ recommended for good C++23 support
project(dezsh LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Collect source files automatically
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")

# Create the executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Include the header directory
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

# Link with the readline library
find_library(READLINE_LIB readline)
if (READLINE_LIB)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${READLINE_LIB})
else()
    message(FATAL_ERROR "readline library not found. Please install it (e.g., 'sudo apt install libreadline-dev').")
endif()

# Optional: enable warnings and nice defaults
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive-)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE)# -Wall -Wextra -Wpedantic)
endif()
