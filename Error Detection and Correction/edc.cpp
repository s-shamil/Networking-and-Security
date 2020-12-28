#include <bits/stdc++.h>
using namespace std;

string dataString;
string paddedDataString;
string generator;
int m;
double prob;
vector<string> dataBlock;
vector<string> receivedDataBlock;
vector<string> receivedDirtyBlock;

//utility
string getBitsFromChar(unsigned char ch);
void printDataBlock();
void printGreen(char s);
void printRed(char s);
void printCyan(char s);
char binStr2Char(string s);
//Hamming Functions
string addCheckBits(string uncheckedStr);
void printCheckBitsGreen(string str);
//CRC Checksum 
string xorStrings(string s1, string s2);
string mod2division(string divident, string divisor);
string getCRCchecksum(string data, string generator); //return crc code only - append in main before sending
//Transmission
pair<string, string> getAfterTransmission(string data, double prob);
//Decode
string removeCheckBits(string str);


int main(int argc, char const *argv[])
{
	//Task 0 - take input
	printf("enter data string: ");
	getline(cin, dataString);

	printf("enter number of data bytes in a row (m): ");
	cin>>m;

	printf("enter probability (p): ");
	cin>>prob;

	printf("enter generator polynomial: ");
	cin>>generator;


	//Task 1 - data string after padding
	paddedDataString = dataString;
	int len = dataString.length();
	if(len%m){
		int tmp = m - (len%m);
		for(int i = 0; i<tmp; i++){
			paddedDataString += '~';
		}
	}
	printf("\ndata string after padding: ");
	cout<<paddedDataString<<endl<<endl;

	//Task 2 - Create the data block
	int paddedLen = paddedDataString.length(); //guaranteed to be multiple of m
	int lines = paddedLen / m;


	for(int i = 0; i<lines; i++){
		string tmp = "";
		for(int j = 0; j<m; j++){
			int idx = i*m + j;
			char lookingAt = paddedDataString[idx];
			tmp += getBitsFromChar(lookingAt);
		}
		dataBlock.push_back(tmp);
	}
	printf("data block <ascii code of m characters per row>: \n");
	printDataBlock();
	cout<<endl;

	//Task 3 - Add check bits Hamming Starts
	//update dataBlock with check bits added
	for(int i = 0; i<lines; i++){
		dataBlock[i] = addCheckBits(dataBlock[i]);
	}
	//print with check bits green colored
	printf("data block after adding check bits:\n");
	for(int i = 0; i<lines; i++){
		printCheckBitsGreen(dataBlock[i]);
	}

	//Task 4 - Serialize data block
	string serializedDataBlock = "";
	int imax = dataBlock[0].length();
	for(int i = 0; i<imax; i++){
		for(int j = 0; j<lines; j++){
			serializedDataBlock += dataBlock[j][i];
		}
	}
	printf("\ndata bits after column-wise serialization:\n");
	cout<<serializedDataBlock<<endl;

	//Task 5 - append CRC checksum
	printf("\ndata bits after appending CRC checksum <sent frame>:\n");
	cout<<serializedDataBlock;
	string crc_checksum = getCRCchecksum(serializedDataBlock, generator);
	int len_tmp = crc_checksum.length();
	for(int i = 0; i<len_tmp; i++){
		printCyan(crc_checksum[i]);
	}
	cout<<endl;
	string dataWithCRC = serializedDataBlock + crc_checksum; //string to be transmitted
	//cout<<dataWithCRC<<endl;

	//sender code ends ---------------------------------------

	//Task 6 - Transmission and received frame (bit toggle)
	pair<string, string> afterTx = getAfterTransmission(dataWithCRC, prob);
	string dirtyData = afterTx.first;
	string dirtyMap = afterTx.second;
	int len_dirtyData = dirtyData.length();
	printf("\nreceived frame:\n");
	for(int i = 0; i<len_dirtyData; i++){
		if(dirtyMap[i]=='1') printRed(dirtyData[i]);
		else cout<<dirtyData[i];
	}
	cout<<endl;

	//receiver code begins -----------------------------------
	string receivedData = dirtyData;
	
	//Task 7 - error detection using crc
	string rem = mod2division(receivedData, generator);
	//cout<<rem<<endl;
	int len_rem = rem.length();
	int corrupt_flag = 0;
	for(int i = 0; i<len_rem; i++){
		if(rem[i]=='1') {
			corrupt_flag = 1;
			break;
		}
	}
	printf("\nresult of CRC checksum matching: ");
	if(corrupt_flag) printf("error detected\n");
	else printf("no error detected\n");

	//Task 8
	//remove crc checksum
	int crc_len = generator.length()-1;
	int data_len = receivedData.length() - crc_len;
	string receivedDataWithoutCRC = receivedData.substr(0, data_len);
	string dirtyMapWithoutCRC = dirtyMap.substr(0, data_len);
	//cout<<receivedDataWithoutCRC<<endl;
	//cout<<dirtyMapWithoutCRC<<endl;
	
	//deserialize receivedData and dirtyMap (Without CRC)
	int len_serialized = receivedDataWithoutCRC.length();
	int gap = len_serialized/lines;

	for(int i = 0; i<lines; i++){
		string tmpData = "", tmpDirty = "";
		for(int j = i; j<len_serialized; j+=lines){
			tmpData += receivedDataWithoutCRC[j];
			tmpDirty += dirtyMapWithoutCRC[j];
		}
		receivedDataBlock.push_back(tmpData);
		receivedDirtyBlock.push_back(tmpDirty);
	}
	printf("\ndata block after removing CRC checksum bits:\n");
	int recv_len = receivedDataBlock[0].length();
	for(int i = 0; i<lines; i++){
		for(int j = 0; j<recv_len; j++){
			if(receivedDirtyBlock[i][j]=='1') printRed(receivedDataBlock[i][j]);
			else cout<<receivedDataBlock[i][j];
		}
		cout<<endl;
	}


	//Task 9 - remove checkbits
	for(int i = 0; i<lines; i++){
		receivedDataBlock[i] = removeCheckBits(receivedDataBlock[i]);
	}
	//print
	printf("\ndata block after removing check bits:\n");
	for(int i = 0; i<lines; i++){
		cout<<receivedDataBlock[i]<<endl;
	}
	
	//Task 10 - retrieve the data screen
	string msg = "";
	for(int i = 0; i<lines; i++){
		string linestring = receivedDataBlock[i];
		for(int j = 0; j<m; j++){
			string charcode = linestring.substr(j*8, 8);
			char ch = binStr2Char(charcode);
			msg += ch;
		}
	}

	printf("\noutput frame: ");
	cout<<msg<<endl<<endl;

	return 0;
}

