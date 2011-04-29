/*
 *  hideMe.c
 *  Steganography
 *
 *  Created by Faizan Aziz on 22/03/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "hideMe.h"

int fgetce(FILE *fp) {
	int c = fgetc(fp);
	if (c == EOF) {
		printf("Not Enough Space. Terminating...");
		return -1;
	}
	return c;
}

void hide(char *Hide, char *Cover, char *Password, int num, char *fileSaveName){
	int i , j, ch, cc, length, exp, count, index, pass[100];
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
	j = length = 0;
	while (Password[j] != '\0') {
		while (Password[j] > 0) {
			pass[length++] = Password[j]%2;
			Password[j] /= 2;
		}
		j++;
	}
	
	FILE *fwrite;
	fwrite = fopen(fileSaveName, "wb");
	
	//Re write the header
	fputc('B', fwrite);
	fputc('M', fwrite);
	
	for (j = 0; j < 52; j++)
		fputc(fgetc(fpc),fwrite);
	
	exp = index = 0;
	cc = fgetc(fpc);
	cc = cc&(255<<num);
	
	j = strlen(Hide);
	while (Hide[j--] != '.');
	j++;
	j++;
	
	i = count = 0;
	ch = Hide[j++];	
	
	while (1) {		
		if ((ch%2) == 1) {
			cc = cc|((1^pass[index])<<exp);
			
			if (count == 4) {				
				if (index == length - 1)
					index = 0;
				else
					index++;
				
				if (exp == num - 1) {
					fputc(cc, fwrite);
					cc = fgetce(fpc);
					cc = cc&(255<<num);
					exp = 0;
				}
				else
					exp++;
				
				cc = cc|((0^pass[index])<<exp);
				
				count = 0;
			}
			else
				count++;
		}
		else {
			cc = cc|((0^pass[index])<<exp);
			count = 0;
		}
		
		if (index == length - 1)
			index = 0;
		else
			index++;
		
		if (exp == num - 1) {
			fputc(cc, fwrite);
			cc = fgetce(fpc);
			cc = cc&(255<<num);
			exp = 0;
		}
		else
			exp++;
		
		if (i == 7) {
			ch = Hide[j++];
			if (ch == '\0') {
				cc = cc|((0^pass[index])<<exp);
				
				if (index == length - 1)
					index = 0;
				else
					index++;
				
				if (exp == num - 1) {
					fputc(cc, fwrite);
					cc = fgetce(fpc);
					cc = cc&(255<<num);
					exp = 0;
				}
				else
					exp++;
				
				for (j = 0; j < 6 ; j++) {
					cc = cc|((1^pass[index])<<exp);
					
					if (index == length - 1)
						index = 0;
					else 
						index++;
					
					if (exp == num - 1) {
						fputc(cc, fwrite);
						cc = fgetce(fpc);
						cc = cc&(255<<num);
						exp = 0;
					}
					else
						exp++;
				}
				
				cc = cc|((0^pass[index])<<exp);
				
				if (index == length - 1)
					index = 0;
				else 
					index++;
				
				if (exp == num - 1) {
					fputc(cc, fwrite);
					cc = fgetce(fpc);
					cc = cc&(255<<num);
					exp = 0;
				}
				else
					exp++;
				
				break;
			}
			i = 0;
		}
		else {
			i++;
			ch /= 2;
		}
	}	
	
	i = count = 0;
	ch = fgetc(fph);
	
	while (1) {		
		if ((ch%2) == 1) {
			cc = cc|((1^pass[index])<<exp);
			
			if (count == 4) {				
				if (index == length - 1)
					index = 0;
				else
					index++;
				
				if (exp == num - 1) {
					fputc(cc, fwrite);
					cc = fgetce(fpc);
					cc = cc&(255<<num);
					exp = 0;
				}
				else
					exp++;
				
				cc = cc|((0^pass[index])<<exp);
				
				count = 0;
			}
			else
				count++;
		}
		else {
			cc = cc|((0^pass[index])<<exp);
			count = 0;
		}
		
		if (index == length - 1)
			index = 0;
		else
			index++;
		
		if (exp == num - 1) {
			fputc(cc, fwrite);
			cc = fgetce(fpc);
			cc = cc&(255<<num);
			exp = 0;
		}
		else
			exp++;
		
		if (i == 7) {
			ch = fgetc(fph);
			if (ch == EOF) {
				cc = cc|((0^pass[index])<<exp);
				
				if (index == length - 1)
					index = 0;
				else
					index++;
				
				if (exp == num - 1) {
					fputc(cc, fwrite);
					cc = fgetce(fpc);
					cc = cc&(255<<num);
					exp = 0;
				}
				else
					exp++;
				
				for (j = 0; j < 6 ; j++) {
					cc = cc|((1^pass[index])<<exp);
					
					if (index == length - 1)
						index = 0;
					else 
						index++;
					
					if (exp == num - 1) {
						fputc(cc, fwrite);
						cc = fgetce(fpc);
						cc = cc&(255<<num);
						exp = 0;
					}
					else
						exp++;
				}
				cc = cc|((0^pass[index])<<exp);
				fputc(cc, fwrite);			
				break;
			}
			i = 0;
		}
		else {
			i++;
			ch /= 2;
		}
	}
	
	while (1) {
		cc = fgetc(fpc);
		if (cc == EOF)
			break;
		fputc(cc, fwrite);
	}
	
	printf("The stego-image output.bmp has been saved successfully.");
	fclose(fph);
	fclose(fpc);
	fclose(fwrite);
}

void show(char *Stego, char *Password,char *extension, int num) {
	printf("FilePath - %s\n",extension);
	int i, cs, cf, length, pow, exp, count, index, pass[100];
	FILE *fp;
	
	if ((fp = fopen(Stego,"rb")) == NULL)
	{
		printf("Error opening file %s.\n",Stego);
		return ;
	}
	
	if (fgetc(fp)!='B' || fgetc(fp)!='M')
	{
		fclose(fp);
		printf("%s is not a bitmap file.\n",Stego);
		return ;
	}
	
	for (i = 0; i < 52; i++)
		fgetc(fp);
	
	i = length = 0;
	while (Password[i] != '\0') {
		while (Password[i] > 0) {
			pass[length++] = Password[i]%2;
			Password[i] /= 2;
		}
		i++;
	}
	
	i = strlen(extension);
	printf("i = %d\n",i);
	exp = index = 0;
	cs = fgetc(fp);
	
	cf = pow = count = 0;
	while (1) {
		if (((cs&(1<<exp))^(pass[index]<<exp)) != 0) {
			cf += (1<<pow);
			
			if (count == 4) {
				if (exp == (num - 1)) {
					cs = fgetce(fp);
					exp = 0;
				}
				else
					exp++;
				
				if (index == length - 1)
					index = 0;
				else
					index++;
				
				if (((cs&(1<<exp))^(pass[index]<<exp)) != 0) {
					extension[i++] = '\0';
					
					if (exp == (num - 1)) {
						cs = fgetce(fp);
						exp = 0;
					}
					else
						exp++;
					
					if (index == length - 1)
						index = 0;
					else
						index++;
					
					if (exp == (num - 1)) {
						cs = fgetce(fp);
						exp = 0;
					}
					else
						exp++;
					
					if (index == length - 1)
						index = 0;
					else
						index++;					
					break;
				}
				else
					count = 0;
			}
			else
				count++;
		}
		else
			count = 0;
		
		if (index == length - 1)
			index = 0;
		else
			index++;
		
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
	
	FILE *fwrite;
	fwrite = fopen(extension, "wb");
	
	cf = pow = count = 0;
	while (1) {
		if (((cs&(1<<exp))^(pass[index]<<exp)) != 0) {
			cf += (1<<pow);
			
			if (count == 4) {
				if (exp == (num - 1)) {
					cs = fgetce(fp);
					exp = 0;
				}
				else
					exp++;
				
				if (index == length - 1)
					index = 0;
				else
					index++;
				
				if (((cs&(1<<exp))^(pass[index]<<exp)) != 0)
					break;
				else
					count = 0;
			}
			else
				count++;
		}
		else
			count = 0;
		
		if (index == length - 1)
			index = 0;
		else
			index++;
		
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