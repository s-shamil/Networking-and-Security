#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
using namespace std;
string int2string(int x){
	string xs = "";
	while(true){
		int rem = x%10;
		xs += ('0'+rem);
		x /= 10;
		if(x==0) break;
	}
	reverse(xs.begin(), xs.end());
	return xs;
}
int main(int argc, char const *argv[])
{


	unsigned char hdr[20];
	int start = 0;
	hdr[start++] = 0x45;
	hdr[start++] = 0x00;
	
	hdr[start++] = 0x00;
	hdr[start++] = 0x73;
	
	hdr[start++] = 0x00;
	hdr[start++] = 0x00;
	
	hdr[start++] = 0x40;
	hdr[start++] = 0x00;
	
	hdr[start++] = 0x40;
	hdr[start++] = 0x11;
	
	hdr[start++] = 0x00;
	hdr[start++] = 0x00;
	
	hdr[start++] = 0xc0;
	hdr[start++] = 0xa8;
	
	hdr[start++] = 0x00;
	hdr[start++] = 0x01;
	
	hdr[start++] = 0xc0;
	hdr[start++] = 0xa8;
	
	hdr[start++] = 0x00;
	hdr[start++] = 0xc7;

	int arr[9];
	int ai = 0;
	int sum = 0;
	for(int i = 0; i<20; i+=2){
		if(i!=10){
			int hi = hdr[i];
			int lo = hdr[i+1];
			int num_16bit = hi*(1<<8) + lo;
			sum += num_16bit;
			arr[ai++] = num_16bit;
		}
	}
	int tmp = sum;
	while(true)	
	{
		int hi = (int) (tmp/(1<<16));
		int lo = (int) (tmp%(1<<16));
		tmp = lo + hi;
		if(hi==0) break;
		
	}
	printf("%x\n", tmp);
	tmp = ((1<<16) - 1) - tmp;
	printf("%x\n", tmp);

	start = 10;
	hdr[start++] = (unsigned char) (tmp/(1<<8));
	hdr[start++] = (unsigned char) (tmp%(1<<8));

	FILE *f = fopen("holytest.bin", "wb");
		fwrite(hdr, 1, 20, f);

} 