string getBitsFromChar(unsigned char ch){
	unsigned char mask = 1<<7;
	string ret = "";
	while(mask){
		if(ch & mask) ret += '1';
		else ret += '0';
		mask >>= 1;
	}
	return ret;
}

void printDataBlock(){
	int len = dataBlock.size();
	for(int i = 0; i<len; i++){
		cout<<dataBlock[i]<<endl;
	}
}

string addCheckBits(string uncheckedStr){
	string checkedStr = "";
	//initializing with 0 at powers of two and data bits otherwise
	int dataLen = uncheckedStr.length();
	int dataIdx = 0;
	int nextPowOfTwo = 1;
	int pwmax = 0;
	int idx = 1;
	while(dataIdx<dataLen){
		if(idx == nextPowOfTwo){
			checkedStr += "0";
			nextPowOfTwo <<= 1;
			pwmax++;
		}
		else{
			checkedStr += uncheckedStr[dataIdx];
			dataIdx++;
		}
		idx++;
	}
	
	int newLen = checkedStr.length();
	//xor all idx (1 indexed) elements with pw-th bit set to 1 
	for(int i = 0; i<newLen; i++){
		int i_oneindexed = i+1;
		for(int pw = 0; pw<pwmax; pw++){
			int pwindex1 = (1<<pw);
			int pwindex0 = pwindex1 - 1;
			if(i_oneindexed & pwindex1){
				//xor
				checkedStr[pwindex0] = '0' + ((checkedStr[i]-'0')^(checkedStr[pwindex0]-'0')); 
			}
		}
	}	
	return checkedStr;
}

