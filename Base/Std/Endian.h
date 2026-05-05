#ifndef NEWAGE_BASE_STD_ENDIAN_H
#define NEWAGE_BASE_STD_ENDIAN_H

// warning C4505: : unreferenced local function has been removed
#if defined WIN32
  #pragma warning(disable:4505)
#endif

static inline unsigned short swap16(unsigned short i)
{
  return (unsigned short)(((((unsigned int) i) & 0xff00) >> 8)
    | ((((unsigned int) i) & 0xff) << 8));
}

static inline unsigned int swap32(unsigned int i)
{
  return (unsigned int)(((((unsigned int) i) & 0xff000000) >> 24)
    | ((((unsigned int) i) & 0xff0000) >> 8)
    | ((((unsigned int) i) & 0xff00) << 8)
    | ((((unsigned int) i) & 0xff) << 24));
}

static inline unsigned long long swap64(unsigned long long i)
{
  return (((unsigned long long) ( swap32((unsigned int) i))) << 32) | ((unsigned long long) (swap32((unsigned int) (i >> 32))));
}

static inline unsigned int endianReadU8Little(unsigned char* mem)
{
  return *mem;
}

static inline unsigned int endianReadU8Big(unsigned char* mem)
{
  return *mem;
}

static inline int endianReadS8Little(char* mem)
{
  return *mem;
}

static inline int endianReadS8Big(char* mem)
{
  return *mem;
}

static inline unsigned int endianReadU16Little(unsigned short* mem)
{
#ifdef NEWAGE_BIG_ENDIAN
  return swap16(*mem);
#else
  return *mem;
#endif
}

static inline unsigned int endianReadU16Big(unsigned short* mem)
{
#ifdef NEWAGE_BIG_ENDIAN
  return *mem;
#else
  return swap16(*mem);
#endif
}

static inline int endianReadS16Little(short* mem)
{
#ifdef NEWAGE_BIG_ENDIAN
  return swap16(*mem);
#else
  return *mem;
#endif
}

static inline int endianReadS16Big(short* mem)
{
#ifdef NEWAGE_BIG_ENDIAN
  return *mem;
#else
  return swap16(*mem);
#endif
}

static inline unsigned int endianReadU32Little(unsigned int* mem)
{
#ifdef NEWAGE_BIG_ENDIAN
  return swap32(*mem);
#else
  return *mem;
#endif
}

static inline unsigned int endianReadU32Big(unsigned int* mem)
{
#ifdef NEWAGE_BIG_ENDIAN
  return *mem;
#else
  return swap32(*mem);
#endif
}

static inline int endianReadS32Little(int* mem)
{
#ifdef NEWAGE_BIG_ENDIAN
  return swap32(*mem);
#else
  return *mem;
#endif
}

static inline int endianReadS32Big(int* mem)
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
    unsigned int i;
    float f;
  } x;
  x.i = swap32(*((unsigned int*) mem));
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
    unsigned int i;
    float f;
  } x;
  x.i = swap32(*((unsigned int*) mem));
  return x.f;
#endif
}

static inline double endianReadFloat64Little(double* mem)
{
#ifdef NEWAGE_BIG_ENDIAN
  union
  {
    unsigned long long i;
    double d;
  } x;
  x.i = swap64(*((unsigned long long*) mem));
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
    unsigned long long i;
    double d;
  } x;
  x.i = swap64(*((unsigned long long*) mem));
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


static inline void endianWriteU8Little(unsigned char* mem, unsigned char data)
{
  *mem = data;
}

static inline void endianWriteU8Big(unsigned char* mem, unsigned char data)
{
  *mem = data;
}

static inline void endianWriteS8Little(char* mem, char data)
{
  *mem = data;
}

static inline void endianWriteS8Big(char* mem, char data)
{
  *mem = data;
}

static inline void endianWriteU16Little(unsigned short* mem, unsigned short data)
{
#ifdef NEWAGE_BIG_ENDIAN
  *mem = swap16(data);
#else
  *mem = data;
#endif
}

static inline void endianWriteU16Big(unsigned short* mem, unsigned short data)
{
#ifdef NEWAGE_BIG_ENDIAN
  *mem = data;
#else
  *mem = swap16(data);
#endif
}

static inline void endianWriteS16Little(short* mem, short data)
{
  endianWriteU16Little((unsigned short*) mem, (unsigned short) data);
}

static inline void endianWriteS16Big(short* mem, short data)
{
  endianWriteU16Big((unsigned short*) mem, (unsigned short) data);
}

static inline void endianWriteU32Little(unsigned int* mem, unsigned int data)
{
#ifdef NEWAGE_BIG_ENDIAN
  *((unsigned int*) mem) = swap32(data);
#else
  *((unsigned int*) mem) = data;
#endif
}

static inline void endianWriteU32Big(unsigned int* mem, unsigned int data)
{
#ifdef NEWAGE_BIG_ENDIAN
  *mem = data;
#else
  *mem = swap32(data);
#endif
}

static inline void endianWriteS32Little(int* mem, int data)
{
  endianWriteU32Little((unsigned int*) mem, (unsigned int) data);
}

static inline void endianWriteS32Big(int* mem, int data)
{
  endianWriteU32Big((unsigned int*) mem, (unsigned int) data);
}

static inline void endianWriteFloat32Little(float* mem, float data)
{
#ifdef NEWAGE_BIG_ENDIAN
  union
  {
    unsigned int i;
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
    unsigned int i;
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
    unsigned long long i;
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
    unsigned long long i;
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
