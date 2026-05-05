
#ifndef NEWAGE_BASE_MATH_QUAT_H
#define NEWAGE_BASE_MATH_QUAT_H

#include <Base/Common/Common.h>

typedef struct QuatTag
{
  float x, y, z, w;
} Quat;

struct Vec3Tag;

float LengthQuat(Quat *q0);
float NormQuat(Quat *q0);
float DotQuat(Quat *q0, Quat *q1);

float QuatLength(Quat *q0);
float QuatNorm(Quat *q0);
float QuatDot(Quat *q0, Quat *q1);

void QuatCopy(Quat *q, Quat *q0);
void QuatConjugate(Quat *q, Quat *q0);
void QuatNegate(Quat *q, Quat *q0);
void QuatAdd(Quat *q, Quat *q0, Quat *q1);
void QuatSub(Quat *q, Quat *q0, Quat *q1);
void QuatScale(Quat *q, Quat *q0, float scale);
void QuatLinearCombine1(Quat *q, Quat *q0, float scale);
void QuatLinearCombine2(Quat *q, Quat *q0, Quat *q1, float s, float t);
void QuatLinearCombine3(Quat *q, Quat *q0, Quat *q1, Quat *q2, float s, float t, float u);
void QuatLinearCombine4(Quat *q, Quat *q0, Quat *q1, Quat *q2, Quat *q3, float s, float t, float u, float v);
void QuatSLinearCombine(Quat *q, Quat *q0, Quat *q1, float t);
void QuatMul(Quat *q, Quat *q0, Quat *q1);
void QuatDiv(Quat *q, Quat *q0, Quat *q1);
void QuatInvert(Quat *q, Quat *q0);
void QuatNormalize(Quat *q, Quat *q0);

void QuatSetZero(Quat *q);
void QuatSetOne(Quat *q);
void QuatSetIdentity(Quat *q);
void QuatSetFromScalars(Quat *q, float x, float y, float z, float w);
void QuatSetFromVec3Scalar(Quat *q, struct Vec3Tag *v0, float s);
void QuatSetFromAxisAngle(Quat *q, struct Vec3Tag *v0, float angle);
void QuatSetFromEulerAngle(Quat* q, float xangle, float yangle, float zangle);

#endif