void printGreen(char s){
	cout << "\033[32m"<<s<<"\033[0m";
}
void printRed(char s){
	cout << "\033[31m"<<s<<"\033[0m";
}
void printCyan(char s){
	cout << "\033[36m"<<s<<"\033[0m";
}

void printCheckBitsGreen(string str){
	int len = str.length();
	int nextPowOfTwo = 1;
	for(int i = 1; i<=len; i++){
		if(i==nextPowOfTwo){
			printGreen(str[i-1]);
			nextPowOfTwo <<= 1;
		}
		else{
			cout<<str[i-1];
		}
	}
	cout<<endl;
}

//CRC Checksum 
string xorStrings(string s1, string s2){
	string ret = "";
	int len = s2.length();
	for(int i = 1; i<len; i++){
		if(s1[i]==s2[i]){
			ret += '0';
		}
		else{
			ret += '1';
		}
	}
	return ret;
}

string mod2division(string divident, string divisor){
	int divident_len = divident.length();
	int range = divisor.length();
	//taking a substring of length=range starting from 0
	string temp = divident.substr(0, range);
	while(range < divident_len){
		string all0 = "";
		for(int i = 0; i<range; i++){
			all0 += '0';
		}
		if(temp[0]=='1'){
			temp = xorStrings(divisor, temp) + divident[range];
		}
		else{
			temp = xorStrings(all0, temp) + divident[range];
		}
		range++;
	}

	if(temp[0]=='1'){
		temp = xorStrings(divisor, temp);
	}
	else{
		string all0 = "";
		for(int i = 0; i<range; i++){
			all0 += '0';
		}
		temp = xorStrings(all0, temp);
	}
	return temp;
}

string getCRCchecksum(string data, string generator){
	int len = generator.length();
	string datawithzeros = data;
	for(int i = 1; i<len; i++){
		datawithzeros += '0';
	}
	string remainder = mod2division(datawithzeros,generator);
	//string ret = data + remainder;
	string ret = remainder;
	return ret;
}

pair<string, string> getAfterTransmission(string data, double prob){
	int len = data.length();
	string dirtyData = data, dirtyMap = "";
	int cnt = 0;

	/*
	random_device rd;  //Will be used to obtain a seed for the random number engine
    mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    uniform_real_distribution<> dis(0.0, 1.0);
	*/
	for(int i = 0; i<len; i++){
		//double random = dis(gen);
		double random = (rand()*1.0)/RAND_MAX;
		//cout<<setprecision(20)<<random<<endl;
		if(random<=prob){
			//flip the bit
			if(dirtyData[i]=='1') dirtyData[i] = '0';
			else dirtyData[i] = '1'; 
			dirtyMap += '1';
			cnt++;
		}
		else{
			dirtyMap += '0';
		}
	}
	//print percentage of dirty bits	
	//cout<<(cnt*100.0)/len<<endl;
	return make_pair(dirtyData, dirtyMap);
}

string removeCheckBits(string str){
	int len = str.length();
	int pw = 0;
	while((1<<pw) <= len){
		pw++;
	}
	int pwmax = pw - 1;
	int flipper = 0;
	for(int pw = 0; pw<=pwmax; pw++){
		int tmpidx = (1<<pw)-1;
		int found = (str[tmpidx]-'0');
		int xorval = 0;
		for(int i = 0; i<len; i++){
			int ione = i+1;
			if((ione != (1<<pw)) && (ione & (1<<pw))){
				int here = (str[i] - '0');
				xorval ^= here;
			}
		}
		if(xorval != found){
			flipper += (1<<pw);
		}
	}
	if(flipper>0){
		//no zero so change needed
		int pos = flipper-1;
		if(str[pos] == '1') str[pos] = '0';
		else str[pos] = '1';
	}
	string ret = "";
	int nextPowOfTwo = 1;
	for(int i = 1; i<=len; i++){
		if(i==nextPowOfTwo){
			nextPowOfTwo <<= 1;
		}
		else{
			ret += str[i-1];
		}
	}
	return ret;
}

char binStr2Char(string s){
	int ret = 0;
	int base = 1;
	for(int i = 0; i<8; i++){
		if(s[7-i]=='1'){
			ret += base;
		}
		base *= 2;
	}
	unsigned char ch = ret;
	return ch;
}