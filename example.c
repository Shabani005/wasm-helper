#define WASM_STDLIB
#include "./wasm_memory.h"

int add(){
  int *x = wasm_alloc(11);
  for (size_t i=0; i<11; ++i) x[i] = i*i;
  return x[10];
}
