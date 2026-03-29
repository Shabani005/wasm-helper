/*
  chaos.h - v1.11.12
  The name of this Library is inspired from chaos, an antonym of standard indicating it is an addition to the C standard
  library with some chaos embedded to it. ENJOY

  Good to knows:
  - String_Builders are basically like Java String builders, which means they're not supposed to be null terminated
  - Dynamic Arrays are structures with a pointer to another structure or type. It is supposed to follow this structure:
      items (pointer)
      count
      capacity
  - chaos_temp_sprintf() returns a pointer to a null terminater string on the stack, which means that it WILL be reset when
    called, this is why we introduced 20 temporary buffers -> temporarily allocated string will only get overwritten after 20 calls.
*/

#ifndef CHAOS_H_
#define CHAOS_H_

#ifndef CHAOSDEF
#define CHAOSDEF
#endif

#ifndef CHAOS_REALLOC
#include <stdlib.h>
#define CHAOS_REALLOC realloc
#endif

#ifndef CHAOS_FREE
#include <stdlib.h>
#define CHAOS_FREE free
#endif

#ifndef CHAOS_ASSERT
#include <assert.h>
#define CHAOS_ASSERT assert
#endif

#ifndef CHAOS_FPRINTF
#include <stdio.h>
#define CHAOS_FPRINTF fprintf
#endif

#ifndef CHAOS_TODO
#include <stdio.h>
#define CHAOS_TODO(message) do { CHAOS_FPRINTF(stderr, "%s:%d TODO: %s\n", __FILE__, __LINE__, message); abort(); } while(0)
#endif

#ifndef CHAOS_PRINTF
#include <stdio.h>
#define CHAOS_PRINTF printf
#endif

#ifndef CHAOS_MEMCMP
#include <string.h>
#define CHAOS_MEMCMP memcmp
#endif

#ifndef CHAOS_STRCMP
#include <string.h>
#define CHAOS_STRCMP strcmp
#endif


#ifndef CHAOS_MEMCPY
#include <string.h>
#define CHAOS_MEMCPY memcpy
#endif

#ifndef CHAOS_CALLOC
#include <stdlib.h>
#define CHAOS_CALLOC calloc
#endif

#ifndef CHAOS_VSNPRINTF
#include <stdio.h>
#define CHAOS_VSNPRINTF vsnprintf
#endif

#ifndef CHAOS_STRLEN
#include <stdio.h>
#define CHAOS_STRLEN strlen
#endif


#ifndef CHAOS_STRDUP
#include <string.h>

static char* chaos_strdup(const char *s) {
    size_t len = strlen(s) + 1;
    char *buf = (char*)CHAOS_REALLOC(NULL, len);
    if (!buf) return NULL;
    memcpy(buf, s, len);
    return buf;
}

#define CHAOS_STRDUP chaos_strdup
#endif

#define CHAOS_ARRAY_LEN(array) (sizeof(array)/sizeof(array[0]))


#ifdef __cplusplus
#define CHAOS_DECLTYPE_CAST(T) (decltype(T))
#else
#define CHAOS_DECLTYPE_CAST(T)
#endif // __cplusplus

#define chaos_da_reserve(da, expected_capacity)                                            \
    do {                                                                                   \
        if ((expected_capacity) > (da)->capacity) {                                        \
            if ((da)->capacity == 0) {                                                     \
                (da)->capacity = CHAOS_DA_INIT_CAP;                                        \
            }                                                                              \
            while ((expected_capacity) > (da)->capacity) {                                 \
                (da)->capacity *= 2;                                                       \
            }                                                                              \
            (da)->items = CHAOS_DECLTYPE_CAST((da)->items)CHAOS_REALLOC((da)->items, (da)->capacity * sizeof(*(da)->items)); \
            CHAOS_ASSERT((da)->items != NULL && "Buy more RAM lol");                       \
        }                                                                                  \
    } while (0)

#define chaos_da_append(da, item)                \
    do {                                         \
        chaos_da_reserve((da), (da)->count + 1); \
        (da)->items[(da)->count++] = (item);     \
    } while (0)

#ifdef __unix__
  #include <unistd.h>
#elif defined(_WIN32)
  #include <windows.h>
  #include <io.h>
  #define access _access
  #define F_OK 0
#endif

