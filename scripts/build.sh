#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
project_root="$(cd -- "$script_dir/.." && pwd)"

source "$project_root/toolchain.conf"

if [[ -z "${TOOLCHAIN_ROOT:-}" &&
      -f "$project_root/toolchain.local.conf" ]]; then
    source "$project_root/toolchain.local.conf"
fi

toolchain_root="${TOOLCHAIN_ROOT:-$HOME/tools/$TOOLCHAIN_DIRECTORY}"

if [[ "$toolchain_root" != /* ]]; then
    toolchain_root="$project_root/$toolchain_root"
fi

tool_prefix="$toolchain_root/bin/$TOOLCHAIN_TRIPLE-"

if [[ ! -x "${tool_prefix}gcc" ]]; then
    printf 'Compiler not found: %s\n' "${tool_prefix}gcc" >&2
    exit 1
fi

gcc_version="$("${tool_prefix}gcc" -dumpfullversion -dumpversion)"

if [[ "$gcc_version" != "$TOOLCHAIN_GCC_VERSION" ]]; then
    printf 'Expected GCC %s, found %s\n' \
        "$TOOLCHAIN_GCC_VERSION" "$gcc_version" >&2
    exit 1
fi

if [[ "${1:-}" == "--check" ]]; then
    "${tool_prefix}gcc" --version
    exit 0
fi

exec make -C "$project_root" CROSS_COMPILE="$tool_prefix" "$@"
