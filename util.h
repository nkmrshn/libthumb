#ifndef UTIL_H
#define UTIL_H
 
#include <stdbool.h>

unsigned long todecimal(unsigned char *, size_t);
int strrev(unsigned char *, size_t);
void vseek(FILE *, long, int, bool *);
void vread(void *, size_t, FILE *, bool *);
void vrev(void *, size_t, bool *, bool *);
void vcmp(const unsigned char *, void *, size_t, bool *);
void read_exif_header(FILE *, unsigned char *, bool *, bool *);

#endif