#ifndef CHAOS_BOOL 
  #include <stdbool.h>
  #define CHAOS_BOOL bool
#else
  typedef enum {false, true} bool;
  #define CHAOS_BOOL bool;
#endif


#ifdef __unix__
#include <sys/stat.h>
#include <time.h>
#include <stdint.h>

#define UINT8_T uint8_t
#define  UINT32_T uint32_t

#else
  #ifndef UINT8_T
    #define UINT8_T usigned char
  #endif

  #ifndef UINT32_T
    #define UINT32_T unsigned int
  #endif
#endif

#include <stdarg.h>

typedef struct {
  char* items;
  size_t count;
  size_t capacity;
} Chaos_String_Builder;

typedef struct {
  const char *data;
  size_t count;
} Chaos_String_View;

typedef struct {
  Chaos_String_View* items;
  size_t count;
  size_t capacity;
} Chaos_cmd_arr;

typedef struct {
  UINT8_T *items;
  size_t count;
  size_t capacity;
} chaos_arena;

typedef struct {
  UINT32_T key;
  char *value;
  size_t freq;
} chaos_KV;

typedef struct {
  chaos_KV *items;
  size_t count;
  size_t capacity;
} chaos_Bucket;

typedef struct {
  chaos_Bucket *items;
  size_t count;
  size_t capacity;
  size_t len;
} chaos_Table;

typedef struct {
  Chaos_String_View long_name;
  Chaos_String_View short_name;
  Chaos_String_View desc;

  CHAOS_BOOL present;
  Chaos_String_View value;
  void (*dispatcher)(void);
} Chaos_Flag;

/*
  ======== CONSTANTS ========
*/

#ifndef CHAOS_TMP_BUF_SIZE
#define CHAOS_TMP_BUF_SIZE 8192
#endif // CHAOS_TMP_BUF_SIZE

#ifndef CHAOS_TMP_BUF_COUNT
#define CHAOS_TMP_BUF_COUNT 20
#endif // CHAOS_TMP_BUF_COUNT

#ifndef CHAOS_DA_INIT_CAP
#define CHAOS_DA_INIT_CAP 256
#endif // CHAOS_DA_INIT_CAP

/*
  ======== FILE RELATED UTILITIES =========
*/

CHAOSDEF CHAOS_BOOL chaos_read_file(char* file_name, Chaos_String_Builder *sb);
CHAOSDEF CHAOS_BOOL chaos_write_file(char* file_name, Chaos_String_Builder *sb);
CHAOSDEF CHAOS_BOOL chaos_does_file_exist(char *filename);
CHAOSDEF CHAOS_BOOL chaos_did_file_change(char *filename);
/*
  ======== STRING RELATED UTILITIES =========
*/

CHAOSDEF char* chaos_temp_sprintf(const char *fmt, ...);
CHAOSDEF Chaos_String_View chaos_sv_from_parts(const char* data, size_t count);
CHAOSDEF Chaos_String_View chaos_sv_from_cstr(char* data);
CHAOSDEF Chaos_String_View chaos_split_by_delim(Chaos_String_View *sv, char delim);
CHAOSDEF Chaos_String_View chaos_trim_left(Chaos_String_View *sv);
CHAOSDEF Chaos_String_View chaos_trim_right(Chaos_String_View *sv);
CHAOSDEF Chaos_String_View chaos_trim(Chaos_String_View *sv);
CHAOSDEF void chaos_sb_append_null(Chaos_String_Builder *sb);
CHAOSDEF void chaos_sb_append_cstr(Chaos_String_Builder *sb, char* s);
CHAOSDEF void chaos_sb_appendf(Chaos_String_Builder *sb, const char* fmt, ...);
CHAOSDEF Chaos_String_View chaos_sb_to_sv(Chaos_String_Builder *sb);
CHAOSDEF void chaos_printb(Chaos_String_Builder sb);
CHAOSDEF void chaos_printv(Chaos_String_View sv);
CHAOSDEF char *chaos_sv_to_cstr(Chaos_String_View *sv);
CHAOSDEF void chaos_sv_to_sb(Chaos_String_View *sv, Chaos_String_Builder *sb);
CHAOSDEF CHAOS_BOOL chaos_starts_with_b(Chaos_String_Builder sb, char *text);
CHAOSDEF CHAOS_BOOL chaos_starts_with_v(Chaos_String_View sv, char *text);

