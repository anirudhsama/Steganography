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

void hide(char *Hide, char *Cover, char *Password, int num, char *fileSaveName);
void show(char *Stego, char *Password,char *extension, int num);