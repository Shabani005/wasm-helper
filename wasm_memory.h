#pragma once

#ifdef WASM_STDLIB
typedef unsigned long long size_t;
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

// TODO: implement actual realloc
void *wasm_realloc(void* ptr, size_t n){
  return wasm_alloc(n);
}

void *wasm_memcpy(void *dst, const void *src, unsigned long n) {
  unsigned char *d = dst;
  const unsigned char *s = src;
  while (n--) {
    *d++ = *s++;
  }
  return dst;
}

int wasm_memcmp(const void *s1, const void *s2, size_t n) {
  const unsigned char *p1 = s1, *p2 = s2;
  while (n--) {
    if (*p1 != *p2) return (int)*p1 - (int)*p2;
    p1++; p2++;
  }
  return 0;
}

void wasm_free(void *p){
  (void)p;
}



#define W_ALLOC   wasm_alloc
#define W_REALLOC wasm_realloc
#define W_MEMCMP  wasm_memcmp
#define W_FREE    wasm_free

#else
  #include <stdint.h>
  #include <stdlib.h>
  #define W_ALLOC   malloc
  #define W_REALLOC realloc
  #define W_MEMCMP  memcmp
  #include <stdio.h>
#endif
