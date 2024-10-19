#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <optional>
#include <vector>

enum class TokenType { 
  RETURN, 
  INT_LIT, 
  SEMI,
};

struct Token {
  TokenType type;
  std::optional<std::string> value;
};

std::vector<Token> tokenize (const std::string& str) {
  std::vector<Token> tokens;

  std::string buf;
  for (int i = 0; i < str.length (); i++) 
  {
    char c = str.at (i);

    /* IF WE FIND LETTERS */
    if (std::isalpha (c)) 
    {
      buf.push_back (c);
      i++;
      while (std::isalpha (str.at (i))) 
      {
        buf.push_back (str.at (i));
        i++;
      }
      i--;
      if (buf == "return") 
      {
        tokens.push_back ({.type = TokenType::RETURN});
        buf.clear();
        continue;
      }
      else 
      {
        std::cerr << "You messed up!" << std::endl;
        exit(EXIT_FAILURE);
      }
    }

    /* IF WE FIND NUMBERS */
    else if(std::isdigit(c))
    {
      buf.push_back(c);
      i++;
      while(std::isdigit(str.at(i)))
      {
        buf.push_back(str.at(i));
        i++;
      }
      i--;
      tokens.push_back({.type = TokenType::INT_LIT, .value = buf});
      buf.clear();
    }

    /* IF WE FIND A SEMICOLON */
    else if (c == ';')
      tokens.push_back({.type = TokenType::SEMI});

    /* SKIP WHITE SPACE */
    else if (std::isspace(c))
      continue;
    else
    {
        std::cerr << "You messed up!" << std::endl;
        exit(EXIT_FAILURE);
    }
  }

  return tokens;
}

std::string tokens_to_asm (const std::vector<Token>& tokens) 
{
  std::stringstream output;
  output << "global _start\n_start:\n";
  for (int i = 0; i < tokens.size(); i++)
  {
    const Token& token = tokens[i];
    if (token.type == TokenType::RETURN)
    {
      if (i + 1 < tokens.size() && tokens.at(i + 1).type == TokenType::INT_LIT)
      {
        if (i + 2 < tokens.size() && tokens.at(i + 2).type == TokenType::SEMI) {
          output << "    mov rax, 60\n";
          output << "    mov rdi, " << tokens.at(i + 1).value.value() << "\n";
          output << "    syscall";
        }
      }
    }
  }
  return output.str();
}

/* ----------------------------------------------------- */
int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Incorrect Usage. Correct usage is.. " << std::endl;
    std::cerr << "epp <input.epp>" << std::endl;
    return EXIT_FAILURE;
  }

  std::string content;
  {
    std::stringstream content_stream;
    std::fstream input(argv[1], std::ios::in);
    content_stream << input.rdbuf();
    content = content_stream.str();
  }

  std::vector<Token> tokens = tokenize(content);
  std::cout << tokens_to_asm(tokens) << std::endl;

  {
    std::fstream file("out.asm", std::ios::out);
    file << tokens_to_asm(tokens);
  }

  system("nasm -f elf64 out.asm");
  system("ld -o out out.o");

  return EXIT_SUCCESS;
}