/*
  ================= Build System UTILITIES ===============
*/

CHAOSDEF void       chaos_cmd_append(Chaos_cmd_arr *arr, char* value);
CHAOSDEF CHAOS_BOOL chaos_cmd_run(Chaos_cmd_arr *arr);
CHAOSDEF void chaos_copy_file(char* original_name, char* clone_name); 
CHAOSDEF void       chaos_rebuild(int argc, char **argv, char* filename);

/*
  =================== MISC UTILITIES ===================
*/

CHAOSDEF CHAOS_BOOL chaos_is_float(char* v);
CHAOSDEF CHAOS_BOOL chaos_is_int(char* v);

/*
  ================== FLAG PARSING ======================
*/

CHAOSDEF void chaos_flags_print_help(const char *program_name, Chaos_Flag *flags, size_t flag_count); 
CHAOSDEF CHAOS_BOOL chaos_flags_parse(int argc, char **argv, Chaos_Flag *flags, size_t flag_count);

/*
  ================= Arena functions ===================
*/

CHAOSDEF void* chaos_arena_alloc(chaos_arena *a, size_t size_b);
CHAOSDEF void chaos_arena_free(chaos_arena *a);
CHAOSDEF void chaos_arena_reset(chaos_arena *a);
CHAOSDEF char* chaos_arena_sprintf(chaos_arena *a, const char* fmt, ...);

/*
  ================ Hash Table functions ================
*/

CHAOSDEF UINT32_T djb33_hash(char *s, size_t len);
CHAOSDEF UINT32_T chaos_hash_generic(char *value, size_t len, uint32_t (*custom_hash)(char *, size_t));
CHAOSDEF void chaos_table_append(chaos_Table *t, char *value, size_t len);
CHAOSDEF UINT32_T chaos_table_index(chaos_Table *t, char *value, size_t len);
CHAOSDEF void chaos_table_print(chaos_Table *t);
CHAOSDEF void chaos_table_rehash(chaos_Table *t, size_t new_bucket_count);
CHAOSDEF void chaos_table_free(chaos_Table *t);

#define chaos_hash(value, len) chaos_hash_generic((value), (len), djb33_hash)

#define chaos_print(sbv) _Generic((sbv),      \
    Chaos_String_Builder: chaos_printb,       \
    Chaos_String_View:    chaos_printv        \
  ) (sbv)

#define chaos_starts_with(S, f) _Generic((S),  \
  Chaos_String_Builder: chaos_starts_with_b,   \
  Chaos_String_View: chaos_starts_with_v       \
) ((S), (f))


#endif // CHAOS_H_

/*
  Functions in this library are Unnamespaced by default, to add the Chaos prefix do #define CHAOS_ADD_PREFIX
*/

#ifndef CHAOS_ADD_PREFIX
  #define TODO              CHAOS_TODO
  #define ARRAY_LEN         CHAOS_ARRAY_LEN
  #define da_append         chaos_da_append
  #define da_reserve        chaos_da_reserve
  #define String_Builder    Chaos_String_Builder
  #define String_View       Chaos_String_View
  #define read_file         chaos_read_file
  #define write_file        chaos_write_file
  #define does_file_exist   chaos_does_file_exist
  #define temp_sprintf      chaos_temp_sprintf
  #define sv_from_parts     chaos_sv_from_parts
  #define sv_from_cstr      chaos_sv_from_cstr
  #define split_by_delim    chaos_split_by_delim
  #define cmd_arr           Chaos_cmd_arr
  #define cmd_run           chaos_cmd_run
  #define cmd_append        chaos_cmd_append
  #define is_float          chaos_is_float
  #define is_int            chaos_is_int
  #define sb_append_null    chaos_sb_append_null
  #define sb_append_cstr    chaos_sb_append_cstr
  #define sb_appendf        chaos_sb_appendf
  #define trim_left         chaos_trim_left
  #define trim_right        chaos_trim_right
  #define trim              chaos_trim
  #define copy_file         chaos_copy_file
  #define rebuild           chaos_rebuild
  #define arena             chaos_arena
  #define arena_alloc       chaos_arena_alloc
  #define arena_free        chaos_arena_free
  #define arena_reset       chaos_arena_reset
  #define arena_sprintf     chaos_arena_sprintf
  #define sb_to_sv          chaos_sb_to_sv
  #define print             chaos_print
  #define printb            chaos_printb
  #define printv            chaos_printv
  #define sv_to_cstr        chaos_sv_to_cstr
  #define KV                chaos_KV
  #define Bucket            chaos_Bucket
  #define Table             chaos_Table
  #define hash_generic      chaos_hash_generic
  #define hash              chaos_hash
  #define table_append      chaos_table_append
  #define table_index       chaos_table_index
  #define table_print       chaos_table_print
  #define table_rehash      chaos_table_rehash
  #define sv_to_sb          chaos_sv_to_sb
  #define Flag              Chaos_Flag
  #define flags_print_help  chaos_flags_print_help
  #define flags_parse       chaos_flags_parse
  #define starts_with_b     chaos_starts_with_b
  #define starts_with_v     chaos_starts_with_v
  #define starts_with       chaos_starts_with
  #define table_free        chaos_table_free
