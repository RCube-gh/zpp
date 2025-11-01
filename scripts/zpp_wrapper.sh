#!/usr/bin/env bash
# =======================================
# zpp_wrapper.sh — zsh-native (chpwd hook)
# =======================================

ZPP_BIN="$(dirname "$(realpath "$0")")/../zpp"
DB_PATH="$(dirname "$ZPP_BIN")/data/zpp.db"

# ---- chpwd hook (called after every cd) ----
_zpp_auto_add() {
    "$ZPP_BIN" --add "$(pwd)" >/dev/null 2>&1
}

# register 
if [[ -z "$chpwd_functions" ]] || [[ "${chpwd_functions[(r)_zpp_auto_add]}" != "_zpp_auto_add" ]]; then
    chpwd_functions+=(_zpp_auto_add)
fi

# ---- main function ----
zpp_main() {
    case "$1" in
        --list)
            [[ -f "$DB_PATH" ]] && cat "$DB_PATH" || echo "No database found."
            ;;
        --top)
            [[ -f "$DB_PATH" ]] && sort -t'|' -k2 -nr "$DB_PATH" | head -n 10 || echo "No database found."
            ;;
        --help|-h|"")
            echo "Usage:"
            echo "  z <keyword>   Jump to matching directory"
            echo "  z --list      Show all entries"
            echo "  z --top       Show top 10 entries"
            echo "  (auto record when cd)"
            ;;
        *)
			abs_path="$(realpath -m "$1" 2>/dev/null)"
			if [[ -n "$abs_path" && -d "$abs_path" ]]; then
				builtin cd "$abs_path"
				return
			fi
            local dest
            dest="$("$ZPP_BIN" "$@")"
            if [[ -n "$dest" && -d "$dest" ]]; then
                builtin cd "$dest" || return
            else
                echo "No match found."
            fi
            ;;
    esac
}

# ---- completion function for z ----
_zpp_main_completion() {
    local -a completions
    local query="${words[2]}"
    
    # Handle special commands
    if [[ "$query" == --* ]]; then
        completions=(--list --top --help)
        compadd -V options -a completions
        return
    fi
    
    # Get directory completions from zpp
    if [[ -n "$query" ]]; then
        completions=("${(@f)$("$ZPP_BIN" --complete "$query" 2>/dev/null)}")
    else
        completions=("${(@f)$("$ZPP_BIN" --complete "" 2>/dev/null)}")
    fi
    
    # Offer completions to zsh
    if [[ ${#completions[@]} -gt 0 ]]; then
        compadd -U -V directories -a completions
    fi
}

# Register completion for z and zpp_main
compdef _zpp_main_completion zpp_main
compdef _zpp_main_completion z

