set -xe

clang \
  -O3 \
  --target=wasm32 -nostdlib \
  -Wl,--no-entry \
  -Wl,--export-all \
	example.c -o example.wasm