#endif


/* 
  Here You Will find the Implementation for the above functions, to include implementations in your C file do
  #define CHAOS_IMPLEMENTATION | after including the header
*/

// #ifdef CHAOS_IMPLEMENTATION

/*
  ======== FILE RELATED UTILITIES =========
*/

#ifdef CHAOS_IMPLEMENTATION

#ifdef __unix
CHAOSDEF CHAOS_BOOL chaos_read_file(char* file_name, Chaos_String_Builder *sb)
{ 
  FILE *f = fopen(file_name, "rb");

  if (!f) {
    CHAOS_FPRINTF(stderr, "Cannot open file: <%s>\n", file_name);
    return false;
  }

  size_t new_count = 0;
  
  if (fseek(f, 0, SEEK_END) < 0) return false;
  
  size_t m = ftell(f);

  new_count = sb->count + m;

  if (new_count > sb->capacity){
    sb->items = (char*)CHAOS_REALLOC(sb->items, new_count * sizeof(char));
    sb->capacity = new_count;
  }
  
  fseek(f, 0, SEEK_SET);
  fread(sb->items + sb->count, 1, m, f);

  if (fclose(f) != 0) {
    CHAOS_FPRINTF(stderr, "Cannot close file <%s>\n", file_name);
    return false;
  }
  
  sb->count = new_count;
  return true;
}

CHAOSDEF CHAOS_BOOL chaos_write_file(char* file_name, Chaos_String_Builder *sb)
{ 
  FILE *f = fopen(file_name, "wb");

  if (!f) {
    CHAOS_FPRINTF(stderr, "Cannot open file <%s>\n", file_name);
    return false;
  }
  
  size_t written = fwrite(sb->items, 1, sb->count, f);
  if (written != sb->count){
    CHAOS_FPRINTF(stderr, "Failed to write file <%s>\n", file_name);
    return false;
  }

  if (fclose(f) != 0){
    CHAOS_FPRINTF(stderr, "Cannot close file <%s>\n", file_name);
    return false;
  }
  return true;
}

CHAOSDEF CHAOS_BOOL chaos_does_file_exist(char *filename){
  if (access(filename, F_OK) == 0) return true;
  return false;
}

CHAOSDEF CHAOS_BOOL chaos_did_file_change(char *filename){
  struct stat a, b;
  
  char* old_file = chaos_temp_sprintf("%s.old", filename);
  if (!chaos_does_file_exist(old_file)) return true;

  stat(filename, &a);
  stat(old_file, &b);
  
  return difftime(a.st_mtime, b.st_mtime) > 0; 
}
#endif

/*
  ======== STRING RELATED UTILITIES =========
*/

CHAOSDEF char* chaos_temp_sprintf(const char *fmt, ...) {
    static char bufs[CHAOS_TMP_BUF_COUNT][CHAOS_TMP_BUF_SIZE];
    static size_t idx = 0;

    char *out = bufs[idx++ % CHAOS_TMP_BUF_COUNT];

    va_list ap;
    va_start(ap, fmt);
    CHAOS_VSNPRINTF(out, CHAOS_TMP_BUF_SIZE, fmt, ap);
    va_end(ap);

    return out;
}

CHAOSDEF Chaos_String_View chaos_sv_from_parts(const char* data, size_t count){
  Chaos_String_View sv;
  sv.data = data;
  sv.count = count;
  return sv;
}

