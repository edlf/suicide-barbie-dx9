#include <cmath>

#include <Base/Math/Math.h>
#include <Base/Math/Quat.h>

extern "C" {

float_t QuatDot(Quat *q0, Quat *q1)
{
	return q0->x * q1->x + q0->y * q1->y + q0->z * q1->z + q0->w * q1->w;
}

float_t LengthQuat(Quat *q0)
{
	return sqrtf(q0->x * q0->x + q0->y * q0->y + q0->z * q0->z + q0->w * q0->w);
}

void QuatConjugate(Quat *q, Quat *q0)
{
	q->x = -q0->x;
	q->y = -q0->y;
	q->z = -q0->z;
	q->w =  q0->w;
}

void QuatNegate(Quat *q, Quat *q0)
{
	q->x = -q0->x;
	q->y = -q0->y;
	q->z = -q0->z;
	q->w = -q0->w;
}

void QuatSetFromAxisAngle(Quat *q, Vec3 *v0, float_t angle)
{
	float_t l = v0->x*v0->x + v0->x*v0->x + v0->z*v0->z;
	if( l > (float_t)0.0 )
	{
		angle *= (float_t)0.5;
		q->w = (float_t)cosf(angle);
		l = (float_t)sinf(angle) * (float_t)1.0 / sqrtf(l);
		q->x = v0->x * l;
		q->y = v0->y * l;
		q->z = v0->z * l;
	}
	else
	{
		q->x = (float_t)0;
		q->y = (float_t)0;
		q->z = (float_t)0;
		q->w = (float_t)1;
	}
}

void QuatSetFromEulerAngle(Quat* q, float xangle, float yangle, float zangle)
{
	float_t yaw = yangle;
	float_t pitch = xangle;
	float_t roll = zangle;

	float_t cr, cp, cy, sr, sp, sy, cpcy, spsy;

	// float_t dToR = Math_PI / (float_t)180;

	// calculate trig identities
	cr = cosf ( roll  / (float_t)2 );
	cp = cosf ( pitch / (float_t)2 );
	cy = cosf ( yaw   / (float_t)2 );

	sr = sinf ( roll  / (float_t)2 );
	sp = sinf ( pitch / (float_t)2 );
	sy = sinf ( yaw   / (float_t)2 );
	                            
	cpcy = cp * cy;
	spsy = sp * sy;

	// now create the scalar
	q->w = cr * cpcy + sr * spsy;

	// and the vector part
	q->x = sr * cpcy - cr * spsy;
	q->y = cr * sp * cy + sr * cp * sy;
	q->z = cr * cp * sy - sr * sp * cy;
}

void QuatSLinearCombine(Quat *q, Quat *q0, Quat *q1, float t)
{
	Quat e;
	float_t cosom, t0, t1;
	
	cosom = QuatDot(q0, q1);

	e = *q1;
    if( cosom < (float_t)0.0 )
    {
		cosom = -cosom;
		QuatNegate(&e, &e);
    }
 
    if( cosom < (float_t)0.9999 )
    {
        float_t omega = acosf(cosom);
        float_t rcpSinom = (float_t)1.0 / sinf(omega);
        t0 = sinf((1.0f - (float)t) * omega) * rcpSinom;
        t1 = sinf((float)t * omega) * rcpSinom;
    }
    else
    {
		t0 = (float_t)1.0 - t;
		t1 = t;
    }

	q->x = q0->x * t0 + e.x * t1;
	q->y = q0->y * t0 + e.y * t1;
	q->z = q0->z * t0 + e.z * t1;
	q->w = q0->w * t0 + e.w * t1;
}

void QuatMul(Quat *q, Quat *q0, Quat *q1)
{
	q->x = q0->w*q1->x + q0->x*q1->w + q0->y*q1->z - q0->z*q1->y;
	q->y = q0->w*q1->y + q0->y*q1->w + q0->z*q1->x - q0->x*q1->z;
	q->z = q0->w*q1->z + q0->z*q1->w + q0->x*q1->y - q0->y*q1->x;
	q->w = q0->w*q1->w - q0->x*q1->x - q0->y*q1->y - q0->z*q1->z; 
}

void QuatNormalize(Quat *q, Quat *q0)
{
	float len = LengthQuat(q0);

	q->x = q0->x / len;
	q->y = q0->y / len;
	q->z = q0->z / len;
	q->w = q0->w / len;
}

} // extern C
