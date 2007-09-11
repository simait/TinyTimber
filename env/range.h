#ifndef RANGE_H_
#define RANGE_H_

#define TYPE_LE(type, v0, v1) \
	(\
	 ((signed type)(v0) <= (signed type)(v1)) ||\
	 (\
	  ((signed type)(v0) <  0) &&\
	  ((signed type)(v1) >= 0)\
	 )\
	)

#define TYPE_LT(type, v0, v1) \
	(\
	 ((signed type)(v0) < (signed type)(v1)) ||\
	 (\
	  ((signed type)(v0) <  0) &&\
	  ((signed type)(v1) >= 0)\
	 )\
	)

#define TYPE_GT(type, v0, v1) \
	(\
	 ((signed type)(v0) > (signed type)(v1)) &&\
	 !(\
		 ((signed type)(v0) <  0) &&\
		 ((signed type)(v1) >= 0)\
	  )\
	)

#define TYPE_GE(type, v0, v1) \
	(\
	  ((signed type)(v0) >= (signed type)(v1)) &&\
	  !(\
		  ((signed type)(v0) <  0) &&\
		  ((signed type)(v1) >= 0)\
	   )\
	)

#define UCHAR_LT(i0, i1) TYPE_LT(char, i0, i1)
#define UCHAR_LE(i0, i1) TYPE_LE(char, i0, i1)
#define UCHAR_GT(i0, i1) TYPE_GT(char, i0, i1)
#define UCHAR_GE(i0, i1) TYPE_GE(char, i0, i1)

#define USHORT_LT(i0, i1) TYPE_LT(short, i0, i1)
#define USHORT_LE(i0, i1) TYPE_LE(short, i0, i1)
#define USHORT_GT(i0, i1) TYPE_GT(short, i0, i1)
#define USHORT_GE(i0, i1) TYPE_GE(short, i0, i1)

#define UINT_LT(i0, i1) TYPE_LT(int, i0, i1)
#define UINT_LE(i0, i1) TYPE_LE(int, i0, i1)
#define UINT_GT(i0, i1) TYPE_GT(int, i0, i1)
#define UINT_GE(i0, i1) TYPE_GE(int, i0, i1)

#define ULONG_LT(i0, i1) TYPE_LT(long, i0, i1)
#define ULONG_LE(i0, i1) TYPE_LE(long, i0, i1)
#define ULONG_GT(i0, i1) TYPE_GT(long, i0, i1)
#define ULONG_GE(i0, i1) TYPE_GE(long, i0, i1)

#define ULONGLONG_LT(i0, i1) TYPE_LT(long long, i0, i1)
#define ULONGLONG_LE(i0, i1) TYPE_LE(long long, i0, i1)
#define ULONGLONG_GT(i0, i1) TYPE_GT(long long, i0, i1)
#define ULONGLONG_GE(i0, i1) TYPE_GE(long long, i0, i1)

#endif
