# vscode TextBuffer C++ Implementation

This is a C++17 port of the VSCode TextBuffer implementation from TypeScript. The TextBuffer is implemented as a piece table data structure for efficient text editing operations.

## Features

- Efficient text buffer management using a piece table data structure
- Red-Black tree implementation for balanced tree operations
- Support for line and column positions
- UTF-16 code points support
- End-of-line normalization

## Building the Project

### Prerequisites

- C++17 compatible compiler
- CMake 3.15 or newer

### Build Steps

```bash
# Create a build directory
mkdir build && cd build

# Generate build files
cmake ..

# Build the project
cmake --build .
```

### Running Tests

```bash
# In the build directory
ctest -V
```

## Usage

```cpp
#include <textbuffer/pieceTreeBuilder.h>
#include <string>

int main() {
    // Create a builder
    textbuffer::PieceTreeTextBufferBuilder builder;
    
    // Add chunks of text
    builder.acceptChunk("Hello, ");
    builder.acceptChunk("world!");
    
    // Create a TextBuffer
    auto factory = builder.finish();
    auto buffer = factory.create(vscode::DefaultEndOfLine::LF);
    
    // Use the buffer
   buffer->getLineConut();
   buffer->getLength();
   buffer->insert(0,"344");
   buffer->delete_(3,5);
    
    return 0;
}
```

## License

Licensed under the MIT License. See the LICENSE file for details. 