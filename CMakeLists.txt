cmake_minimum_required(VERSION 3.15)
project(vscode_textbuffer VERSION 1.0.0 LANGUAGES CXX)

# C++17 Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Source files
set(SOURCES
    src/common/charCode.cpp
    src/common/position.cpp
    src/common/range.cpp
    src/rbTreeBase.cpp
    src/pieceTreeBase.cpp
    src/pieceTreeBuilder.cpp
)

# Headers
set(HEADERS
    src/common/charCode.h
    src/common/position.h
    src/common/range.h
    src/rbTreeBase.h
    src/pieceTreeBase.h
    src/pieceTreeBuilder.h
)

# Add library target
add_library(vscode_textbuffer STATIC ${SOURCES} ${HEADERS})

# Examples directory
add_subdirectory(examples)

# Install target
install(TARGETS vscode_textbuffer
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(FILES ${HEADERS} DESTINATION include/vscode_textbuffer)
