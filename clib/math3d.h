// This is a rewrite version (in C) from Horde3D (utMath.h) , http://www.horde3d.org

#ifndef ejoy3d_math_h
#define ejoy3d_math_h

#include <math.h>
#include <float.h>
#include <stddef.h>

struct vector3 {
	float x,y,z;
};

struct vector4 {
	float x,y,z,w;
};

struct quaternion {
	float x,y,z,w;
};

union matrix44 {
	float c[4][4];
	float x[16];
};

struct plane {
	struct vector3 normal;
	float dist;
};

// vector

static inline float *
vector3_array(struct vector3 *v) {
	return (float *)v;
}

static inline float *
vector4_array(struct vector4 *v) {
	return (float *)v;
}

static inline float
vector3_dot(const struct vector3 *a, const struct vector3 *b) {
	return a->x * b->x + a->y * b->y + a->z * b->z;
}

static inline struct vector3 *
vector3_cross(struct vector3 *v, const struct vector3 *a, const struct vector3 *b) {
	float x = a->y * b->z - a->z * b->y;
	float y = a->z * b->x - a->x * b->z;
	float z = a->x * b->y - a->y * b->x;

	v->x = x;
	v->y = y;
	v->z = z;

	return v;
}

static inline struct vector3 *
vector3_vector(struct vector3 *v, const struct vector3 *p1, const struct vector3 *p2) {
	v->x = p1->x - p2->x;
	v->y = p1->y - p2->y;
	v->z = p1->z - p2->z;

	return v;
}

static inline float
vector3_length(const struct vector3 *v) {
	return sqrtf(v->x * v->x + v->y * v->y + v->z * v->z );
}

static inline struct vector3 *
vector3_normalize(struct vector3 *v) {
	float invLen = 1.0f / vector3_length(v);
	v->x *= invLen;
	v->y *= invLen;
	v->z *= invLen;

	return v;
}

static inline struct vector3 *
vector3_to_rotation(struct vector3 *v, const struct vector3 *r) {
	// Assumes that the unrotated view vector is (0, 0, -1)
	v->x = v->y = v->z = 0;
	if (r->y != 0) {
		v->x = atan2f( r->y, sqrtf( r->x*r->x + r->z*r->z ) );
	}
	if (r->x != 0 || r->z != 0) {
		v->y = atan2f( -r->x, -r->z );
	}

	return v;
}

static inline struct vector3 *
vector3_lerp(struct vector3 *v, const struct vector3 *a, const struct vector3 *b, float f) {
	float x = a->x + (b->x - a->x) * f;
	float y = a->y + (b->y - a->y) * f;
	float z = a->z + (b->z - a->z) * f;

	v->x = x;
	v->y = y;
	v->z = z;

	return v;
}

// quaternion

static inline struct quaternion *
quaternion_mul(struct quaternion *q, const struct quaternion *a, const struct quaternion *b) {
	float x = a->y * b->z - a->z * b->y + b->x * a->w + a->x * b->w;
	float y = a->z * b->x - a->x * b->z + b->y * a->w + a->y * b->w;
	float z = a->x * b->y - a->y * b->x + b->z * a->w + a->z * b->w;
	float w = a->w * b->w - (a->x * b->x + a->y * b->y + a->z * b->z);

	q->x = x;
	q->y = y;
	q->z = z;
	q->w = w;

	return q;
}

static inline struct quaternion *
quaternion_init(struct quaternion *q, float x, float y, float z) {
	struct quaternion roll = { sinf( x * 0.5f ), 0, 0, cosf( x * 0.5f ) };
	struct quaternion pitch = { 0, sinf( y * 0.5f ), 0, cosf( y * 0.5f ) };
	struct quaternion yaw = { 0, 0, sinf( z * 0.5f ), cosf( z * 0.5f ) };

	// Order: y * x * z
	quaternion_mul(q, &pitch, &roll);
	quaternion_mul(q, q, &yaw);

	return q;
}

static inline struct quaternion *
quaternion_slerp(struct quaternion *q, const struct quaternion *a, const struct quaternion *b, float t) {
	float cosTheta = a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w;
	if (cosTheta < 0) {
		cosTheta = -cosTheta; 
		q->x = -b->x; q->y = -b->y;
		q->z = -b->z; q->w = -b->w;
	} else {
		*q = *b;
	}
	float scale0 = 1 - t, scale1 = t;
	if( (1 - cosTheta) > 0.001f ) {
		// use spherical interpolation
		float theta = acosf( cosTheta );
		float sinTheta = sinf( theta );
		scale0 = sinf( (1 - t) * theta ) / sinTheta;
		scale1 = sinf( t * theta ) / sinTheta;
	}

	q->x = a->x * scale0 + q->x * scale1;
	q->y = a->y * scale0 + q->y * scale1;
	q->z = a->z * scale0 + q->z * scale1;
	q->w = a->w * scale0 + q->w * scale1;

	return q;
}