CHAOSDEF Chaos_String_View chaos_sv_from_cstr(char* data){
  size_t count = CHAOS_STRLEN(data); 
  Chaos_String_View sv;
  sv.data = data;
  sv.count = count;
  return sv;
}


CHAOSDEF Chaos_String_View chaos_split_by_delim(Chaos_String_View *sv, char delim){
  size_t i = 0;

  while (i < sv->count && sv->data[i] != delim){
    i++;
  }
  Chaos_String_View result = chaos_sv_from_parts(sv->data, i);
  if (i < sv->count){
    sv->count -= i+1;
    sv->data  += i+1;
  } else {
    sv->count -= i;
    sv->data  += i;
  }

  return result;
}

CHAOSDEF Chaos_String_View chaos_trim_left(Chaos_String_View *sv){
  Chaos_String_View result = {0};

  result.data = sv->data;
  result.count = sv->count;

  size_t i=0;

  while (i < result.count && result.data[i] == ' '){
    i++;
  }

  result.count -=i;
  result.data +=i;

  return result;
}

CHAOSDEF Chaos_String_View chaos_trim_right(Chaos_String_View *sv) {
  Chaos_String_View result = {0};

  result.data  = sv->data;
  result.count = sv->count;

  size_t i = result.count;

  while (i > 0 && result.data[i - 1] == ' ') {
    --i;
  }

  result.count = i;

  return result;
}

CHAOSDEF Chaos_String_View chaos_trim(Chaos_String_View *sv){
  Chaos_String_View tmp = chaos_trim_left(sv);
  return chaos_trim_right(&tmp);
}

CHAOSDEF void chaos_sb_append_null(Chaos_String_Builder *sb){
  chaos_da_append(sb, '\0');  
}

CHAOSDEF void chaos_sb_append_cstr(Chaos_String_Builder *sb, char* s){
  while (*s){
    chaos_da_append(sb, *s++);
  }
}

CHAOSDEF void chaos_sb_appendf(Chaos_String_Builder *sb, const char* fmt, ...){
  char buf[CHAOS_TMP_BUF_SIZE];

  va_list ap;
  va_start(ap, fmt);
  CHAOS_VSNPRINTF(buf, sizeof(buf), fmt, ap);
  va_end(ap);

  chaos_sb_append_cstr(sb, buf);
}

CHAOSDEF Chaos_String_View chaos_sb_to_sv(Chaos_String_Builder *sb){
  return chaos_sv_from_parts(sb->items, sb->count);
}

CHAOSDEF void chaos_printb(Chaos_String_Builder sb){
  CHAOS_PRINTF("%.*s\n", (int)sb.count, sb.items);
}

CHAOSDEF void chaos_printv(Chaos_String_View sv){
  CHAOS_PRINTF("%.*s\n", (int)sv.count, sv.data);
}

CHAOSDEF char *chaos_sv_to_cstr(Chaos_String_View *sv) {
  char *buf = (char*)CHAOS_REALLOC(NULL, sv->count + 1);
  CHAOS_MEMCPY(buf, sv->data, sv->count);
  buf[sv->count] = '\0';
  return buf;
}

CHAOSDEF void chaos_sv_to_sb(Chaos_String_View *sv, Chaos_String_Builder *sb) {
  for (size_t i = 0; i < sv->count; ++i) chaos_da_append(sb, sv->data[i]);
}

CHAOSDEF CHAOS_BOOL chaos_starts_with_b(Chaos_String_Builder sb, char *text) {
  size_t i = 0;
  while (text[i] != '\0') {
    if (i >= sb.count) return false;
    if (sb.items[i] != text[i]) return false;
    i++;
  }
  return true;
}

CHAOSDEF CHAOS_BOOL chaos_starts_with_v(Chaos_String_View sv, char *text) {
  size_t i = 0;
  while (text[i] != '\0') {
    if (i >= sv.count) return false;
    if (sv.data[i] != text[i]) return false;
    i++;
  }
  return true;
}


/*
  ================= Build System Utils ===============
*/
CHAOSDEF void chaos_cmd_append(Chaos_cmd_arr *arr, char* value){
  size_t len = CHAOS_STRLEN(value);

  Chaos_String_View sv = {0};
  sv.data = value;
  sv.count = len;
  
  chaos_da_append(arr, sv);
}

