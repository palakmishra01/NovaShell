# NovaShell

A lightweight Linux shell built in C++ to explore process management, system calls, file descriptors, I/O redirection, and inter-process communication.

## Features

- Interactive command-line interface
- Execution of external Linux commands
- Built-in commands:
  - `cd`
  - `exit`
- Process creation using `fork()`
- Program execution using `execvp()`
- Parent-child synchronization using `waitpid()`
- Input redirection using `<`
- Output redirection using `>`
- Command pipelines using `|`
- Inter-process communication using `pipe()`
- File descriptor manipulation using `dup2()`

## How It Works

NovaShell reads a command from the user, parses it into arguments, and determines whether it is a built-in command, an external command, or involves I/O redirection or a pipeline.

```bash
g++ -Wall -Wextra main.cpp -o novashell
and run:
./novashell
