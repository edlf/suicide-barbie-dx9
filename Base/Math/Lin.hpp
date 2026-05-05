
#ifndef NEWAGE_BASE_MATH_LIN_H
#define NEWAGE_BASE_MATH_LIN_H

#include <Base/Std/Std.h>

typedef struct Vec3Tag
{
  float x, y, z;
} Vec3;

typedef struct Mat33Tag
{
  Vec3 Row[3];
} Mat33;

typedef struct Mat34Tag
{
  Mat33 Rot;
  Vec3 Move;
} Mat34;

struct QuatTag;

float Vec3_dot(Vec3* v0, Vec3* v1);
float Vec3_crossZ(Vec3* v0, Vec3* v1);
float Vec3_length(Vec3* v0);

void Vec3_copy(Vec3* v, Vec3* v0);
void Vec3_neg(Vec3* v, Vec3* v0);
void Vec3_add(Vec3* v, Vec3* v0, Vec3* v1);
void Vec3_sub(Vec3* v, Vec3* v0, Vec3* v1);
void Vec3_scale(Vec3* v, Vec3* v0, float scale);
void Vec3_scaleXY(Vec3* v, Vec3* v0, float xscale, float yscale);
void Vec3_scaleXYZ(Vec3* v, Vec3* v0, float xscale, float yscale, float zscale);
void Vec3_project2D(Vec3* v, Vec3* v0);
void Vec3_cross(Vec3* v, Vec3* v0, Vec3* v1);
void Vec3_crossNormalize(Vec3* v, Vec3* v0, Vec3* v1);
void Vec3_normalize(Vec3* v, Vec3* v0);
void Vec3_linearCombine1(Vec3* v, Vec3* v0, float scale);
void Vec3_linearCombine2(Vec3* v, Vec3* v0, Vec3* v1, float s, float t);
void Vec3_linearCombine3(Vec3* v, Vec3* v0, Vec3* v1, Vec3* v2, float s, float t, float u);
void Vec3_linearCombine4(Vec3* v, Vec3* v0, Vec3* v1, Vec3* v2, Vec3* v3, float s, float t, float u, float w);

void Vec3_setZero(Vec3* v);
void Vec3_setXYZ(Vec3* v, float x, float y, float z);
void Vec3_setMat33MulVec3(Vec3* v, Mat33* m0, Vec3* v0);
void Vec3_setMat34RotVec3(Vec3* v, Mat34* t0, Vec3* v0);
void Vec3_setMat34MulVec3(Vec3* v, Mat34* t0, Vec3* v0);
void Vec3_setMat33CofactorRow(Vec3* v, Vec3* v0, Vec3* v1);


float Mat33_determinant(Mat33* m0);

void Mat33_copy(Mat33* m, Mat33* m0);
void Mat33_mul(Mat33* m, Mat33* m0, Mat33* m1);
void Mat33_invertOrthogonal(Mat33* m, Mat33* m0);
void Mat33_transpose(Mat33* m, Mat33* m0);
void Mat33_invert(Mat33* m, Mat33* m0);

void Mat33_postScale(Mat33* m, Mat33* m0, float scale);
void Mat33_postScaleXYZ(Mat33* m, Mat33* m0, float scalex, float scaley, float scalez);

void Mat33_applyPreScale(Mat33* m, float scale);
void Mat33_applyPostScale(Mat33* m, float scale);
void Mat33_applyPreScaleXYZ(Mat33* m, float scalex, float scaley, float scalez);
void Mat33_applyPostScaleXYZ(Mat33* m, float scalex, float scaley, float scalez);
void Mat33_applyPreRotX(Mat33* m, float angle);
void Mat33_applyPostRotX(Mat33* m, float angle);
void Mat33_applyPreRotY(Mat33* m, float angle);
void Mat33_applyPostRotY(Mat33* m, float angle);
void Mat33_applyPreRotZ(Mat33* m, float angle);
void Mat33_applyPostRotZ(Mat33* m, float angle);
void Mat33_applyPreRotH(Mat33* m, float angle);
void Mat33_applyPostRotH(Mat33* m, float angle);
void Mat33_applyPreRotP(Mat33* m, float angle);
void Mat33_applyPostRotP(Mat33* m, float angle);
void Mat33_applyPreRotB(Mat33* m, float angle);
void Mat33_applyPostRotB(Mat33* m, float angle);
void Mat33_applyPreRotXYZ(Mat33* m, float anglex, float angley, float anglez);
void Mat33_applyPostRotXYZ(Mat33* m, float anglex, float angley, float anglez);
void Mat33_applyPreRotHPB(Mat33* m, float heading, float pitch, float bank);
void Mat33_applyPostRotHPB(Mat33* m, float heading, float pitch, float bank);