#if defined(__unix__) || defined(_WIN32)
CHAOSDEF CHAOS_BOOL chaos_cmd_run(Chaos_cmd_arr *arr) {
  if (arr->count < 1) {
    CHAOS_FPRINTF(stderr, "USAGE: provide more parameters\n");
    return false;
  }

  size_t total_len = 0;
    
  for (size_t i = 0; i < arr->count; i++) {
    total_len += arr->items[i].count;
    if (i + 1 < arr->count) total_len += 1;
  }

  char *cmd = (char*)CHAOS_REALLOC(NULL, total_len + 1);

  if (!cmd) {
    CHAOS_FPRINTF(stderr, "Allocation failed in cmd_run\n");
    return false;
  }

  size_t pos = 0;

  for (size_t i = 0; i < arr->count; i++) {
    const char *s = arr->items[i].data;
    size_t n = arr->items[i].count;

    for (size_t j=0; j<n; ++j){
      cmd[pos++] = s[j];
    }
    
    if (i+1 < arr->count) cmd[pos++] = ' ';
  }

  cmd[pos] = '\0';

  CHAOS_PRINTF("[CMD] %s\n", cmd);

  int ret = system(cmd);
  if (ret == -1) perror("system");

  CHAOS_FREE(cmd);
  arr->count = 0;
  
  return ret == 0; 
}

CHAOSDEF void chaos_copy_file(char* original_name, char* clone_name){
  Chaos_String_Builder sb = {0};
  
  chaos_read_file(original_name, &sb);
  chaos_write_file(clone_name, &sb);
} 

CHAOSDEF void chaos_rebuild(int argc, char **argv, char* filename){
  char* old = chaos_temp_sprintf("%s.old", filename);

  if (!chaos_does_file_exist(old)){
    chaos_copy_file(filename, old);
    return;
  }

  if (!chaos_did_file_change(filename)) return;
  
  CHAOS_PRINTF("[Rebuilding]\n");

  chaos_copy_file(filename, old);

  Chaos_cmd_arr cmd = {0};

  #ifdef _WIN32
    chaos_cmd_append(&cmd, "cl.exe");
  #else
    chaos_cmd_append(&cmd, "gcc");
  #endif
  chaos_cmd_append(&cmd, "-o");
  chaos_cmd_append(&cmd, argv[0]);
  chaos_cmd_append(&cmd, filename);      
  chaos_cmd_run(&cmd);

  CHAOS_PRINTF("[INFO] rebuilt %s\n\n", filename);

  for (size_t i=0; i<argc; ++i) chaos_cmd_append(&cmd, argv[i]);

  chaos_cmd_run(&cmd);
  exit(0);
  return;
}
#endif

/*
  =================== MISC Utilities ===================
*/

CHAOSDEF CHAOS_BOOL chaos_is_float(char* v) {
  size_t len = CHAOS_STRLEN(v);
  size_t dots = 0;
  size_t sign = 0;
  CHAOS_BOOL has_digit = false;

  if (len == 0) return false;

  if (v[0] == '-' || v[0] == '+') sign = 1;

  for (size_t i = sign; i < len; ++i) {
    if (v[i] == '.') {
      dots++;
      if (dots > 1) return false;
    } else if ((unsigned char)v[i] >= '0' && (unsigned char)v[i] <= '9') {
        has_digit = true;
    } else {
        return false;
    }
  }
  return dots == 1 && has_digit;
}

CHAOSDEF CHAOS_BOOL chaos_is_int(char* v){
  size_t len = CHAOS_STRLEN(v);
  size_t sign = 0;

  if (len==0) return false;

  if (v[0] == '+' || v[0] == '-') sign=1;
  
  for (size_t i=sign; i<len; ++i){
    if ((unsigned char)v[i] < '0' || (unsigned char)v[i] > '9') return false;
  }
  return true;
}

CHAOSDEF void* chaos_arena_alloc(chaos_arena *a, size_t size_b){
  size_t offset = a->count;
  
  da_reserve(a, a->count + size_b);
  a->count += size_b;
  
  return a->items + offset;
}

CHAOSDEF void chaos_arena_free(chaos_arena *a){
  CHAOS_FREE(a->items);
  a->items = NULL;
  a->count = 0;
  a->capacity = 0;
}

