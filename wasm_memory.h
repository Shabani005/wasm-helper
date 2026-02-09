#ifndef WASM_STDLIB
typedef unsigned long size_t;
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;

#define HEAP_SIZE (65536 * 2 * 2)

static unsigned char heap[64 * 1024 * 1024];
static size_t heap_pos = 0;

void *wasm_alloc(size_t size) {
  if (heap_pos + size > HEAP_SIZE)
    return 0;
  void *p = &heap[heap_pos];
  heap_pos += size;
  return p;
}

#else
#include <stdint.h>
#include <stdlib.h>
#endif

