#ifndef __UTIL_FUNC_H__
#define __UTIL_FUNC_H__

#ifndef TRUE
#define TRUE   1
#endif
#define FALSE  0

#define max(a,b)					((a > b) ? a : b)
#define min(a,b)					((a < b) ? a : b)

#define sat(x, xmin, xmax)		min(xmax, max(xmin, x))

#define eps		0.000001
#define pi		3.141593
#define PREC	100


#endif // __UTIL_FUNC_H__