static inline struct quaternion *
quaternion_nslerp(struct quaternion *q, const struct quaternion *a, const struct quaternion *b, float t) {
	// Normalized linear quaternion interpolation
	// Note: NLERP is faster than SLERP and commutative but does not yield constant velocity

	float cosTheta = a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w;
	
	if( cosTheta < 0 ) {
		q->x = a->x + (-b->x - a->x) * t;
		q->y = a->y + (-b->y - a->y) * t;
		q->z = a->z + (-b->z - a->z) * t;
		q->w = a->w + (-b->w - a->w) * t;
	} else {
		q->x = a->x + (b->x - a->x) * t;
		q->y = a->y + (b->y - a->y) * t;
		q->z = a->z + (b->z - a->z) * t;
		q->w = a->w + (b->w - a->w) * t;
	}

	float invLen = 1.0f / sqrtf( q->x * q->x + q->y * q->y + q->z * q->z + q->w * q->w );

	q->x *= invLen;
	q->y *= invLen;
	q->z *= invLen;
	q->w *= invLen;

	return q;
}

static inline struct quaternion *
quaternion_inverted(struct quaternion * q) {
	float len = q->x * q->x + q->y * q->y + q->z * q->z + q->w * q->w;
	if( len > 0 ) {
		float invLen = - 1.0f / len;
		q->x *= invLen;
		q->y *= invLen;
		q->z *= invLen;
		q->w *= invLen;
		q->w = -q->w;
	} else {
		q->x = q->y = q->z = q->w = 0;
	}
	return q;
}

// matrix 4*4

#define C m->c

static inline union matrix44 *
matrix44_identity(union matrix44 * m) {
	C[0][0] = 1; C[1][0] = 0; C[2][0] = 0; C[3][0] = 0;
	C[0][1] = 0; C[1][1] = 1; C[2][1] = 0; C[3][1] = 0;
	C[0][2] = 0; C[1][2] = 0; C[2][2] = 1; C[3][2] = 0;
	C[0][3] = 0; C[1][3] = 0; C[2][3] = 0; C[3][3] = 1;

	return m;
}

static inline union matrix44 *
matrix44_from_quaternion(union matrix44 *m, const struct quaternion *q) {
	// Calculate coefficients
	float x2 = q->x + q->x, y2 = q->y + q->y, z2 = q->z + q->z;
	float xx = q->x * x2,  xy = q->x * y2,  xz = q->x * z2;
	float yy = q->y * y2,  yz = q->y * z2,  zz = q->z * z2;
	float wx = q->w * x2,  wy = q->w * y2,  wz = q->w * z2;

	C[0][0] = 1 - (yy + zz);  C[1][0] = xy - wz;	
	C[2][0] = xz + wy;        C[3][0] = 0;
	C[0][1] = xy + wz;        C[1][1] = 1 - (xx + zz);
	C[2][1] = yz - wx;        C[3][1] = 0;
	C[0][2] = xz - wy;        C[1][2] = yz + wx;
	C[2][2] = 1 - (xx + yy);  C[3][2] = 0;
	C[0][3] = 0;              C[1][3] = 0;
	C[2][3] = 0;              C[3][3] = 1;

	return m;
}

static inline union matrix44 *
matrix44_transmat(union matrix44 *m, float x, float y, float z) {
	matrix44_identity(m);
	C[3][0] = x;
	C[3][1] = y;
	C[3][2] = z;

	return m;
}

static inline union matrix44 *
matrix44_trans(union matrix44 *m, float x, float y, float z) {
	C[3][0] += x;
	C[3][1] += y;
	C[3][2] += z;

	return m;
}

static inline union matrix44 *
matrix44_scalemat(union matrix44 *m, float x, float y, float z) {
	matrix44_identity(m);
	C[0][0] = x;
	C[1][1] = y;
	C[2][2] = z;

	return m;
}

static inline union matrix44 *
matrix44_scale(union matrix44 *m, float x, float y, float z) {
	C[0][0] *= x;
	C[0][1] *= y;
	C[0][2] *= z;

	C[1][0] *= x;
	C[1][1] *= y;
	C[1][2] *= z;

	C[2][0] *= x;
	C[2][1] *= y;
	C[2][2] *= z;

	return m;
}

