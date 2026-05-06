#ifndef NEWAGE_BASE_STD_ENDIAN_H
#define NEWAGE_BASE_STD_ENDIAN_H

// warning C4505: : unreferenced local function has been removed
#if defined WIN32
  #pragma warning(disable:4505)
#endif

#include "Base/Common/Types.h"

static inline uint16_t swap16(uint16_t i)
{
  return (uint16_t)(((((uint32_t) i) & 0xff00) >> 8)
    | ((((uint32_t) i) & 0xff) << 8));
}

static inline uint32_t swap32(uint32_t i)
{
  return (uint32_t)(((((uint32_t) i) & 0xff000000) >> 24)
    | ((((uint32_t) i) & 0xff0000) >> 8)
    | ((((uint32_t) i) & 0xff00) << 8)
    | ((((uint32_t) i) & 0xff) << 24));
}

static inline uint64_t swap64(uint64_t i)
{
  return (((uint64_t) ( swap32((uint32_t) i))) << 32) | ((uint64_t) (swap32((uint32_t) (i >> 32))));
}

static inline uint32_t endianReadU8Little(uint8_t* mem)
{
  return *mem;
}

static inline uint32_t endianReadU8Big(uint8_t* mem)
{
  return *mem;
}

static inline int endianReadS8Little(int8_t* mem)
{
  return *mem;
}

static inline int endianReadS8Big(int8_t* mem)
{
  return *mem;
}

static inline uint32_t endianReadU16Little(uint16_t* mem)
{
#ifdef NEWAGE_BIG_ENDIAN
  return swap16(*mem);
#else
  return *mem;
#endif
}

static inline uint32_t endianReadU16Big(uint16_t* mem)
{
#ifdef NEWAGE_BIG_ENDIAN
  return *mem;
#else
  return swap16(*mem);
#endif
}

static inline int endianReadS16Little(int16_t* mem)
{
#ifdef NEWAGE_BIG_ENDIAN
  return swap16(*mem);
#else
  return *mem;
#endif
}

static inline int endianReadS16Big(int16_t* mem)
{
#ifdef NEWAGE_BIG_ENDIAN
  return *mem;
#else
  return swap16(*mem);
#endif
}

static inline uint32_t endianReadU32Little(uint32_t* mem)
{
#ifdef NEWAGE_BIG_ENDIAN
  return swap32(*mem);
#else
  return *mem;
#endif
}

static inline uint32_t endianReadU32Big(uint32_t* mem)
{
#ifdef NEWAGE_BIG_ENDIAN
  return *mem;
#else
  return swap32(*mem);
#endif
}

static inline int endianReadS32Little(int32_t* mem)
{
#ifdef NEWAGE_BIG_ENDIAN
  return swap32(*mem);
#else
  return *mem;
#endif
}

static inline int endianReadS32Big(int32_t* mem)
{
#ifdef NEWAGE_BIG_ENDIAN
  return *mem;
#else
  return swap32(*mem);
#endif
}

static inline float endianReadFloat32Little(float* mem)
{
#ifdef NEWAGE_BIG_ENDIAN
  union
  {
    uint32_t i;
    float f;
  } x;
  x.i = swap32(*((uint32_t*) mem));
  return x.f;
#else
  return *mem;
#endif
}

static inline float endianReadFloat32Big(float* mem)
{
#ifdef NEWAGE_BIG_ENDIAN
  return *mem;
#else
  union
  {
    uint32_t i;
    float f;
  } x;
  x.i = swap32(*((uint32_t*) mem));
  return x.f;
#endif
}

static inline double endianReadFloat64Little(double* mem)
{
#ifdef NEWAGE_BIG_ENDIAN
  union
  {
    uint64_t i;
    double d;
  } x;
  x.i = swap64(*((uint64_t*) mem));
  return x.d;
#else
  return *mem;
#endif
}

static inline double endianReadFloat64Big(double* mem)
{
#ifdef NEWAGE_BIG_ENDIAN
  return *mem;
#else
  union
  {
    uint64_t i;
    double d;
  } x;
  x.i = swap64(*((uint64_t*) mem));
  return x.d;
#endif
}

