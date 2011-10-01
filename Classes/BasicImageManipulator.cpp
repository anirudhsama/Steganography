/*
 *  hideMe.c
 *  Steganography
 *
 *  Created by Faizan Aziz on 22/03/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "BasicImageManipulator.h"













int passIndex, passLength;

int fgetce(FILE *fp) {
	int c = fgetc(fp);
	if (c == EOF) {
		printf("Not Enough Space. Terminating...");
		return -1;
	}
	return c;
}

void incrementPasswordIndex(){
	if (passIndex == passLength - 1)
		passIndex = 0;
	else
		passIndex++;
}

void writeChar(int *cc, int *temp, int *exp, int num, double *mse, FILE *fwrite, FILE *fpc){
	//This is to write the actual data
	incrementPasswordIndex();
	
	//All the LSB's in the particular byte of image have been used and hence, store the data and move to the next character
	if (*exp == num - 1) {
		//Get MSE
		*mse = (*mse) + (*temp-*cc)*(*temp-*cc);
		
		//Store data
		fputc(*cc, fwrite);
		
		//Next char
		*cc = fgetce(fpc);
		*temp = *cc;
		*cc = *cc&(255<<num);
		*exp = 0;
	}
	else
		(*exp) = (*exp) + 1;
}

int read32bitdata(FILE *fp){
	int32_t actual_size;
	int16_t filesize;
	fread(&filesize, sizeof(short), 1, fp);
	actual_size = filesize;
	fread(&filesize, sizeof(short), 1, fp);
	actual_size += filesize*65536;
	
	return actual_size;
}

void write32bit(FILE *fp, int32_t aData){
	int16_t tempData1;
	int32_t tempData;
	
	//Store this first
	tempData = aData&65535;
	tempData1 = tempData;
	fwrite(&tempData, sizeof(short), 1, fp);
	
	//Store this later
	tempData = aData>>16;
	tempData = tempData&65535;
	tempData1 = tempData;
	fwrite(&tempData, sizeof(short), 1, fp);
}

void hide(char *Hide, char *Cover, char *Password, int num, char *fileSaveName) {
	int i , j, ch, count;
	double mse = 0, psnr = 0;
	FILE *fph, *fpc;
	
	if ((fph = fopen(Hide,"rb")) == NULL)
	{
		printf("Error opening file %s.\n",Hide);
		return;
	}	
	
	if ((fpc = fopen(Cover,"rb")) == NULL)
	{
		printf("Error opening file %s.\n",Cover);
		return ;
	}
	
	if (fgetc(fpc)!='B' || fgetc(fpc)!='M')
	{
		fclose(fpc);
		printf("%s is not a bitmap file.\n",Cover);
		return ;
	}
	
	//Getting the bit format of password
	int pass[100];
	j = passLength = 0 ;
	while (Password[j] != '\0') {
		while (Password[j] > 0) {
			pass[passLength++] = Password[j]%2;
			Password[j] /= 2;
		}
		j++;
	}
	
	FILE *fwrite;
	fwrite = fopen(fileSaveName, "wb");
	
	//Make it BMP
	fputc('B', fwrite);
	fputc('M', fwrite);
	
	//Read and write the header so that we do not corrupt the data, we also read the height and width... Copy the first 16 bytes of the header files
	int height = 0, width = 0;
	for (j = 0; j < 16; j++)
		fputc(fgetc(fpc),fwrite);
	
	height = read32bitdata(fpc);
	write32bit(fwrite, height);
	width = read32bitdata(fpc);
	write32bit(fwrite, width);
	
	//Copy the first 28 bytes of the header files
	for (j = 0; j < 28; j++)
		fputc(fgetc(fpc),fwrite);
	
	int exp = 0, cc;
	
	j = strlen(Hide);
	while (Hide[j--] != '.');
	j++;
	j++;
	
	i = count = passIndex = 0;
	
	//Get character to hide and to put into store in temp to cal MSE and PSNR since it is the original value
	ch = Hide[j++];
	cc = fgetc(fpc);
	int temp = cc;
	cc = cc&(255<<num);
	while (1) {
		if ((ch%2) == 1) {
			//Encrypting the data into the cover image
			cc = cc|((1^pass[passIndex])<<exp);
			
			//The count is used for the bit stuffing reqd in the extn so that it does not int with demark - 111111
			if (count == 4) {	
				writeChar(&cc, &temp, &exp, num, &mse, fwrite, fpc);
				
				// Force put a zero to make 111110
				cc = cc|((0^pass[passIndex])<<exp);
				count = 0;
			}
			else
				count++;
		}
		else {
			cc = cc|((0^pass[passIndex])<<exp);
			count = 0;
		}
		
		writeChar(&cc, &temp, &exp, num, &mse, fwrite, fpc);
		
		//The char has now been, embedded and we will get the next char to hide
		if (i == 7) {
			ch = Hide[j++];
			if (ch == '\0')
				break;
			i = 0;
		}
		else {
			i++;
			ch /= 2;
		}
	}	
	
	int bitSeq[8] = {0,1,1,1,1,1,1,0};
	for (j = 0; j < 8 ; j++) {
		cc = cc|((bitSeq[j]^pass[passIndex])<<exp);
		writeChar(&cc, &temp, &exp, num, &mse, fwrite, fpc);
	}
	
	i = count = 0;
	ch = fgetc(fph);
	
	while (1) {		
		if ((ch%2) == 1) {
			cc = cc|((1^pass[passIndex])<<exp);
			
			if (count == 4) {				
				writeChar(&cc, &temp, &exp, num, &mse, fwrite, fpc);
				cc = cc|((0^pass[passIndex])<<exp);				
				count = 0;
			}
			else
				count++;
		}
		else {
			cc = cc|((0^pass[passIndex])<<exp);
			count = 0;
		}
		
		writeChar(&cc, &temp, &exp, num, &mse, fwrite, fpc);
		if (i == 7) {
			ch = fgetc(fph);
			if (ch == EOF)
				break;
			i = 0;
		}
		else {
			i++;
			ch /= 2;
		}
	}
	
	for (j = 0; j < 8 ; j++) {
		cc = cc|((bitSeq[j]^pass[passIndex])<<exp);
		writeChar(&cc, &temp, &exp, num, &mse, fwrite, fpc);
	}
	
	cc = cc|((0^pass[passIndex])<<exp);
	mse += (temp-cc)*(temp-cc);
	fputc(cc, fwrite);	
	
	//Complete the cover image
	while (1) {
		cc = fgetc(fpc);
		if (cc == EOF)
			break;
		fputc(cc, fwrite);
	}
	
	mse /= (3*width*height);
	psnr = 10*log(65025/mse);
	printf("Height: %d\nWidth: %d\nMSE: %f\nPSNR: %f dB\n", height, width, mse, psnr);
	printf("The stego-image output.bmp has been saved successfully.");
	fclose(fph);
	fclose(fpc);
	fclose(fwrite);
}

void show(char *Stego, char *Password,char *extension, int num) {
	printf("saving in %s",extension);
	int i, cs, cf, pow, exp, count, pass[100];
	FILE *fp;
	
	if ((fp = fopen(Stego,"rb")) == NULL){
		printf("Error opening file %s.\n",Stego);
		return;
	}
	
	if (fgetc(fp)!='B' || fgetc(fp)!='M'){
		fclose(fp);
		printf("%s is not a bitmap file.\n",Stego);
		return;
	}
	
	for (i = 0; i < 52; i++)
		fgetc(fp);
	
	i = passLength = 0;
	while (Password[i] != '\0') {
		while (Password[i] > 0) {
			pass[passLength++] = Password[i]%2;
			Password[i] /= 2;
		}
		i++;
	}
	
	i = strlen(extension);
	exp = passIndex = 0;
	cs = fgetc(fp);
	
	cf = pow = count = 0;
	while (1) {
		if (((cs&(1<<exp))^(pass[passIndex]<<exp)) != 0) {
			cf += (1<<pow);
			
			if (count == 4) {
				if (exp == (num - 1)) {
					cs = fgetce(fp);
					exp = 0;
				}
				else
					exp++;
				
				incrementPasswordIndex();
				
				if (((cs&(1<<exp))^(pass[passIndex]<<exp)) != 0)
					break;
				else
					count = 0;
			}
			else
				count++;
		}
		else
			count = 0;
		
		incrementPasswordIndex();
		
		if (exp == (num - 1)) {
			cs = fgetce(fp);
			exp = 0;
		}
		else
			exp++;
		
		if (pow == 7) {
			extension[i++] = cf;
			pow = cf = 0;
		}
		else
			pow++;
	}	
	
	//Adding the null char
	extension[i++] = '\0';
	if (exp == (num - 1)) {
		cs = fgetce(fp);
		exp = 0;
	}
	else
		exp++;
	
	incrementPasswordIndex();
	
	//Avoiding the bit sequence 0 chaf
	if (exp == (num - 1)) {
		cs = fgetce(fp);
		exp = 0;
	}
	else
		exp++;
	
	incrementPasswordIndex();
	
	FILE *fwrite;
	fwrite = fopen(extension, "wb");
	
	cf = pow = count = 0;
	while (1) {
		if (((cs&(1<<exp))^(pass[passIndex]<<exp)) != 0) {
			cf += (1<<pow);
			
			if (count == 4) {
				if (exp == (num - 1)) {
					cs = fgetce(fp);
					exp = 0;
				}
				else
					exp++;
				
				incrementPasswordIndex();
				
				if (((cs&(1<<exp))^(pass[passIndex]<<exp)) != 0)
					break;
				else
					count = 0;
			}
			else
				count++;
		}
		else
			count = 0;
		
		incrementPasswordIndex();
		
		if (exp == (num - 1)) {
			cs = fgetce(fp);
			exp = 0;
		}
		else
			exp++;
		
		if (pow == 7) {
			fputc(cf, fwrite);
			pow = cf = 0;
		}
		else
			pow++;
	}
	
	printf("Finished extracting.");
	fclose(fp);
	fclose(fwrite);
	return;
}