static inline union matrix44 *
matrix44_rotmat(union matrix44 *m, float x, float y, float z) {
	// Rotation order: YXZ [* Vector]
	struct quaternion q;
	quaternion_init(&q, x, y, z);

	return matrix44_from_quaternion(m, &q);
}

static inline union matrix44 *
matrix44_rot_axis(union matrix44 *m, const struct vector3 *axis, float angle) {
	float t = sinf( angle * 0.5f);
	float x = axis->x * t;
	float y = axis->y * t;
	float z = axis->z * t;
	struct quaternion q = {	x,y,z, cosf( angle * 0.5f ) };

	return matrix44_from_quaternion(m, &q);
}

static inline union matrix44 *
matrix44_perspective(union matrix44 *m, float l, float r, float b, float t, float n, float f) {
	matrix44_identity(m);
	float *mx = m->x;

	mx[0] = 2 * n / (r - l);
	mx[5] = 2 * n / (t - b);
	mx[8] = (r + l) / (r - l);
	mx[9] = (t + b) / (t - b);
	mx[10] = -(f + n) / (f - n);
	mx[11] = -1;
	mx[14] = -2 * f * n / (f - n);
	mx[15] = 0;

	return m;
}

static inline union matrix44 *
matrix44_ortho(union matrix44 *m, float l, float r, float b, float t, float n, float f ) {
	matrix44_identity(m);
	float *mx = m->x;

	mx[0] = 2 / (r - l);
	mx[5] = 2 / (t - b);
	mx[10] = -2 / (f - n);
	mx[12] = -(r + l) / (r - l);
	mx[13] = -(t + b) / (t - b);
	mx[14] = -(f + n) / (f - n);

	return m;
}

static inline union matrix44 *
matrix44_fastmul43(union matrix44 *m, const union matrix44 *m1, const union matrix44 *m2) {
	// Note: m may not be the same as m1 or m2

	const float *m1x = m1->x;
	const float *m2x = m2->x;
	float *mx = m->x;
		
	mx[0] = m1x[0] * m2x[0] + m1x[4] * m2x[1] + m1x[8] * m2x[2];
	mx[1] = m1x[1] * m2x[0] + m1x[5] * m2x[1] + m1x[9] * m2x[2];
	mx[2] = m1x[2] * m2x[0] + m1x[6] * m2x[1] + m1x[10] * m2x[2];
	mx[3] = 0.0f;

	mx[4] = m1x[0] * m2x[4] + m1x[4] * m2x[5] + m1x[8] * m2x[6];
	mx[5] = m1x[1] * m2x[4] + m1x[5] * m2x[5] + m1x[9] * m2x[6];
	mx[6] = m1x[2] * m2x[4] + m1x[6] * m2x[5] + m1x[10] * m2x[6];
	mx[7] = 0.0f;

	mx[8] = m1x[0] * m2x[8] + m1x[4] * m2x[9] + m1x[8] * m2x[10];
	mx[9] = m1x[1] * m2x[8] + m1x[5] * m2x[9] + m1x[9] * m2x[10];
	mx[10] = m1x[2] * m2x[8] + m1x[6] * m2x[9] + m1x[10] * m2x[10];
	mx[11] = 0.0f;

	mx[12] = m1x[0] * m2x[12] + m1x[4] * m2x[13] + m1x[8] * m2x[14] + m1x[12] * m2x[15];
	mx[13] = m1x[1] * m2x[12] + m1x[5] * m2x[13] + m1x[9] * m2x[14] + m1x[13] * m2x[15];
	mx[14] = m1x[2] * m2x[12] + m1x[6] * m2x[13] + m1x[10] * m2x[14] + m1x[14] * m2x[15];
	mx[15] = 1.0f;

	return m;
}

