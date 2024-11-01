# E++ and the Please Compiler

### COS360: Final Project

**Author:** Ethan H. Gilles  
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

The **Please Compiler** is a compiler for a new programming language, E++, which emphasizes politeness. This project was developed for the COS360: Programming Languages class, 
where the task was an open ended project, somehow related to programming languages. In E++, politeness is mandatory, and the compiler enforces it by requiring the programmer 
to include polite language throughout the code—most importantly, the word "please".

The idea behind the project is to encourage a unique syntax style that introduces humor and creativity into programming while exploring the process of language design and compiler construction.

## Features

- **Syntax Enforcement**: The compiler will only compile code if it detects the right amount of polite language, including "please" or "PLEASE".
- **Basic Language Constructs**: Supports fundamental operations, including variable declarations, basic arithmetic, conditional statements and print statements.
- **Error Handling**: Provides 'user-friendly' error messages when polite language is missing or when syntax rules are broken.
- **Tokenization and Parsing**: Uses custom-built tokenization and parsing to identify keywords, operators, and polite phrases.
- **Code Generation**: Outputs runnable x86 assembly instructions which it will then assemble using the NASM assembler and link object files with the GNU linker to create an executable.
- **Grammar Documentation**: LaTeX/pdf documentation for the grammar of the E++ language, providing production rules and syntax-related information.

## Getting Started


### Prerequisites

- A working C/C++ compiler.
- cmake, make

### Installation

1. Clone the repository:
    ```bash
    git clone https://github.com/EthanGilles/epp.git
    ```
2. Enter the repository directory:
    ```bash 
    cd epp
    ```
3. Install the compiler:
    ```bash 
    make install
    ```
4. Test your installation:
    ```bash
    make install-test
    ```

You should get the following output: 
``` 
Hello World!
``` 
Along with two new files in the current directory: `hello.asm` and `hello`.

### Usage

To compile an E++ source code file ending in .epp, use:
```bash 
please file.epp
```


## Example Code 

The following code is an example of a valid 'Hello World!' E++ program:
```
/* Hello world! */
please set H = 72;
please set e to 101;
set l = 108;
set o = 100 + 10 + 2 - 1;
please set space to 8 * 4;
set W to 87;
set r = (50 * 2) + (7 * 2);
set d to 100;
set excl = 3 * 11;

please print(H, e, l, l, o, space, W, o, r, l, d, excl);

goodbye(0);
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
    \text{set} \; \text{ID} = [\text{Expr}]; \\
    \text{reset} \; \text{ID} = [text{Expr}]; \\
    \text{if} ([\text{Expr}])[\text{Scope}][\text{AfterIf}]  \\
    [\text{Scope}] \\
  \end{cases} \\
\end{align*}
$$


## Project Structure

- **src/**: Contains the source code for the Please Compiler.
- **examples/**: Sample polite code files for testing.
- **docs/**: Documentation and specifications for the language.
- **build/**: Contains the build files from cmake and the compiler executable.

## Contributing

This project is primarily a class project and is not open for contributions at this time. 
However, feel free to explore the code and create issues if you find any bugs or have suggestions for improvement!

## License

This project is licensed under the MIT License.
