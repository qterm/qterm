#include "getput.h"

u_int32_t get_u32(const void * vp)
{
    u_int32_t v;
    const u_char * p = (const u_char *) vp;
    v  = (u_int32_t) p[0] << 24;
    v |= (u_int32_t) p[1] << 16;
    v |= (u_int32_t) p[2] << 8;
    v |= (u_int32_t) p[3];
    return v;
}

u_int16_t get_u16(const void *vp)
{
    const u_char *p = (const u_char *) vp;
    u_int16_t v;

    v  = (u_int16_t) p[0] << 8;
    v |= (u_int16_t) p[1];

    return (v);
}

void put_u32(void * vp, u_int32_t v)
{
    u_char * p = (u_char *) vp;

    p[0] = (u_char)(v >> 24) & 0xff;
    p[1] = (u_char)(v >> 16) & 0xff;
    p[2] = (u_char)(v >> 8) & 0xff;
    p[3] = (u_char) v & 0xff;
}

void put_u16(void * vp, u_int16_t v)
{
    u_char * p = (u_char *) vp;
    p[0] = (u_char)(v >> 8) & 0xff;
    p[1] = (u_char) v & 0xff;
}
