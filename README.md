# 🙏The Please Compiler🙏

<img src="https://github.com/EthanGilles/pleaselang/blob/7a304416626c7f6f290c7f19873cd371ba574546/docs/logo.png" alt="Logo" width="200"/>

### COS360: Final Project

**Author:** Ethan Gilles  
**University:** University of Southern Maine  
**Course:** COS360 - Programming Languages  

- [Project Description](#project-description)
- [Features](#features)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
  - [Usage](#usage)
- [Example Code](#example-code)
- [Grammar Documentation](#grammar-documentation)
  - [Grammar Rules](#grammar-rules)
- [Project Structure](#project-structure)
- [Contributing](#contributing)
- [License](#license)

## Project Description

<img src="https://github.com/EthanGilles/Portfolio/blob/bfd5dd556865539b840b7b9e7d84139b3a8b3f55/src/assets/projects/compiler.png" alt="Logo" width="600"/>

The **Please Compiler** is a compiler for a new programming language, **PleaseLang**, which emphasizes politeness. This project was developed for the COS360: Programming Languages class, 
where the task was an open ended project, somehow related to programming languages. In **PleaseLang**, politeness is mandatory, and the compiler enforces it by requiring the programmer 
to include polite language throughout the code—most importantly, the word "please".

The idea behind the project is to encourage a unique syntax style that introduces humor and creativity into programming while exploring the process of language design and compiler construction.

## Features

- **Syntax Enforcement**: The compiler will only compile code if it detects the right amount of polite language, including "please", "PLEASE", or the emoji "🙏".
- **Basic Language Constructs**: Supports fundamental operations, including variable declarations, basic arithmetic, conditional statements, loops, lists and print statements.
- **Error Handling**: Provides 'user-friendly' error messages when polite language is missing or when syntax rules are broken.
- **Tokenization and Parsing**: Uses custom-built tokenization and parsing to identify keywords, operators, and polite phrases.
- **Code Generation**: Outputs runnable x86 assembly instructions which it will then assemble using the NASM assembler and link object files with the GNU linker to create an executable.
- **Grammar Documentation**: LaTeX/pdf documentation for the grammar of the PleaseLang language, providing production rules and syntax-related information.

## Getting Started


### Prerequisites

- A Debian/Ubuntu Linux system.
- We're using *modern* C++. gcc C++ 20 compiler is required.
- cmake, make
- Netwide Assembler
    ```bash
    sudo apt install gcc cmake nasm
    ```
test
### Installation

1. Clone the repository:
    ```bash
    git clone https://github.com/EthanGilles/pleaselang.git
    ```
2. Enter the repository directory:
    ```bash 
    cd pleaselang
    ```
3. Install the compiler:
    ```bash 
    make install
    ```
4. Test your installation:
    ```bash
    make install-test
    ```

Testing the installation will use the compiler binary on a .pls file then run
the executable, which gives the output: 
```
please examples/install.pls -o test
./test
PleaseLang is installed!
``` 
Along with two new files in the current directory: `test.asm` and `test`.

### Usage

To compile a PleaseLang source code file ending in .pls, use:
```bash 
please file.pls
```


## Example Code 

The following code is an example of a valid 'Hello World!' PleaseLang program:
```
/* Hello world! */
please printn("Hello World!");

goodbye(0);
```

More examples can be found in the `examples/` directory.

The language is actually quite expressive at times. Here is an example of 
the alphabet being printed without using a loop:

```
please set range = ['a'..'z'];
please printn("Alphabet: ", range);
```

## Grammar Documentation

The full documentation for the grammar is available in `docs/grammar.pdf`
The documentation is created using LaTeX and has the production rules of the grammar. 
An example of the grammar is shown below, with the production rules for statements.

$$
\begin{align*}
  [\text{Prog}] &\to [\text{Statement}]^* \\
  [\text{Statement}] &\to 
  \begin{cases}
    \text{please} \\
    \text{PLEASE} \\
    \text{exit}([\text{Expr}]); \\ 
    \text{set} \space \text{ID} = [\text{Expr}]; \\
    \text{reset} \space \text{ID} = [text{Expr}]; \\
    \text{if} ([\text{Expr}])[\text{Scope}][\text{AfterIf}]  \\
    [\text{Scope}] \\
  \end{cases} \\
\end{align*}
$$


## Project Structure

- **src/**: Contains the source code for the Please Compiler.
- **examples/**: Sample code files for testing and exploring the language.
- **docs/**: Documentation and specifications for the language.
- **build/**: Contains the build files from cmake and the compiler executable after using `make install`.

## Contributing

This project is primarily a class project and is not open for contributions at this time. 
However, feel free to explore the code and create issues if you find any bugs or have suggestions for improvement!

## License

This project is licensed under the MIT License.
