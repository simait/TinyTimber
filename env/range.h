/*
 * Copyright (c) 2007, Per Lindgren, Johan Eriksson, Johan Nordlander,
 * Simon Aittamaa.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Luleå University of Technology nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