void Mat33_setIdentity(Mat33* m);
void Mat33_setScale(Mat33* m, float scale);
void Mat33_setScaleXYZ(Mat33* m, float scalex, float scaley, float scalez);
void Mat33_setRotX(Mat33* m, float xangle);
void Mat33_setRotY(Mat33* m, float yangle);
void Mat33_setRotZ(Mat33* m, float zangle);
void Mat33_setRotXYZ(Mat33* m, float xangle, float yangle, float zangle);
void Mat33_setRotHPB(Mat33* m, float heading, float pitch, float bank);
void Mat33_setRotQuat(Mat33* m, struct QuatTag* q);
//void Mat33_setAxisAngle(Mat33* m, Vec3* axis, float angle);


float Mat34_determinant(Mat34* m0);

void Mat34_copy(Mat34* t, Mat34* t0);
void Mat34_mul(Mat34* t, Mat34* t0, Mat34* t1);
void Mat34_mulMat33(Mat34* t, Mat34* t0, Mat33* t1);
void Mat34_invert(Mat34* t, Mat34* t0);
void Mat34_invertOrthogonal(Mat34* t, Mat34* t0);

void Mat34_applyPreTranslation(Mat34* m, Vec3* v);
void Mat34_applyPostTranslation(Mat34* m, Vec3* v);
void Mat34_applyPreScale(Mat34* m, float scale);
void Mat34_applyPostScale(Mat34* m, float scale);
void Mat34_applyPreScaleXYZ(Mat34* m, float scalex, float scaley, float scalez);
void Mat34_applyPostScaleXYZ(Mat34* m, float scalex, float scaley, float scalez);
void Mat34_applyPreRotX(Mat34* m, float angle);
void Mat34_applyPostRotX(Mat34* m, float angle);
void Mat34_applyPreRotY(Mat34* m, float angle);
void Mat34_applyPostRotY(Mat34* m, float angle);
void Mat34_applyPreRotZ(Mat34* m, float angle);
void Mat34_applyPostRotZ(Mat34* m, float angle);
void Mat34_applyPreRotH(Mat34* m, float angle);
void Mat34_applyPostRotH(Mat34* m, float angle);
void Mat34_applyPreRotP(Mat34* m, float angle);
void Mat34_applyPostRotP(Mat34* m, float angle);
void Mat34_applyPreRotB(Mat34* m, float angle);
void Mat34_applyPostRotB(Mat34* m, float angle);
void Mat34_applyPreRotXYZ(Mat34* m, float anglex, float angley, float anglez);
void Mat34_applyPostRotXYZ(Mat34* m, float anglex, float angley, float anglez);
void Mat34_applyPreRotHPB(Mat34* m, float heading, float pitch, float bank);
void Mat34_applyPostRotHPB(Mat34* m, float heading, float pitch, float bank);

void Mat34_setIdentity(Mat34* t);
void Mat34_setMat33Vec3(Mat34* t, Mat33* rot, Vec3* move);
void Mat34_setRotXYZMove(Mat34* t, float xangle, float yangle, float zangle, Vec3* pos);
void Mat34_setRotHPBMove(Mat34* t, float heading, float pitch, float bank, Vec3* pos);
void Mat34_setMat33MulMat34(Mat34* m, Mat33* m0, Mat34* m1);

static inline void Vec3_readBigEndian(Vec3* binary, Vec3* host)
{
  host->x = endianReadFloat32Big(&binary->x);
  host->y = endianReadFloat32Big(&binary->y);
  host->z = endianReadFloat32Big(&binary->z);
}

static inline void Vec3_writeBigEndian(Vec3* binary, Vec3* host)
{
  endianWriteFloat32Big(&binary->x, host->x);
  endianWriteFloat32Big(&binary->y, host->y);
  endianWriteFloat32Big(&binary->z, host->z);
}

static inline void Mat34_readBigEndian(Mat34* binary, Mat34* host)
{
  Vec3_readBigEndian(&binary->Rot.Row[0], &host->Rot.Row[0]);
  Vec3_readBigEndian(&binary->Rot.Row[1], &host->Rot.Row[1]);
  Vec3_readBigEndian(&binary->Rot.Row[2], &host->Rot.Row[2]);
  Vec3_readBigEndian(&binary->Move, &host->Move);
}

static inline void Mat34_writeBigEndian(Mat34* binary, Mat34* host)
{
  Vec3_writeBigEndian(&binary->Rot.Row[0], &host->Rot.Row[0]);
  Vec3_writeBigEndian(&binary->Rot.Row[1], &host->Rot.Row[1]);
  Vec3_writeBigEndian(&binary->Rot.Row[2], &host->Rot.Row[2]);
  Vec3_writeBigEndian(&binary->Move, &host->Move);
}

#endif
