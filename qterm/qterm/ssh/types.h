#ifndef TYPES_H
#define TYPES_H

#include <qglobal.h>
#if !defined(Q_OS_WIN32) && !defined(_OS_WIN32_)
#include <sys/types.h>
#else
typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned long u_long;
typedef Q_UINT32 u_int32_t;
typedef Q_UINT64 u_int64_t;
#endif // Q_OS_WIN32

#endif // TYPES_H