CHAOSDEF void chaos_arena_reset(chaos_arena *a){
  a->count = 0;
}

CHAOSDEF char* chaos_arena_sprintf(chaos_arena *a, const char* fmt, ...){
  va_list ap, ap2;

  
  va_start(ap, fmt);
  va_copy(ap2, ap);

  size_t len = (size_t)CHAOS_VSNPRINTF(NULL, 0, fmt, ap);
  va_end(ap);
  
  if (len < 0){
    va_end(ap2);
    return NULL;
  }

  char* buf = (char*)chaos_arena_alloc(a, sizeof(char) * (len+1)); 
  CHAOS_VSNPRINTF(buf, len+1, fmt, ap2);
  va_end(ap2);

  return buf;
}

CHAOSDEF UINT32_T djb33_hash(char *s, size_t len) {
  uint32_t h = 5381;
  while (len--) {
    h += (h << 5);
    h ^= *s++;
  }
  return h;
}

CHAOSDEF UINT32_T chaos_hash_generic(char *value, size_t len,
                            UINT32_T (*custom_hash)(char *, size_t)) {
  return custom_hash(value, len);
}


CHAOSDEF void chaos_table_append(chaos_Table *t, char *value, size_t len) {
  if (t->items == NULL) {
    t->count = 16;
    t->len = 0;
    t->items = (chaos_Bucket*)CHAOS_CALLOC(t->count, sizeof(chaos_Bucket));
  } if (t->len*4 >= t->count * 3){
    chaos_table_rehash(t, t->count*2);
  }

  UINT32_T key = chaos_hash(value, len);
  chaos_Bucket *bucket = &t->items[key % t->count];

  chaos_KV *found = NULL;

  for (size_t i = 0; i < bucket->count; ++i) {
    chaos_KV *kv = &bucket->items[i];

    if (kv->key == key && CHAOS_STRCMP(kv->value, value) == 0) {
      found = kv;
      break;
    }
  }

  if (found) {
    found->freq++;
  } else {
    chaos_da_append(bucket, ((chaos_KV){
                                .key = key,
                                .value = CHAOS_STRDUP(value),
                                .freq = 1,
                            }));
    t->len++;
  }
}

CHAOSDEF void chaos_table_free(chaos_Table *t) {
  for (size_t i = 0; i < t->count; ++i) {
    chaos_Bucket *b = &t->items[i];

    for (size_t j = 0; j < b->count; ++j) {
      CHAOS_FREE(b->items[j].value);
    }
    CHAOS_FREE(b->items);
  }

  CHAOS_FREE(t->items);
  t->items = NULL;
  t->count = 0;
  t->capacity = 0;
  t->len = 0;
}

CHAOSDEF UINT32_T chaos_table_index(chaos_Table *t, char *value, size_t len) {
  return chaos_hash(value, len) % t->count;
}

CHAOSDEF void chaos_table_print(chaos_Table *t) {
  for (size_t i = 0; i < t->count; ++i) {
    chaos_Bucket *b = &t->items[i];

    if (b->count == 0)
      continue;

    for (size_t j = 0; j < b->count; ++j) {
      CHAOS_PRINTF("value = %s\n", b->items[j].value);
      CHAOS_PRINTF("key = %u\n", b->items[j].key);
      CHAOS_PRINTF("freq %zu\n", b->items[j].freq);
      CHAOS_PRINTF("-----------\n");
    }
  }
}

CHAOSDEF void chaos_table_rehash(chaos_Table *t, size_t new_bucket_count) {
  chaos_Bucket *old_items = t->items;
  size_t old_count = t->count;

  t->items = (chaos_Bucket*)(new_bucket_count, sizeof(chaos_Bucket));
  t->count = new_bucket_count;
  t->len = 0;

  for (size_t i = 0; i < old_count; ++i) {
    chaos_Bucket *b = &old_items[i];

    for (size_t j = 0; j < b->count; ++j) {
      chaos_KV *kv = &b->items[j];
      UINT32_T key = kv->key;
      chaos_Bucket *new_bucket = &t->items[key % t->count];
      chaos_da_append(new_bucket, *kv);
      t->len++;
    }

    CHAOS_FREE(b->items);
  }

  CHAOS_FREE(old_items);
}

