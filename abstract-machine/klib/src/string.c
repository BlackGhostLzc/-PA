#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t len = 0;
  char* t = (char*)s;
  while(*t != '\0'){
    len++;
    t++;
  }
  return len;
}

char *strcpy(char *dst, const char *src) {
  int i;
  for (i = 0; src[i] != '\0'; ++i){
    dst[i] = src[i];
  }
  dst[i] = '\0';

  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  panic("Not implemented");
}

char *strcat(char *dst, const char *src) {
  size_t len = strlen(dst);
  char* s = (char*)src;
  size_t i = 0;
  while(*s != '\0'){
    dst[len + i] = *s;
    s++;
    i++;
  }
  dst[len + i] = '\0';
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  char* t1 = (char*)s1;
  char* t2 = (char*)s2;
  while(*t1 != '\0' && *t2 != '\0'){
    if(*t1 != *t2){
      return (int)(*t1) - (int)(*t2);
    }
    t1++;
    t2++;
  }

  return 0;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  panic("Not implemented");
}

void *memset(void *s, int c, size_t n) {
  char* t = (char*)s;
  for(size_t i = 0; i < n; i++){
    t[i] = (char)(c);
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n) {
  char* o = (char*)out;
  char* i = (char*)in;
  for(size_t j = 0; j < n; j++){
    *(o + j) = *(i + j);
  }
  return out;
  // panic("Not implemented");
}

int memcmp(const void *s1, const void *s2, size_t n) {
  panic("Not implemented");
}

#endif