static inline union matrix44 *
matrix44_mul(union matrix44 *m, const union matrix44 *m1, const union matrix44 *m2) {
	union matrix44 mf;
	const float *m1x = m1->x;
	const float *m2x = m2->x;

	mf.x[0] = m1x[0] * m2x[0] + m1x[4] * m2x[1] + m1x[8] * m2x[2] + m1x[12] * m2x[3];
	mf.x[1] = m1x[1] * m2x[0] + m1x[5] * m2x[1] + m1x[9] * m2x[2] + m1x[13] * m2x[3];
	mf.x[2] = m1x[2] * m2x[0] + m1x[6] * m2x[1] + m1x[10] * m2x[2] + m1x[14] * m2x[3];
	mf.x[3] = m1x[3] * m2x[0] + m1x[7] * m2x[1] + m1x[11] * m2x[2] + m1x[15] * m2x[3];

	mf.x[4] = m1x[0] * m2x[4] + m1x[4] * m2x[5] + m1x[8] * m2x[6] + m1x[12] * m2x[7];
	mf.x[5] = m1x[1] * m2x[4] + m1x[5] * m2x[5] + m1x[9] * m2x[6] + m1x[13] * m2x[7];
	mf.x[6] = m1x[2] * m2x[4] + m1x[6] * m2x[5] + m1x[10] * m2x[6] + m1x[14] * m2x[7];
	mf.x[7] = m1x[3] * m2x[4] + m1x[7] * m2x[5] + m1x[11] * m2x[6] + m1x[15] * m2x[7];

	mf.x[8] = m1x[0] * m2x[8] + m1x[4] * m2x[9] + m1x[8] * m2x[10] + m1x[12] * m2x[11];
	mf.x[9] = m1x[1] * m2x[8] + m1x[5] * m2x[9] + m1x[9] * m2x[10] + m1x[13] * m2x[11];
	mf.x[10] = m1x[2] * m2x[8] + m1x[6] * m2x[9] + m1x[10] * m2x[10] + m1x[14] * m2x[11];
	mf.x[11] = m1x[3] * m2x[8] + m1x[7] * m2x[9] + m1x[11] * m2x[10] + m1x[15] * m2x[11];

	mf.x[12] = m1x[0] * m2x[12] + m1x[4] * m2x[13] + m1x[8] * m2x[14] + m1x[12] * m2x[15];
	mf.x[13] = m1x[1] * m2x[12] + m1x[5] * m2x[13] + m1x[9] * m2x[14] + m1x[13] * m2x[15];
	mf.x[14] = m1x[2] * m2x[12] + m1x[6] * m2x[13] + m1x[10] * m2x[14] + m1x[14] * m2x[15];
	mf.x[15] = m1x[3] * m2x[12] + m1x[7] * m2x[13] + m1x[11] * m2x[14] + m1x[15] * m2x[15];

	*m = mf;

	return m;
}

static inline union matrix44 *
matrix44_rot(union matrix44 *m, float x, float y, float z) {
	// Rotation order: YXZ [* Vector]
	struct quaternion q;
	quaternion_init(&q, x, y, z);

	union matrix44 tmp;
	matrix44_from_quaternion(&tmp, &q);
	return matrix44_mul(m, &tmp, m);
}

// vector * matrix

static inline struct vector3 *
vector3_mul(struct vector3 *v, const union matrix44 *m) {
	float x = v->x * C[0][0] + v->y * C[1][0] + v->z * C[2][0] + C[3][0];
	float y = v->x * C[0][1] + v->y * C[1][1] + v->z * C[2][1] + C[3][1];
	float z = v->x * C[0][2] + v->y * C[1][2] + v->z * C[2][2] + C[3][2];

	v->x = x;
	v->y = y;
	v->z = z;

	return v;
}

static inline struct vector4 *
vector4_mul(struct vector4 *v, const union matrix44 *m) {
	float x = v->x * C[0][0] + v->y * C[1][0] + v->z * C[2][0] + v->w * C[3][0];
	float y = v->x * C[0][1] + v->y * C[1][1] + v->z * C[2][1] + v->w * C[3][1];
	float z = v->x * C[0][2] + v->y * C[1][2] + v->z * C[2][2] + v->w * C[3][2];
	float w = v->x * C[0][3] + v->y * C[1][3] + v->z * C[2][3] + v->w * C[3][3];

	v->x = x;
	v->y = y;
	v->z = z;
	v->w = w;
	return v;
}

static inline struct vector3 *
vector3_mul33(struct vector3 *v, const union matrix44 *m) {
	float x = v->x * C[0][0] + v->y * C[1][0] + v->z * C[2][0];
	float y = v->x * C[0][1] + v->y * C[1][1] + v->z * C[2][1];
	float z = v->x * C[0][2] + v->y * C[1][2] + v->z * C[2][2];

	v->x = x;
	v->y = y;
	v->z = z;

	return v;
}

static inline union matrix44 *
matrix44_transposed(union matrix44 *m) {
	int x,y;
	for (y = 0; y < 4; ++y ) {
		for(x = y + 1; x < 4; ++x ) {
			float tmp = C[x][y];
			C[x][y] = C[y][x];
			C[y][x] = tmp;
		}
	}

	return m;
}

