# z++ ~ Directory Jumper ~

A smart directory navigation tool for zsh that learns from your habits and provides intelligent tab completion.

## Features

- **Frecency-based ranking**: Combines frequency and recency to prioritize your most used directories
- **Fuzzy matching**: Jump to directories with partial keyword matches
- **Tab completion**: Intelligent tab completion for directory candidates
- **Automatic tracking**: Records directory visits automatically via zsh hooks
- **Local directory priority**: Subdirectories of current location are checked first
- **Lightweight**: Written in C++17 for fast performance

## How It Works

z++ uses a "frecency" algorithm (frequency + recency) to rank directories:
- Directories you visit frequently get higher scores
- Recently visited directories are weighted more heavily
- The score decays over time if a directory isn't visited

The database is automatically updated every time you change directories.

## Installation

### Requirements

- zsh
- C++17 compatible compiler (g++)
- Linux/Unix system

### Build

```bash
git clone <repository-url>
cd zpp
make
```

### Shell Integration

Add to your `~/.zshrc`:

```bash
export PATH="/path/to/zpp:$PATH"
eval "$(zpp --init)"
```

Then restart your shell:

```bash
exec zsh
```

## Usage

### Basic Commands

**Jump to a directory:**
```bash
z++ projects      # Jump to best matching directory containing "projects"
z++ doc           # Jump to directory matching "doc"
```

**Tab completion:**
```bash
z++ pro<Tab>      # Press Tab to see all matching candidates
```

The completion system shows:
1. First: Subdirectories of your current location
2. Then: Directories from your frecency database, sorted by score

### Alternative: Using `z` command

For additional features, source the wrapper script:

```bash
# Add to .zshrc
source /path/to/zpp/scripts/zpp_wrapper.sh
alias z=zpp_main
```

Available commands:
```bash
z <keyword>       # Jump to matching directory
z --list          # Show all recorded directories with stats
z --top           # Show top 10 most used directories
z --help          # Display help message
z pro<Tab>        # Tab completion works here too
```

## Examples

```bash
# After using the tool for a while...
cd ~/projects/myapp
cd ~/documents
cd ~/projects/myapp
cd ~/projects/myapp

# Now you can jump quickly:
z++ app           # Takes you to ~/projects/myapp
z++ doc           # Takes you to ~/documents

# Tab completion shows ranked results:
z++ pro<Tab>      # Shows all "pro" matches, sorted by frecency
```

## Database

The directory history is stored in `data/zpp.db` relative to the binary location.

Format: `path|rank|last_access_timestamp`

Example:
```
/home/user/projects|23|1761714020
/home/user/documents|15|1761713841
```

## Architecture

- **C++ Core**: Fast frecency calculation and directory matching
- **Database**: Simple pipe-delimited file format for portability
- **Shell Integration**: zsh hooks for automatic tracking
- **Completion System**: Native zsh completion using `compadd`

### Components

- `database.cpp/hpp` - Directory database management
- `frecent.cpp/hpp` - Frecency score calculation algorithm
- `search.cpp/hpp` - Directory matching and completion logic
- `cli.cpp/hpp` - Command-line interface and initialization
- `utils.cpp/hpp` - Utility functions
- `zpp_wrapper.sh` - Enhanced shell wrapper with additional commands

## Configuration

### Custom Database Location

By default, the database is stored in `<zpp-directory>/data/zpp.db`. You can modify this in the source code if needed.

### Frecency Algorithm

The scoring formula can be adjusted in `frecent.cpp`:

```cpp
double frecent(double rank, std::time_t last_access) {
    std::time_t now = std::time(nullptr);
    double dx = static_cast<double>(now - last_access);
    return 10000.0 * rank * (3.75 / ((0.0001 * dx + 1.0) + 0.25));
}
```

## Troubleshooting

See [INSTALL_COMPLETION.md](INSTALL_COMPLETION.md) for detailed troubleshooting steps.

## Credits

Inspired by [z](https://github.com/rupa/z) and [autojump](https://github.com/wting/autojump).
