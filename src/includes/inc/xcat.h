#ifndef _XCAT_H
#define _XCAT_H 1

#define cat(a, b) a##b
#define cat3(a, b, c) a##b##c
#define cat4(a, b, c, d) a##b##c##d
#define cat5(a, b, c, d, e) a##b##c##d##e
#define xcat(a, b) cat(a, b)
#define xcat3(a, b, c) cat3(a, b, c)
#define xcat4(a, b, c, d) cat4(a, b, c, d)
#define xcat5(a, b, c, d, e) cat5(a, b, c, d, e)

#endif
