#include "pieceTreeBuilder.h"
#include <iostream>
#include <string>

using namespace buffer;

int main() {
  // 创建一个文本构建器
  std::unique_ptr<PieceTreeTextBufferBuilder> builder =
      std::make_unique<PieceTreeTextBufferBuilder>();

  // 添加文本块
  builder->acceptChunk("abc\n");
  builder->acceptChunk("def");
  builder->acceptChunk("+KML");
  builder->acceptChunk("\n123");

  // 创建一个工厂
  auto factory = builder->finish();

  // 创建文本缓冲区
  auto buffer = factory->create(DefaultEndOfLine::LF);

  // 获取文本内容
  std::cout << "text count " << buffer->getLineCount() << " line\t"
            << buffer->getLength() << " characters\n"
            << std::endl;
  std::cout << "text content: " << buffer->getLinesRawContent() << std::endl;
  std::cout << "line1 content1s: " << buffer->getLineContent(1) << std::endl;
  // 获取第2行
  std::cout << "line2 content2s: " << buffer->getLineContent(2) << std::endl;
  std::cout << "line3 content3s: " << buffer->getLineContent(3) << std::endl;
  std::cout << "--------check all lines---------" << std::endl;
  auto all_Lines = buffer->getLinesContent();
  for (auto const &str : all_Lines) {
    std::cout << "line:->" << str << std::endl;
  }
  std::cout << "......insert function......" << std::endl;
  std::string content{"124"};
  buffer->insert(0, content);
  std::cout << "text count\t" << buffer->getLineCount() << "line\t"
            << buffer->getLength() << " characters" << std::endl;
  for (int i = 1; i <= buffer->getLineCount(); i++) {
    std::cout << "line" << i << " content: " << buffer->getLineContent(i)
              << std::endl;
  }

  std::cout << "......insert middle function......" << std::endl;
  content = "keb";
  buffer->insert(2, content);
  std::cout << "text count\t" << buffer->getLineCount() << "line\t"
            << buffer->getLength() << " characters" << std::endl;
  for (int i = 1; i <= buffer->getLineCount(); i++) {
    std::cout << "line" << i << " content: " << buffer->getLineContent(i)
              << std::endl;
  }
  std::cout << "......insert end function......" << std::endl;
  // 插入到最后一行
  content = "keb";
  buffer->insert(buffer->getLength(), content);
  std::cout << "text count\t" << buffer->getLineCount() << "line\t"
            << buffer->getLength() << " characters" << std::endl;
  for (int i = 1; i <= buffer->getLineCount(); i++) {
    std::cout << "line" << i << " content: " << buffer->getLineContent(i)
              << std::endl;
  }
  // 插入换行
  std::cout << "......insert multi line function......" << std::endl;
  content = "\nmul lines\n";
  buffer->insert(4, content);
  std::cout << "text count\t" << buffer->getLineCount() << "line\t"
            << buffer->getLength() << " characters" << std::endl;
  for (int i = 1; i <= buffer->getLineCount(); i++) {
    std::cout << "line" << i << " content: " << buffer->getLineContent(i)
              << std::endl;
  }

  std::cout << "buffers->contents:\n"
            << buffer->getLinesRawContent() << std::endl;

  std::cout << "......delete begin function......" << std::endl;
  buffer->delete_(0, 2);
  std::cout << "text count\t" << buffer->getLineCount() << "line\t"
            << buffer->getLength() << " characters" << std::endl;
  // std::cout << "buffer->contents:" << buffer->getLinesRawContent() <<
  // std::endl;
  for (int i = 1; i <= buffer->getLineCount(); i++) {
    std::cout << "line" << i << "content:\t " << buffer->getLineContent(i)
              << std::endl;
  }
  std::cout << "......delete end function......" << std::endl;
  buffer->delete_(buffer->getLength() - 2, 2);
  std::cout << "text count\t" << buffer->getLineCount() << "line\t"
            << buffer->getLength() << " characters" << std::endl;
  for (int i = 1; i <= buffer->getLineCount(); i++) {
    std::cout << "line" << i << " content:\t " << buffer->getLineContent(i)
              << std::endl;
  }
  std::cout << "......delete middle function......" << std::endl;
  buffer->delete_(5, 2);
  std::cout << "text count\t" << buffer->getLineCount() << "line\t"
            << buffer->getLength() << " characters" << std::endl;
  for (int i = 1; i <= buffer->getLineCount(); i++) {
    std::cout << "line" << i << " content:\t " << buffer->getLineContent(i)
              << std::endl;
  }
  std::cout << "......delete multil function......" << std::endl;
  buffer->delete_(8, 6);
  // std::cout << "all contents:" << buffer->getLinesRawContent() << std::endl;
  std::cout << "text count\t" << buffer->getLineCount() << "line\t"
            << buffer->getLength() << " characters" << std::endl;
  for (int i = 1; i <= buffer->getLineCount(); i++) {
    std::cout << "line" << i << " content:\t " << buffer->getLineContent(i)
              << std::endl;
  }
  // std::cout << "all contents:" << buffer->getLinesRawContent() << std::endl;

  std::cout << "......delete edge function......" << std::endl;
  buffer->delete_(4, 34);
  std::cout << "text count\t" << buffer->getLineCount() << "line\t"
            << buffer->getLength() << " characters" << std::endl;
  for (int i = 1; i <= buffer->getLineCount(); i++) {
    std::cout << "line" << i << " content:\t " << buffer->getLineContent(i)
              << std::endl;
  }

  return 0;
}