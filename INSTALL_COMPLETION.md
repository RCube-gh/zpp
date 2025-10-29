# zpp Tab Completion Installation

## Installation Steps

1. **Build the binary**
   ```bash
   cd /path/to/zpp
   make
   ```

2. **Add to PATH and enable shell integration**
   Add the following to your `.zshrc`:
   ```bash
   export PATH="/path/to/zpp:$PATH"
   eval "$(zpp --init)"
   ```

3. **Restart your shell**
   ```bash
   exec zsh
   ```

## Usage

```bash
# Try tab completion
z++ test<Tab>    # Shows matching directory candidates
z++ proj<Tab>    # Shows directories containing "proj"
```

## Alternative: Using the `z` command

If you prefer the shorter `z` command with additional features:

```bash
# Add to .zshrc
source /path/to/zpp/scripts/zpp_wrapper.sh
alias z=zpp_main
```

The `z` command provides:
- `z <keyword>` - Jump to matching directory
- `z --list` - Show all recorded directories
- `z --top` - Show top 10 frequently used directories
- Tab completion for both directories and options

## Troubleshooting

### Completion not working

1. **Check PATH**
   ```bash
   which zpp
   # Should show /path/to/zpp/zpp
   ```

2. **Check completion function**
   ```bash
   which _zpp_completion
   # Should show function definition
   ```

3. **Test completion candidates manually**
   ```bash
   zpp --complete test
   # Should display list of directories
   ```

4. **Reinitialize completion system**
   ```bash
   autoload -Uz compinit && compinit
   eval "$(zpp --init)"
   ```

### Debug mode

To verify completion is being triggered:
```bash
# Add to .zshrc
zstyle ':completion:*' verbose yes
zstyle ':completion:*:descriptions' format '%B%d%b'
```