static inline float endianReadFloatLittle(float* mem)
{
  return endianReadFloat32Little(mem);
}

static inline float endianReadFloatBig(float* mem)
{
  return endianReadFloat32Big(mem);
}

static inline double endianReadDoubleLittle(double* mem)
{
  return endianReadFloat64Little(mem);
}

static inline double endianReadDoubleBig(double* mem)
{
  return endianReadFloat64Big(mem);
}


static inline void endianWriteU8Little(uint8_t* mem, uint8_t data)
{
  *mem = data;
}

static inline void endianWriteU8Big(uint8_t* mem, uint8_t data)
{
  *mem = data;
}

static inline void endianWriteS8Little(int8_t* mem, int8_t data)
{
  *mem = data;
}

static inline void endianWriteS8Big(int8_t* mem, int8_t data)
{
  *mem = data;
}

static inline void endianWriteU16Little(uint16_t* mem, uint16_t data)
{
#ifdef NEWAGE_BIG_ENDIAN
  *mem = swap16(data);
#else
  *mem = data;
#endif
}

static inline void endianWriteU16Big(uint16_t* mem, uint16_t data)
{
#ifdef NEWAGE_BIG_ENDIAN
  *mem = data;
#else
  *mem = swap16(data);
#endif
}

static inline void endianWriteS16Little(int16_t* mem, int16_t data)
{
  endianWriteU16Little((uint16_t*) mem, (uint16_t) data);
}

static inline void endianWriteS16Big(int16_t* mem, int16_t data)
{
  endianWriteU16Big((uint16_t*) mem, (uint16_t) data);
}

static inline void endianWriteU32Little(uint32_t* mem, uint32_t data)
{
#ifdef NEWAGE_BIG_ENDIAN
  *((uint32_t*) mem) = swap32(data);
#else
  *((uint32_t*) mem) = data;
#endif
}

static inline void endianWriteU32Big(uint32_t* mem, uint32_t data)
{
#ifdef NEWAGE_BIG_ENDIAN
  *mem = data;
#else
  *mem = swap32(data);
#endif
}

static inline void endianWriteS32Little(int32_t* mem, int32_t data)
{
  endianWriteU32Little((uint32_t*) mem, (uint32_t) data);
}

static inline void endianWriteS32Big(int32_t* mem, int32_t data)
{
  endianWriteU32Big((uint32_t*) mem, (uint32_t) data);
}

static inline void endianWriteFloat32Little(float* mem, float data)
{
#ifdef NEWAGE_BIG_ENDIAN
  union
  {
    uint32_t i;
    float f;
  } x;
  x.f = data;
  x.i = swap32(x.i);
  *mem= x.f;
#else
  *mem = data;
#endif
}

static inline void endianWriteFloat32Big(float* mem, float data)
{
#ifdef NEWAGE_BIG_ENDIAN
  *mem = data;
#else
  union
  {
    uint32_t i;
    float f;
  } x;
  x.f = data;
  x.i = swap32(x.i);
  *mem = x.f;
#endif
}

static inline void endianWriteFloat64Little(double* mem, double data)
{
#ifdef NEWAGE_BIG_ENDIAN
  union
  {
    uint64_t i;
    double d;
  } x;
  x.d = data;
  x.i = swap64(x.i);
  *mem = x.d;
#else
  *mem = data;
#endif
}

static inline void endianWriteFloat64Big(double* mem, double data)
{
#ifdef NEWAGE_BIG_ENDIAN
  *mem = data;
#else
  union
  {
    uint64_t i;
    double d;
  } x;
  x.d = data;
  x.i = swap64(x.i);
  *mem = x.d;
#endif
}

static inline void endianWriteFloatLittle(float* mem, float data)
{
  endianWriteFloat32Little(mem, data);
}

static inline void endianWriteFloatBig(float* mem, float data)
{
  endianWriteFloat32Big(mem, data);
}

static inline void endianWriteDoubleLittle(double* mem, double data)
{
  endianWriteFloat64Little(mem, data);
}

static inline void endianWriteDoubleBig(double* mem, double data)
{
  endianWriteFloat64Big(mem, data);
}
#endif
