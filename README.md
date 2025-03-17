# Minishell

Minishell is a simple shell implementation inspired by bash, created as part of the 42 school curriculum. This project focuses on process management, pipes, file descriptors, and command interpretation, allowing users to execute commands with the same behavior as a standard shell.

## Features

### Core Functionality
- Custom command prompt
- Command history navigation
- Command execution using PATH variable or relative/absolute paths
- Signal handling

### Command Parsing
- The command line gets parsed and tokenized into an abstract syntax tree
- Proper handling of single and double quotes
- Environment variable expansion
- Exit status variable expansion (`$?`)

### Redirections
- Output redirection (`>`)
- Append output redirection (`>>`)
- Input redirection (`<`)
- Heredoc (`<<`): Read input until a specified delimiter

### Pipes
- Command pipeline support (`|`): Connect output of one command to input of another

### Built-in Commands
- `echo`   with `-n` option
- `cd`     without options
- `pwd`    without options
- `export` without options
- `unset`  without options
- `exit`   without options
- `env`    without options or arguments

### Bonus Features
- Logical operators `&&` and `||`
- Support for both interactive and non-interactive shell
- Unclosed quotes handling: Allows continuation of input over multiple lines until quotes are closed (like bash)

## Dependencies

This project requires the [GNU Readline library](https://tiswww.case.edu/php/chet/readline/rltop.html) to handle command line input and history.

### Ubuntu/Debian
```bash
sudo apt-get install libreadline-dev
```

### macOS with Homebrew
```bash
brew install readline
```

### Arch Linux
```bash
sudo pacman -S readline
```

## Installation

```bash
git clone https://github.com/yourusername/minishell.git
cd minishell
make
```

## Usage

### Interactive Mode
```bash
./minishell
```

### Non-Interactive Mode
```bash
echo "ls -la" | ./minishell
./minishell < commands.txt
```