static inline float 
matrix44_determinant(const union matrix44 *m) {
	return 
		C[0][3]*C[1][2]*C[2][1]*C[3][0] - C[0][2]*C[1][3]*C[2][1]*C[3][0] - C[0][3]*C[1][1]*C[2][2]*C[3][0] + C[0][1]*C[1][3]*C[2][2]*C[3][0] +
		C[0][2]*C[1][1]*C[2][3]*C[3][0] - C[0][1]*C[1][2]*C[2][3]*C[3][0] - C[0][3]*C[1][2]*C[2][0]*C[3][1] + C[0][2]*C[1][3]*C[2][0]*C[3][1] +
		C[0][3]*C[1][0]*C[2][2]*C[3][1] - C[0][0]*C[1][3]*C[2][2]*C[3][1] - C[0][2]*C[1][0]*C[2][3]*C[3][1] + C[0][0]*C[1][2]*C[2][3]*C[3][1] +
		C[0][3]*C[1][1]*C[2][0]*C[3][2] - C[0][1]*C[1][3]*C[2][0]*C[3][2] - C[0][3]*C[1][0]*C[2][1]*C[3][2] + C[0][0]*C[1][3]*C[2][1]*C[3][2] +
		C[0][1]*C[1][0]*C[2][3]*C[3][2] - C[0][0]*C[1][1]*C[2][3]*C[3][2] - C[0][2]*C[1][1]*C[2][0]*C[3][3] + C[0][1]*C[1][2]*C[2][0]*C[3][3] +
		C[0][2]*C[1][0]*C[2][1]*C[3][3] - C[0][0]*C[1][2]*C[2][1]*C[3][3] - C[0][1]*C[1][0]*C[2][2]*C[3][3] + C[0][0]*C[1][1]*C[2][2]*C[3][3];
}

