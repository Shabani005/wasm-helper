set -xe

clang \
  -O3 \
  --target=wasm32 -nostdlib \
  -Wl,--no-entry \
  -Wl,--export-all \
  -Wl,--initial-memory=131072\
  -Wl,--max-memory=$((256 * 1024 * 1024))\
	example.c -o example.wasm
