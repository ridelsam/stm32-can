#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
project_root="$(cd -- "$script_dir/.." && pwd)"
config="${CONFIG:-Debug}"

if [[ "$#" != 0 ]]; then
    printf 'Usage: CONFIG=Debug|Release %s\n' "$0" >&2
    exit 1
fi

case "$config" in
    Debug|Release) ;;
    *) printf 'Unsupported CONFIG: %s; use Debug or Release\n' "$config" >&2; exit 1 ;;
esac

elf="$project_root/build/$config/stm32-can.elf"

command -v openocd >/dev/null 2>&1 || {
    printf 'Flash error: OpenOCD is not available on PATH\n' >&2
    exit 1
}

[[ -f "$elf" ]] || {
    printf 'Flash error: %s does not exist; build with ./scripts/build.sh CONFIG=%s first\n' \
        "$elf" "$config" >&2
    exit 1
}

printf 'Programming %s\n' "$elf"
exec openocd \
    -f "$project_root/openocd.cfg" \
    -c "program {$elf} verify reset exit"