static inline union matrix44 *
matrix44_inverted(union matrix44 *dst, const union matrix44 *m) {
	float d = matrix44_determinant(m);
	if( d == 0 ) {
		*dst = *m;
		return dst;
	}
	d = 1.0f / d;
		
	dst->c[0][0] = d * (C[1][2]*C[2][3]*C[3][1] - C[1][3]*C[2][2]*C[3][1] + C[1][3]*C[2][1]*C[3][2] - C[1][1]*C[2][3]*C[3][2] - C[1][2]*C[2][1]*C[3][3] + C[1][1]*C[2][2]*C[3][3]);
	dst->c[0][1] = d * (C[0][3]*C[2][2]*C[3][1] - C[0][2]*C[2][3]*C[3][1] - C[0][3]*C[2][1]*C[3][2] + C[0][1]*C[2][3]*C[3][2] + C[0][2]*C[2][1]*C[3][3] - C[0][1]*C[2][2]*C[3][3]);
	dst->c[0][2] = d * (C[0][2]*C[1][3]*C[3][1] - C[0][3]*C[1][2]*C[3][1] + C[0][3]*C[1][1]*C[3][2] - C[0][1]*C[1][3]*C[3][2] - C[0][2]*C[1][1]*C[3][3] + C[0][1]*C[1][2]*C[3][3]);
	dst->c[0][3] = d * (C[0][3]*C[1][2]*C[2][1] - C[0][2]*C[1][3]*C[2][1] - C[0][3]*C[1][1]*C[2][2] + C[0][1]*C[1][3]*C[2][2] + C[0][2]*C[1][1]*C[2][3] - C[0][1]*C[1][2]*C[2][3]);
	dst->c[1][0] = d * (C[1][3]*C[2][2]*C[3][0] - C[1][2]*C[2][3]*C[3][0] - C[1][3]*C[2][0]*C[3][2] + C[1][0]*C[2][3]*C[3][2] + C[1][2]*C[2][0]*C[3][3] - C[1][0]*C[2][2]*C[3][3]);
	dst->c[1][1] = d * (C[0][2]*C[2][3]*C[3][0] - C[0][3]*C[2][2]*C[3][0] + C[0][3]*C[2][0]*C[3][2] - C[0][0]*C[2][3]*C[3][2] - C[0][2]*C[2][0]*C[3][3] + C[0][0]*C[2][2]*C[3][3]);
	dst->c[1][2] = d * (C[0][3]*C[1][2]*C[3][0] - C[0][2]*C[1][3]*C[3][0] - C[0][3]*C[1][0]*C[3][2] + C[0][0]*C[1][3]*C[3][2] + C[0][2]*C[1][0]*C[3][3] - C[0][0]*C[1][2]*C[3][3]);
	dst->c[1][3] = d * (C[0][2]*C[1][3]*C[2][0] - C[0][3]*C[1][2]*C[2][0] + C[0][3]*C[1][0]*C[2][2] - C[0][0]*C[1][3]*C[2][2] - C[0][2]*C[1][0]*C[2][3] + C[0][0]*C[1][2]*C[2][3]);
	dst->c[2][0] = d * (C[1][1]*C[2][3]*C[3][0] - C[1][3]*C[2][1]*C[3][0] + C[1][3]*C[2][0]*C[3][1] - C[1][0]*C[2][3]*C[3][1] - C[1][1]*C[2][0]*C[3][3] + C[1][0]*C[2][1]*C[3][3]);
	dst->c[2][1] = d * (C[0][3]*C[2][1]*C[3][0] - C[0][1]*C[2][3]*C[3][0] - C[0][3]*C[2][0]*C[3][1] + C[0][0]*C[2][3]*C[3][1] + C[0][1]*C[2][0]*C[3][3] - C[0][0]*C[2][1]*C[3][3]);
	dst->c[2][2] = d * (C[0][1]*C[1][3]*C[3][0] - C[0][3]*C[1][1]*C[3][0] + C[0][3]*C[1][0]*C[3][1] - C[0][0]*C[1][3]*C[3][1] - C[0][1]*C[1][0]*C[3][3] + C[0][0]*C[1][1]*C[3][3]);
	dst->c[2][3] = d * (C[0][3]*C[1][1]*C[2][0] - C[0][1]*C[1][3]*C[2][0] - C[0][3]*C[1][0]*C[2][1] + C[0][0]*C[1][3]*C[2][1] + C[0][1]*C[1][0]*C[2][3] - C[0][0]*C[1][1]*C[2][3]);
	dst->c[3][0] = d * (C[1][2]*C[2][1]*C[3][0] - C[1][1]*C[2][2]*C[3][0] - C[1][2]*C[2][0]*C[3][1] + C[1][0]*C[2][2]*C[3][1] + C[1][1]*C[2][0]*C[3][2] - C[1][0]*C[2][1]*C[3][2]);
	dst->c[3][1] = d * (C[0][1]*C[2][2]*C[3][0] - C[0][2]*C[2][1]*C[3][0] + C[0][2]*C[2][0]*C[3][1] - C[0][0]*C[2][2]*C[3][1] - C[0][1]*C[2][0]*C[3][2] + C[0][0]*C[2][1]*C[3][2]);
	dst->c[3][2] = d * (C[0][2]*C[1][1]*C[3][0] - C[0][1]*C[1][2]*C[3][0] - C[0][2]*C[1][0]*C[3][1] + C[0][0]*C[1][2]*C[3][1] + C[0][1]*C[1][0]*C[3][2] - C[0][0]*C[1][1]*C[3][2]);
	dst->c[3][3] = d * (C[0][1]*C[1][2]*C[2][0] - C[0][2]*C[1][1]*C[2][0] + C[0][2]*C[1][0]*C[2][1] - C[0][0]*C[1][2]*C[2][1] - C[0][1]*C[1][0]*C[2][2] + C[0][0]*C[1][1]*C[2][2]);
		
	return dst;
}

static inline struct vector3 *
matrix44_gettrans(const union matrix44 *m, struct vector3 *trans) {
	// Getting translation is trivial
	trans->x = C[3][0];
	trans->y = C[3][1];
	trans->z = C[3][2];

	return trans;
}

static inline struct vector3 *
matrix44_getscale(const union matrix44 *m, struct vector3 *scale) {
	// Scale is length of columns
	scale->x = sqrtf( C[0][0] * C[0][0] + C[0][1] * C[0][1] + C[0][2] * C[0][2] );
	scale->y = sqrtf( C[1][0] * C[1][0] + C[1][1] * C[1][1] + C[1][2] * C[1][2] );
	scale->z = sqrtf( C[2][0] * C[2][0] + C[2][1] * C[2][1] + C[2][2] * C[2][2] );

	return scale;
}

