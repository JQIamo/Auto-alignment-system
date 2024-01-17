#ifndef POSITIOINMOTIONMATRICES_H_
#define POSITIOINMOTIONMATRICES_H_

typedef struct {
	int32_t v[4];
} IntVector4;

typedef struct {
	float v[4];
} FloatVector4;

typedef struct {
	int32_t m[4][4];
} IntMatrix4x4;

typedef struct {
	float m[4][4];
} FloatMatrix4x4;

typedef IntMatrix4x4 PositionMotionMatrix;  // Position to Motor motion

const PositionMotionMatrix POS_MTR_MTX_P_P_P_P = {
    {
        {   21015,    2384,  -59648,   -2786 },
        {   -2478,    8862,    5385,  -31338 },
        {  -34409,   -3170,   67786,    3003 },
        {    1970,  -20767,   -3759,   45933 },
    }
};
    
const PositionMotionMatrix POS_MTR_MTX_P_P_P_N = {
    {
        {   21040,    2118,  -59697,   -2199 },
        {   -2532,    9433,    5488,  -32601 },
        {  -34438,   -2869,   67840,    2337 },
        {    2246,  -23667,   -4284,   52347 },
    }
};
    
const PositionMotionMatrix POS_MTR_MTX_P_P_N_P = {
    {
        {   21209,    2402,  -60031,   -2803 },
        {   -2280,    8880,    4994,  -31356 },
        {  -39468,   -3636,   77752,    3444 },
        {    1509,  -20810,   -2850,   45973 },
    }
};
    
const PositionMotionMatrix POS_MTR_MTX_P_P_N_N = {
    {
        {   21228,    2134,  -60068,   -2211 },
        {   -2321,    9451,    5073,  -32616 },
        {  -39493,   -3290,   77799,    2681 },
        {    1720,  -23711,   -3247,   52383 },
    }
};
    
const PositionMotionMatrix POS_MTR_MTX_P_N_P_P = {
    {
        {   20964,    2564,  -59539,   -3424 },
        {   -2483,    8878,    5395,  -31396 },
        {  -34342,   -3412,   67639,    3859 },
        {    1776,  -20071,   -3336,   43469 },
    }
};
    
const PositionMotionMatrix POS_MTR_MTX_P_N_P_N = {
    {
        {   20986,    2319,  -59580,   -2894 },
        {   -2531,    9430,    5487,  -32591 },
        {  -34366,   -3137,   67684,    3264 },
        {    2019,  -22824,   -3793,   49433 },
    }
};
    
const PositionMotionMatrix POS_MTR_MTX_P_N_N_P = {
    {
        {   21162,    2584,  -59929,   -3446 },
        {   -2284,    8898,    5004,  -31419 },
        {  -39397,   -3914,   77595,    4428 },
        {    1331,  -20115,   -2459,   43519 },
    }
};
    
const PositionMotionMatrix POS_MTR_MTX_P_N_N_N = {
    {
        {   21179,    2337,  -59959,   -2912 },
        {   -2321,    9449,    5072,  -32611 },
        {  -39418,   -3598,   77634,    3743 },
        {    1513,  -22870,   -2796,   49481 },
    }
};
    
const PositionMotionMatrix POS_MTR_MTX_N_P_P_P = {
    {
        {   19594,    2223,  -55616,   -2597 },
        {   -2279,    8884,    4820,  -31365 },
        {  -34401,   -3169,   67763,    3002 },
        {    1607,  -20809,   -2727,   45981 },
    }
};
    
const PositionMotionMatrix POS_MTR_MTX_N_P_P_N = {
    {
        {   19613,    1975,  -55648,   -2049 },
        {   -2323,    9454,    4895,  -32623 },
        {  -34425,   -2867,   67803,    2336 },
        {    1831,  -23709,   -3107,   52390 },
    }
};
    
const PositionMotionMatrix POS_MTR_MTX_N_P_N_P = {
    {
        {   19775,    2239,  -55973,   -2613 },
        {   -2079,    8903,    4426,  -31382 },
        {  -39459,   -3635,   77726,    3443 },
        {    1142,  -20851,   -1812,   46022 },
    }
};
    
