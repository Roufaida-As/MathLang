# MathLang Copilot Instructions

## Project Overview
MathLang is a French-based domain-specific language for mathematical programming, implemented using Flex (lexer) and Bison (parser) in C. It supports variables, functions, control structures, set operations, and complex numbers with mathematical notation.

## Architecture
- **Lexer** (`mathlang.l`): Tokenizes input using patterns; outputs tokens defined in `tokens.h`
- **Parser** (`mathlang.y`): Defines grammar rules and operator precedence; currently validates syntax only
- **Tokens** (`tokens.h`): Central token definitions with YYSTYPE union for semantic values
- Data flow: Source code → Lexer → Tokens → Parser → Syntax validation

## Key Workflows
- **Build parser**: `make` (generates `lex.yy.c`, `mathlang.tab.c`, compiles to `parser` executable)
- **Test parsing**: `make test-parser` (requires `test_parser.math` file)
- **Test lexer**: `make lexer && make test-lexer` (requires `test_lexer.math`)
- **Interactive testing**: `make interactive` (run parser manually)

## Language Conventions
- **Keywords**: French (e.g., `SOIT x : Z` for variable, `SI ... ALORS ... FIN` for if)
- **Types**: Mathematical notation (`Z` integers, `R` reals, `B` booleans, `C` complexes, `Sigma` strings)
- **Operators**: Symbolic math (`<-` assignment, `∪` union, `∈` in, `^` power, `div` integer division)
- **Literals**: Standard with complex support (e.g., `2+3i`)
- **Comments**: `#` for single-line comments

## Development Patterns
- **Adding tokens**: Define in `tokens.h` (e.g., `#define TOK_NEW 999`), add pattern in `mathlang.l`, declare in `mathlang.y` `%token`
- **Grammar rules**: Use Bison syntax in `mathlang.y`; update precedences for operators
- **Testing**: Create `.math` files in `tests/` directory; test files use `.ml` extension but contain MathLang code
- **Error handling**: Lexer reports lexical errors with line/column; parser uses `yyerror()`

## Key Files
- `mathlang.l`: Lexical analysis rules
- `mathlang.y`: Grammar and parsing logic
- `tokens.h`: Token constants and YYSTYPE
- `Makefile`: Build automation
- `tests/*.ml`: Test cases (MathLang syntax, not OCaml)</content>
<parameter name="filePath">c:\Users\PC HP\Desktop\MathLang\.github\copilot-instructions.md