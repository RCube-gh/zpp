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

