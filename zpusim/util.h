#ifndef UTIL_H
#define UTIL_H 1

uint8_t *file_read(const char *name, size_t *retlen);
void dump(const uint8_t *data, size_t len);

#endif