static inline void
matrix44_decompose(const union matrix44 *m, struct vector3 *trans, struct vector3 *rot, struct vector3 *scale ) {
	matrix44_gettrans(m, trans);
	matrix44_getscale(m, scale);

	if( scale->x == 0 || scale->y == 0 || scale->z == 0 ) {
		rot->x = 0;
		rot->y = 0;
		rot->z = 0;
		return;
	}

	// Detect negative scale with determinant and flip one arbitrary axis
	if( matrix44_determinant(m) < 0) 
		scale->x = -scale->x;

	// Combined rotation matrix YXZ
	//
	// Cos[y]*Cos[z]+Sin[x]*Sin[y]*Sin[z]   Cos[z]*Sin[x]*Sin[y]-Cos[y]*Sin[z]  Cos[x]*Sin[y]	
	// Cos[x]*Sin[z]                        Cos[x]*Cos[z]                       -Sin[x]
	// -Cos[z]*Sin[y]+Cos[y]*Sin[x]*Sin[z]  Cos[y]*Cos[z]*Sin[x]+Sin[y]*Sin[z]  Cos[x]*Cos[y]

	rot->x = asinf( -C[2][1] / scale->z );
		
	// Special case: Cos[x] == 0 (when Sin[x] is +/-1)
	float f = fabsf( C[2][1] / scale->z );

	if( f > 0.999f && f < 1.001f ) {
		// Pin arbitrarily one of y or z to zero
		// Mathematical equivalent of gimbal lock
		rot->y = 0;
			
		// Now: Cos[x] = 0, Sin[x] = +/-1, Cos[y] = 1, Sin[y] = 0
		// => m[0][0] = Cos[z] and m[1][0] = Sin[z]
		rot->z = atan2f( -C[1][0] / scale->y, C[0][0] / scale->x );
	} else {
		// Standard case
		rot->y = atan2f( C[2][0] / scale->z, C[2][2] / scale->z );
		rot->z = atan2f( C[0][1] / scale->x, C[1][1] / scale->y );
	}
}

static inline float *
matrix44_to33(const union matrix44 *m, float m33[9]) {
	m33[0] = C[0][0]; m33[1] = C[0][1]; m33[2] = C[0][2];
	m33[3] = C[1][0]; m33[4] = C[1][1]; m33[5] = C[1][2];
	m33[6] = C[2][0]; m33[7] = C[2][1]; m33[8] = C[2][2];

	return m33;
}

#undef C

// plane

static inline struct plane *
plane_init(struct plane *p, const struct vector3 *normal, float d ) {
	p->normal = *normal;
	// normalize
	float invLen = 1.0f / vector3_length(normal);
	p->normal.x *= invLen;
	p->normal.y *= invLen;
	p->normal.z *= invLen;
	p->dist = d * invLen;

	return p;
}

static inline struct plane *
plane_init_dot3(struct plane *p, const struct vector3 *v0, const struct vector3 *v1, const struct vector3 *v2) {
	struct vector3 a,b;
	vector3_vector(&a, v1, v0);
	vector3_vector(&b, v2, v0);

	vector3_cross(&p->normal, &a, &b);
	vector3_normalize(&p->normal);
	p->dist = -vector3_dot(&p->normal, v0);

	return p;
}

static inline float
plane_dist(const struct plane *p, const struct vector3 *v) {
	float d = vector3_dot(&p->normal, v);
	return d + p->dist;
}

// Intersection

static inline struct vector3 *
intersection_raytriangle(const struct vector3 *rayOrig, const struct vector3 *rayDir,
	const struct vector3 *vert0, const struct vector3 *vert1, const struct vector3 *vert2,
	struct vector3 *intsPoint) {
	// Idea: Tomas Moeller and Ben Trumbore
	// in Fast, Minimum Storage Ray/Triangle Intersection 
	
	// Find vectors for two edges sharing vert0
	struct vector3 edge1, edge2;
	vector3_vector(&edge1, vert1, vert0);
	vector3_vector(&edge2, vert2, vert0);

	// Begin calculating determinant - also used to calculate U parameter
	struct vector3 pvec;
	vector3_cross(&pvec, rayDir, &edge2);

	// If determinant is near zero, ray lies in plane of triangle
	float det = vector3_dot(&edge1, &pvec);

	// *** Culling branch ***
	/*if( det < FLT_EPSILON )
		return NULL;

	// Calculate distance from vert0 to ray origin
	struct vector3 tvec;
	vector3_vector(&tvec, rayOrig, &vert0);

	// Calculate U parameter and test bounds
	float u = vector3_dot(&tvec, &pvec);
	if (u < 0 || u > det ) 
		return NULL;

	// Prepare to test V parameter
	struct vector3 qvec;
	vector3_cross(&qvec, &tvec, &edge1);

	// Calculate V parameter and test bounds
	float v = vector3_dot(rayDir, &qvec);
	if (v < 0 || u + v > det ) 
		return NULL;

	// Calculate t, scale parameters, ray intersects triangle
	float t = vector3_dot(&edge2, &qvec ) / det;*/

	// *** Non-culling branch ***
	if( det > -FLT_EPSILON && det < FLT_EPSILON )
		return 0;
	float inv_det = 1.0f / det;

	// Calculate distance from vert0 to ray origin
	struct vector3 tvec;
	vector3_vector(&tvec, rayOrig, vert0);

	// Calculate U parameter and test bounds
	float u = vector3_dot(&tvec, &pvec ) * inv_det;
	if( u < 0.0f || u > 1.0f ) 
		return 0;

	// Prepare to test V parameter
	struct vector3 qvec;
	vector3_cross(&qvec, &tvec, &edge1);

	// Calculate V parameter and test bounds
	float v = vector3_dot(rayDir, &qvec ) * inv_det;
	if( v < 0.0f || u + v > 1.0f ) 
		return 0;

	// Calculate t, ray intersects triangle
	float t = vector3_dot(&edge2, &qvec) * inv_det;

	// Calculate intersection point and test ray length and direction
	intsPoint->x = rayOrig->x + rayDir->x * t;
	intsPoint->y = rayOrig->y + rayDir->y * t;
	intsPoint->z = rayOrig->z + rayDir->z * t;

	struct vector3 vec;
	vector3_vector(&vec, intsPoint, rayOrig);
	if( vector3_dot(&vec, rayDir) < 0 || vector3_length(&vec) > vector3_length(rayDir)) 
		return NULL;

	return intsPoint;
}

