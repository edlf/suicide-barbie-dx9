
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


extern __asm float_t LengthQuat(Quat *q0);
extern __asm float_t NormQuat(Quat *q0);
extern __asm float_t DotQuat(Quat *q0, Quat *q1);

extern __asm float_t QuatLength(Quat *q0);
extern __asm float_t QuatNorm(Quat *q0);
extern __asm float_t QuatDot(Quat *q0, Quat *q1);

extern __asm void QuatCopy(Quat *q, Quat *q0);
extern __asm void QuatConjugate(Quat *q, Quat *q0);
extern __asm void QuatNegate(Quat *q, Quat *q0);
extern __asm void QuatAdd(Quat *q, Quat *q0, Quat *q1);
extern __asm void QuatSub(Quat *q, Quat *q0, Quat *q1);
extern __asm void QuatScale(Quat *q, Quat *q0, float_t scale);
extern __asm void QuatLinearCombine1(Quat *q, Quat *q0, float scale);
extern __asm void QuatLinearCombine2(Quat *q, Quat *q0, Quat *q1, float s, float t);
extern __asm void QuatLinearCombine3(Quat *q, Quat *q0, Quat *q1, Quat *q2, float s, float t, float u);
extern __asm void QuatLinearCombine4(Quat *q, Quat *q0, Quat *q1, Quat *q2, Quat *q3,
					float s, float t, float u, float v);
extern __asm void QuatSLinearCombine(Quat *q, Quat *q0, Quat *q1, float t);
extern __asm void QuatMul(Quat *q, Quat *q0, Quat *q1);
extern __asm void QuatDiv(Quat *q, Quat *q0, Quat *q1);
extern __asm void QuatInvert(Quat *q, Quat *q0);
extern __asm void QuatNormalize(Quat *q, Quat *q0);

extern __asm void QuatSetZero(Quat *q);
extern __asm void QuatSetOne(Quat *q);
extern __asm void QuatSetIdentity(Quat *q);
extern __asm void QuatSetFromScalars(Quat *q, float_t x, float_t y, float_t z, float_t w);
extern __asm void QuatSetFromVec3Scalar(Quat *q, struct Vec3Tag *v0, float_t s);
extern __asm void QuatSetFromAxisAngle(Quat *q, struct Vec3Tag *v0, float_t angle);
extern __asm void QuatSetFromEulerAngle(Quat* q, float xangle, float yangle, float zangle);


extern Quat Quat_sIdentity;

} // extern C

#endif
