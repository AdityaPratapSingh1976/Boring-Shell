# Boring Shell — A Simple UNIX Shell in C

A minimal shell interpreter written in C to explore UNIX primitives like `fork`, `exec`, `pipe`, `dup`, `wait`, and signal handling.

---

## Features

- Interactive prompt with a shell-like banner  
- Command execution using `fork()` and `execvp()`  
- Input / output redirection (`<`, `>`, and `>>`)  
- Piping using `|`  
- Signal handling for `SIGINT` (`Ctrl-C`) and `SIGTSTP` (`Ctrl-Z`)

---
## todo list
- [ ] add job handling with &, fg and bg
- [ ] integrate environ variable handling to work with new parser
- [ ] use lex and yacc for parsing
---
## Installation & Usage

```bash
git clone https://github.com/AdityaPratapSingh1976/Boring-Shell.git
cd Boring-Shell
make
./main