static inline float
minf(float a, float b) {
	return a < b ? a : b;
}

static inline float
maxf(float a, float b) {
	return a > b ? a : b;
}

static inline int
intersection_rayAABB(const struct vector3 *rayOrig, const struct vector3 *rayDir, 
	const struct vector3 *mins, const struct vector3 *maxs ) {
	// SLAB based optimized ray/AABB intersection routine
	// Idea taken from http://ompf.org/ray/
	
	float l1 = (mins->x - rayOrig->x) / rayDir->x;
	float l2 = (maxs->x - rayOrig->x) / rayDir->x;
	float lmin = minf( l1, l2 );
	float lmax = maxf( l1, l2 );

	l1 = (mins->y - rayOrig->y) / rayDir->y;
	l2 = (maxs->y - rayOrig->y) / rayDir->y;
	lmin = maxf( minf( l1, l2 ), lmin );
	lmax = minf( maxf( l1, l2 ), lmax );
		
	l1 = (mins->z - rayOrig->z) / rayDir->z;
	l2 = (maxs->z - rayOrig->z) / rayDir->z;
	lmin = maxf( minf( l1, l2 ), lmin );
	lmax = minf( maxf( l1, l2 ), lmax );

	if( (lmax >= 0.0f) & (lmax >= lmin) ) {
		// Consider length
		const struct vector3 rayDest = { rayOrig->x + rayDir->x , rayOrig->y + rayDir->y , rayOrig->z + rayDir->z };
		const struct vector3 rayMins = { minf( rayDest.x, rayOrig->x), minf( rayDest.y, rayOrig->y ), minf( rayDest.z, rayOrig->z ) };
		const struct vector3 rayMaxs = { maxf( rayDest.x, rayOrig->x), maxf( rayDest.y, rayOrig->y ), maxf( rayDest.z, rayOrig->z ) };
		return 
			(rayMins.x < maxs->x) && (rayMaxs.x > mins->x) &&
			(rayMins.y < maxs->y) && (rayMaxs.y > mins->y) &&
			(rayMins.z < maxs->z) && (rayMaxs.z > mins->z);
	} else {
		return 0;
	}
}

static inline float 
vector3_distAABB(const struct vector3 *pos, const struct vector3 *mins, const struct vector3 *maxs ) {
	struct vector3 center;
	struct vector3 extent;
	center.x = (mins->x + maxs->x) * 0.5f;
	center.y = (mins->y + maxs->y) * 0.5f;
	center.z = (mins->z + maxs->z) * 0.5f;

	extent.x = (maxs->x - mins->x) * 0.5f;
	extent.y = (maxs->y - mins->y) * 0.5f;
	extent.z = (maxs->z - mins->z) * 0.5f;
	
	struct vector3 nearestVec;
	nearestVec.x = maxf( 0, fabsf( pos->x - center.x ) - extent.x );
	nearestVec.y = maxf( 0, fabsf( pos->y - center.y ) - extent.y );
	nearestVec.z = maxf( 0, fabsf( pos->z - center.z ) - extent.z );
	
	return vector3_length(&nearestVec);
}

#endif

