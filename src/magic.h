#ifndef MAGIC_H
#define MAGIC_H

// Paste
#define P(...) P2(__VA_ARGS__,,,,,,,,,,)
#define P2(a,b,c,d,e,f,g,h,i,j,...) a##b##c##d##e##f##g##h##i##j

#endif
