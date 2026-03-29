#define WASM_STDLIB

typedef unsigned long long size_t;

__attribute__((import_module("env"), import_name("write_char_js"))) extern int
write_char_js(char c);
#define WRITE_CHAR write_char_js
#include "./wasm_memory.h"

#define P_MALLOC W_ALLOC

int printf(const char *fmt, ...);
static int get_strlen(char *s);
void *simp_memcpy(void *dst, const void *src, unsigned long n);
int simp_strcmp(const char *s1, const char *s2);
char *simp_strdup(char *s);
int simp_memcmp(const void *s1, const void *s2, size_t n);

#define CHAOS_REALLOC W_REALLOC
#define CHAOS_FREE W_FREE
#define CHAOS_MEMCMP simp_memcmp
#define CHAOS_PRINTF printf
#define CHAOS_STRLEN get_strlen
#define CHAOS_MEMCPY simp_memcpy
#define CHAOS_STRCMP simp_strcmp
#define CHAOS_STRDUP simp_strdup
#define CHAOS_VSNPRINTF(...) 0

#define NULL ((void *)0)
#define UINT32_T uint32_t
#define UINT8_T uint8_t

#define CHAOS_FPRINTF(...) 0
#define CHAOS_CALLOC(n, sz) W_REALLOC(NULL, (n) * (sz))
#define CHAOS_ASSERT(cond)
#define CHAOS_TODO(msg)

#include "./print.h"
#define CHAOS_IMPLEMENTATION
#include "./chaos.h"

int main() {
  int *x = W_REALLOC(NULL, 11 * sizeof(int));
  for (size_t i = 0; i < 11; ++i)
    x[i] = i * i;

  String_Builder sb = {0};

  sb_append_cstr(&sb, "Hello");
  sb_append_cstr(&sb, "Chaos");
  sb_append_cstr(&sb, "From");
  sb_append_cstr(&sb, "Wasm");
  sb_append_null(&sb);

  printf("items ptr: %p\n", (void *)sb.items);
  print(sb);


  printf("String Builder count: %d\n", sb.count);
  printf("array element: x[3] = %d\n", x[3]);

  W_FREE(sb.items);

  return x[10];
}
