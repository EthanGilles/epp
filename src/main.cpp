#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <optional>
#include <vector>
#include "include/generation.hpp"

static std::string get_file_content(std::string filename)
{
  std::string content;
  {
    std::stringstream content_stream;
    std::fstream input(filename, std::ios::in);
    content_stream << input.rdbuf();
    content = content_stream.str();
  }
  return content;
}

/* ----------------------------------------------------- */

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Incorrect Usage. Correct usage is.. " << std::endl;
    std::cerr << "epp <input.epp>" << std::endl;
    return EXIT_FAILURE;
  }

  std::string content = get_file_content(argv[1]);

  Tokenizer tokenizer(std::move(content));
  std::vector<Token> tokens = tokenizer.tokenize();

  Parser parser(std::move(tokens));
  std::optional<NodeExit> tree = parser.parse();

  if (!tree.has_value())
  {
    std::cerr << "No exit statement found" << std::endl;
    exit(EXIT_FAILURE);
  }

  Generator generator(tree.value());
  {
    std::fstream file("out.asm", std::ios::out);
    file << generator.generate();
  }

  system("nasm -f elf64 out.asm");
  system("ld -o out out.o");

  return EXIT_SUCCESS;
}
