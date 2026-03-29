set -xe

clang \
  -O3 \
  -w \
  --target=wasm32 -nostdlib -fno-builtin  \
  -Wl,--no-entry \
  -Wl,--export-all \
  -Wl,--max-memory=$((256 * 1024 * 1024))\
	example.c -o example.wasm
