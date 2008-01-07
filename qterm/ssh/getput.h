#ifndef GETPUT_H
#define GETPUT_H

#include "types.h"

u_int32_t get_u32(const void * vp);
u_int16_t get_u16(const void *vp);
void put_u32(void * vp, u_int32_t v);
void put_u16(void * vp, u_int16_t v);

#endif
