#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <optional>
#include <vector>
#include <filesystem>
#include "include/tokenization.hpp"
#include "include/parser.hpp"
#include "include/generation.hpp"

#define DEFAULT_MAX_PR 0.76
#define DEFAULT_MIN_PR 0.35

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

int main(int argc, char *argv[]) 
{
  if (argc < 2) 
  {
    std::cerr << "Incorrect Usage. Correct usage is.. " << std::endl;
    std::cerr << "please <input.pls>" << std::endl;
    return (EXIT_FAILURE);
  }

  std::filesystem::path filePath = argv[1];
  if (filePath.extension() != ".pls")
  {
    std::cerr << "Incorrect Usage. Please use on a .pls file.. " << std::endl;
    std::cerr << "please <input.pls>" << std::endl;
    return (EXIT_FAILURE);
  }

  if (!std::filesystem::exists(filePath)) 
  {
    std::cerr << "File `" << filePath.string() << "` does not exist.\n";
    exit (EXIT_FAILURE);
  }

  std::string outputFilename = "out";
  for (int i = 1; i < argc; ++i) {
      if (std::string(argv[i]) == "-o" && i + 1 < argc) {
          outputFilename = argv[i + 1];
      }
  }

  std::string content = get_file_content(argv[1]);

  Tokenizer tokenizer(std::move(content));
  std::vector<Token> tokens = tokenizer.tokenize();

  Parser parser(std::move(tokens));
  std::optional<NodeProgram> program = parser.parse_program();

  if (!program.has_value())
  {
    std::cerr << "Invalid program" << std::endl;
    exit (EXIT_FAILURE);
  }

  {
    Generator generator(program.value(), DEFAULT_MAX_PR, DEFAULT_MIN_PR);
    std::fstream file( (outputFilename + ".asm").c_str(), std::ios::out);
    file << generator.gen_program();
  }

  std::string command = "nasm -f elf64 " + outputFilename + ".asm";
  system (command.c_str());
  command = "ld -o " + outputFilename + " " + outputFilename + ".o";
  system (command.c_str());
  command = outputFilename + ".o";
  remove (command.c_str());

  return (EXIT_SUCCESS);
}
