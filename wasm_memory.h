#ifdef WASM_STDLIB
typedef unsigned long size_t;
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;

extern uint8_t __heap_base;
static uint8_t *heap_pos = &__heap_base;
static uint8_t *heap_end = 0;

unsigned long __builtin_wasm_memory_size(int index);
unsigned long __builtin_wasm_memory_grow(int index, unsigned long delta);

static inline void wasm_heap_init(void) {
  uint32_t pages = __builtin_wasm_memory_size(0);
  heap_end = (uint8_t *)(pages * 65536);
}

void *wasm_alloc(size_t size) {
  if (!heap_end) {
    wasm_heap_init();
  }

  size = (size + 7) & ~7;

  if (heap_pos + size > heap_end) {
    uint32_t needed = (uint32_t)((heap_pos + size - heap_end + 65535) / 65536);
    if (__builtin_wasm_memory_grow(0, needed) == (uint32_t)-1)
      return 0;
    heap_end += needed * 65536;
  }
  void *p = heap_pos;
  heap_pos += size;
  return p;
}

#define W_ALLOC wasm_alloc

#else
#include <stdint.h>
#include <stdlib.h>
#define W_ALLOC malloc
#include <stdio.h>
#endif
