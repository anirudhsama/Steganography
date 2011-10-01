/*
 *  hideMe.h
 *  Steganography
 *
 *  Created by Faizan Aziz on 22/03/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include <stdio.h>
#include <string.h>
#include "math.h"

#ifndef _INT8_T
#define _INT8_T
typedef	__signed char		int8_t;
#endif
typedef	unsigned char		u_int8_t;
#ifndef _INT16_T
#define _INT16_T
typedef	short			int16_t;
#endif
typedef	unsigned short		u_int16_t;
#ifndef _INT32_T
#define _INT32_T
typedef	int			int32_t;
#endif
typedef	unsigned int		u_int32_t;
#ifndef _INT64_T
#define _INT64_T
typedef	long long		int64_t;
#endif
typedef	unsigned long long	u_int64_t;



void hide(char *Hide, char *Cover, char *pass, int num, char *fileSaveName);
void show(char *Stego, char *Password,char *extension, int num);

void write32bit(FILE *fp, int32_t aData);