const PositionMotionMatrix POS_MTR_MTX_N_P_N_N = {
    {
        {   19789,    1989,  -55995,   -2061 },
        {   -2110,    9472,    4476,  -32638 },
        {  -39478,   -3288,   77756,    2679 },
        {    1301,  -23753,   -2064,   52426 },
    }
};
    
const PositionMotionMatrix POS_MTR_MTX_N_N_P_P = {
    {
        {   19551,    2391,  -55524,   -3193 },
        {   -2284,    8903,    4830,  -31429 },
        {  -34339,   -3412,   67631,    3859 },
        {    1428,  -20113,   -2349,   43526 },
    }
};
    
const PositionMotionMatrix POS_MTR_MTX_N_N_P_N = {
    {
        {   19567,    2163,  -55551,   -2698 },
        {   -2323,    9453,    4894,  -32620 },
        {  -34359,   -3136,   67664,    3263 },
        {    1624,  -22868,   -2671,   49487 },
    }
};
    
const PositionMotionMatrix POS_MTR_MTX_N_N_N_P = {
    {
        {   19735,    2410,  -55888,   -3214 },
        {   -2084,    8923,    4436,  -31451 },
        {  -39394,   -3914,   77587,    4427 },
        {     980,  -20158,   -1467,   43576 },
    }
};
    
const PositionMotionMatrix POS_MTR_MTX_N_N_N_N = {
    {
        {   19747,    2179,  -55905,   -2715 },
        {   -2110,    9473,    4476,  -32640 },
        {  -39409,   -3597,   77610,    3742 },
        {    1114,  -22914,   -1667,   49536 },
    }
};

const PositionMotionMatrix pMMatrices[16] = {
       POS_MTR_MTX_P_P_P_P, POS_MTR_MTX_P_P_P_N,
       POS_MTR_MTX_P_P_N_P, POS_MTR_MTX_P_P_N_N,
       POS_MTR_MTX_P_N_P_P, POS_MTR_MTX_P_N_P_N,
       POS_MTR_MTX_P_N_N_P, POS_MTR_MTX_P_N_N_N,
       POS_MTR_MTX_N_P_P_P, POS_MTR_MTX_N_P_P_N,
       POS_MTR_MTX_N_P_N_P, POS_MTR_MTX_N_P_N_N,
       POS_MTR_MTX_N_N_P_P, POS_MTR_MTX_N_N_P_N,
       POS_MTR_MTX_N_N_N_P, POS_MTR_MTX_N_N_N_N
};

#define pMMatricesSelect(m_vec) pMMatrices[(m_vec.v[3] < 0) + ((m_vec.v[2] < 0) << 1) + ((m_vec.v[1] < 0) << 2) + ((m_vec.v[0] < 0) << 3)]

inline void matrixMul(const IntMatrix4x4 *mat, const IntVector4 *vec, IntVector4 *ret) {
	for (uint8_t r=0; r<4; ++r) {
		ret->v[r] = 0;
		for (uint8_t c=0; c<4; ++c) {
			ret->v[r] += mat->m[r][c] * vec->v[c];
		}
	}
}

inline void matrixMul(const IntMatrix4x4 *mat, const FloatVector4 *vec, IntVector4 *ret) {
	for (uint8_t r=0; r<4; ++r) {
		ret->v[r] = 0;
		for (uint8_t c=0; c<4; ++c) {
			ret->v[r] += mat->m[r][c] * vec->v[c];
		}
	}
}

inline void vectorDelta(const IntVector4 *v1, const IntVector4 *v2, IntVector4 *ret) {
	ret->v[0] = v1->v[0] - v2->v[0];
	ret->v[1] = v1->v[1] - v2->v[1];
	ret->v[2] = v1->v[2] - v2->v[2];
	ret->v[3] = v1->v[3] - v2->v[3];
}

inline void vectorDelta(const FloatVector4 *v1, const FloatVector4 *v2, FloatVector4 *ret) {
	ret->v[0] = v1->v[0] - v2->v[0];
	ret->v[1] = v1->v[1] - v2->v[1];
	ret->v[2] = v1->v[2] - v2->v[2];
	ret->v[3] = v1->v[3] - v2->v[3];
}

#endif