CHAOSDEF void chaos_flags_print_help(const char *program_name, Chaos_Flag *flags,
                            size_t flag_count) {
  CHAOS_PRINTF("Usage: %s [options]\n\n", program_name);
  CHAOS_PRINTF("Options:\n");

  for (size_t i = 0; i < flag_count; ++i) {
    Chaos_Flag *f = &flags[i];

    CHAOS_PRINTF("  ");

    if (f->short_name.count > 0) {
      CHAOS_PRINTF("-%.*s", (int)f->short_name.count, f->short_name.data);
    }

    if (f->long_name.count > 0) {
      if (f->short_name.count > 0)
        CHAOS_PRINTF(", ");
      CHAOS_PRINTF("--%.*s", (int)f->long_name.count, f->long_name.data);
    }

    if (f->desc.count > 0) {
      CHAOS_PRINTF("\n      %.*s", (int)f->desc.count, f->desc.data);
    }

    CHAOS_PRINTF("\n");
  }
}

#if defined(__unix__) || defined(_WIN32)
CHAOSDEF CHAOS_BOOL chaos_flags_parse(int argc, char **argv, Chaos_Flag *flags,
                                size_t flag_count) {

  if (argc == 1) {
    chaos_flags_print_help(argv[0], flags, flag_count);
    return 0;
  }
  
  for (size_t i = 1; i < argc; ++i) {
    char *arg = argv[i];
    if (arg[0] != '-')
      continue;

    Chaos_String_View sv = chaos_sv_from_cstr(arg);
    CHAOS_BOOL is_long = false;

    if (sv.count >= 2 && sv.data[0] == '-' && sv.data[1] == '-') {
      sv.data += 2;
      sv.count -= 2;
      is_long = true;
    } else if (sv.count >= 1 && sv.data[0] == '-') {
      sv.data += 1;
      sv.count -= 1;
    } else {
      continue;
    }

    Chaos_String_View name = sv;
    Chaos_String_View value = {0};

    Chaos_String_View tmp = sv;
    Chaos_String_View left = split_by_delim(&tmp, '=');

    if (tmp.count > 0) {
      name = left;
      value = tmp;
    }

    CHAOS_BOOL matched = false;
    CHAOS_BOOL help_requested = false;

    for (size_t f = 0; f < flag_count; ++f) {
      Chaos_Flag *flag = &flags[f];

      Chaos_String_View match = is_long ? flag->long_name : flag->short_name;

      if (match.data == NULL || match.count == 0) {
        continue;
      }

      if ((is_long && name.count == 4 && CHAOS_MEMCMP(name.data, "help", 4) == 0) ||
          (!is_long && name.count == 1 && CHAOS_MEMCMP(name.data, "h", 1) == 0)) {
        help_requested = true;
        continue;
      }

      if (match.count == name.count &&
          CHAOS_MEMCMP(match.data, name.data, name.count) == 0) {
        flag->present = true;
        flag->value = value;
        matched = true;
        if (flag->dispatcher)
          flag->dispatcher();
        break;
      }
    }

    if (!matched) {
      Chaos_String_View program = chaos_sv_from_cstr(argv[0]);
      split_by_delim(&program, '/');
      CHAOS_FPRINTF(stderr, "%.*s: error: unrecognized command-line option '%.*s'\n",
              (int)program.count, program.data, (int)name.count, name.data);
      chaos_flags_print_help(argv[0], flags, flag_count);
      return false;
    }
  }
  return true;
}
#endif

#endif // CHAOS_IMPLEMENTATION


#ifdef CHAOS_GC
static chaos_arena *__chaos_gc;
int chaos_entry(int argc, char **argv);

#undef arena_alloc
#undef arena_sprintf

#define arena_alloc(size_b) \
  chaos_arena_alloc(__chaos_gc, (size_b))

#define arena_sprintf(fmt, ...) \
  chaos_arena_sprintf(__chaos_gc, (fmt), ##__VA_ARGS__)

#undef main
#define main(...)                                               \
  main(int argc, char **argv) {                             \
    static chaos_arena __arena = {0};                           \
    __chaos_gc = &__arena;                                      \
    int __ret = chaos_entry(argc, argv);                        \
    chaos_arena_free(__chaos_gc);                               \
    return __ret;                                               \
  }                                                             \
  int chaos_entry(__VA_ARGS__)
#endif // CHAOS_GC
