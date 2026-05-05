
#ifndef NEWAGE_BASE_MATH_QUAT_H
#define NEWAGE_BASE_MATH_QUAT_H

#include <Base/Common/Common.h>

extern "C" {

typedef float float_t;
typedef struct QuatTag
{
  float_t x, y, z, w;
} Quat;

struct Vec3Tag;


extern float_t LengthQuat(Quat *q0);
extern float_t NormQuat(Quat *q0);
extern float_t DotQuat(Quat *q0, Quat *q1);

extern float_t QuatLength(Quat *q0);
extern float_t QuatNorm(Quat *q0);
extern float_t QuatDot(Quat *q0, Quat *q1);

extern void QuatCopy(Quat *q, Quat *q0);
extern void QuatConjugate(Quat *q, Quat *q0);
extern void QuatNegate(Quat *q, Quat *q0);
extern void QuatAdd(Quat *q, Quat *q0, Quat *q1);
extern void QuatSub(Quat *q, Quat *q0, Quat *q1);
extern void QuatScale(Quat *q, Quat *q0, float_t scale);
extern void QuatLinearCombine1(Quat *q, Quat *q0, float scale);
extern void QuatLinearCombine2(Quat *q, Quat *q0, Quat *q1, float s, float t);
extern void QuatLinearCombine3(Quat *q, Quat *q0, Quat *q1, Quat *q2, float s, float t, float u);
extern void QuatLinearCombine4(Quat *q, Quat *q0, Quat *q1, Quat *q2, Quat *q3,
          float s, float t, float u, float v);
extern void QuatSLinearCombine(Quat *q, Quat *q0, Quat *q1, float t);
extern void QuatMul(Quat *q, Quat *q0, Quat *q1);
extern void QuatDiv(Quat *q, Quat *q0, Quat *q1);
extern void QuatInvert(Quat *q, Quat *q0);
extern void QuatNormalize(Quat *q, Quat *q0);

extern void QuatSetZero(Quat *q);
extern void QuatSetOne(Quat *q);
extern void QuatSetIdentity(Quat *q);
extern void QuatSetFromScalars(Quat *q, float_t x, float_t y, float_t z, float_t w);
extern void QuatSetFromVec3Scalar(Quat *q, struct Vec3Tag *v0, float_t s);
extern void QuatSetFromAxisAngle(Quat *q, struct Vec3Tag *v0, float_t angle);
extern void QuatSetFromEulerAngle(Quat* q, float xangle, float yangle, float zangle);


extern Quat Quat_sIdentity;

} // extern C

#endif
