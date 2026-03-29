#pragma once

typedef __builtin_va_list va_list;
typedef __builtin_va_list __isoc_va_list;
typedef unsigned long long size_t;

#ifndef P_MALLOC
#define P_MALLOC __builtin_malloc
#endif

#ifndef WRITE_CHAR
#ifdef __unix__
#include <unistd.h>
static int write_char(char c) { return write(1, &(c), 1); }
#define WRITE_CHAR write_char
#endif
#endif

#define va_start(v, l) __builtin_va_start(v, l)
#define va_end(v) __builtin_va_end(v)
#define va_arg(v, l) __builtin_va_arg(v, l)
#define va_copy(d, s) __builtin_va_copy(d, s)

void *simp_memcpy(void *dst, const void *src, unsigned long n) {
  unsigned char *d = dst;
  const unsigned char *s = src;
  while (n--) {
    *d++ = *s++;
  }
  return dst;
}

int simp_memcmp(const void *s1, const void *s2, size_t n) {
  const unsigned char *p1 = s1, *p2 = s2;
  while (n--) {
    if (*p1 != *p2)
      return *p1 - *p2;
    p1++;
    p2++;
  }
  return 0;
}

static int get_strlen(char *s) {
  int size = 0;
  while (s[size] != '\0') {
    size++;
  }
  return size;
}

int simp_strcmp(const char *s1, const char *s2) {
  while (*s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }
  return *(unsigned char *)s1 - *(unsigned char *)s2;
}

char *simp_strdup(char *s) {
  size_t len = get_strlen(s);
  char *buf = P_MALLOC(len + 1);

  for (size_t i = 0; i < len; ++i)
    buf[i] = s[i];

  buf[len] = '\0';
  return buf;
}

static char *get_itoa(int n) {
  char *buf = P_MALLOC(12);
  int i = 10;
  int neg = 0;
  unsigned int num;

  buf[11] = '\0';

  if (n == 0) {
    buf[10] = '0';
    return &buf[10];
  }

  if (n < 0) {
    neg = 1;
    num = (unsigned int)-n;
  } else {
    num = (unsigned int)n;
  }

  while (num > 0) {
    buf[i--] = (num % 10) + '0';
    num /= 10;
  }

  if (neg) {
    buf[i--] = '-';
  }

  return &buf[i + 1];
}
static int write_str(char *s) {
  size_t i = 0;

  while (i < get_strlen(s)) {
    WRITE_CHAR(s[i]);
    i++;
  }
  return i;
}

static int write_ptr(void *p) {
  char buf[20];
  unsigned long addr = (unsigned long)p;
  char *hex_chars = "0123456789abcdef";
  int i = 18;
  buf[19] = '\0';

  if (addr == 0)
    return write_str("0x0");

  while (addr > 0) {
    buf[i--] = hex_chars[addr % 16];
    addr /= 16;
  }

  buf[i--] = 'x';
  buf[i--] = '0';
  i++;

  return write_str(&buf[i]);
}

static int write_hex(void *p) {
  char buf[20];
  unsigned long addr = (unsigned long)p;
  char *hex_chars = "0123456789abcdef";
  int i = 18;
  buf[19] = '\0';

  if (addr == 0)
    return write_str("0x0");

  while (addr > 0) {
    buf[i--] = hex_chars[addr % 16];
    addr /= 16;
  }
  return write_str(&buf[i]);
}

static int write_sized_str(char *str, size_t n) {
  for (size_t i = 0; i < n; ++i)
    WRITE_CHAR(str[i]);
  return n;
}

static int write_int_recursive(unsigned int n) {
  int count = 0;
  if (n / 10) {
    count += write_int_recursive(n / 10);
  }
  char c = (n % 10) + '0';
  WRITE_CHAR(c);
  return count + 1;
}

static int write_int(int n) {
  int count = 0;
  unsigned int num;

  if (n == 0) {
    return WRITE_CHAR('0');
  }

  if (n < 0) {
    count += WRITE_CHAR('-');
    num = (unsigned int)-n;
  } else {
    num = (unsigned int)n;
  }

  return count + write_int_recursive(num);
}

int printf(const char *fmt, ...) {
  va_list args;
  int count = 0;
  va_start(args, fmt);

  while (*fmt) {
    if (*fmt == '%' && *(fmt + 1)) {
      fmt++;
      if (*fmt == '.' && *(fmt + 1) == '*') {
        fmt += 2;
        int len = va_arg(args, int);
        if (*fmt == 's') {
          fmt++;
          char *str = va_arg(args, char *);
          count += write_sized_str(str, len);
        }
      } else if (*fmt == 's') {
        count += write_str(va_arg(args, char *));
        fmt++;
      } else if (*fmt == 'd') {
        count += write_int(va_arg(args, int));
        fmt++;
      } else if (*fmt == 'c') {
        count += WRITE_CHAR(va_arg(args, int));
        fmt++;
      } else if (*fmt == 'p') {
        count += write_ptr(va_arg(args, void *));
        fmt++;
      } else if (*fmt == 'x') {
        count += write_hex(va_arg(args, void *));
        fmt++;
      } else {
        count += WRITE_CHAR(*fmt);
        fmt++;
      }
    } else {
      count += WRITE_CHAR(*fmt);
      fmt++;
    }
  }
  va_end(args);
  return count;
}
