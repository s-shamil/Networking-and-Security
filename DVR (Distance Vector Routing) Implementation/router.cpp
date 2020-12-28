#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <stack>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define INF 999999
#define NULLHOP "--null--" //nexthop if null

using namespace std;


struct RoutingTableEntry{
	string destination;
	string nextHop;
	int cost;

	bool operator<(const RoutingTableEntry& rt) const {
		return destination < rt.destination;
	}
} typedef routingTableEntry;

vector<string> routersListTemp;
vector<routingTableEntry> routingTable;
vector< pair<string,int> > neighbor_cost; //stores <neighbor ip, cost to neighbor>
vector< pair<string,int> > neighbor_cost_org; //stores original <neighbor ip, cost to neighbor>

vector< pair<string,int> > lastClk;
int myclock;

string this_router_ip;
int sockfd;

//utility

string intToString(int a){
	stack<char> dgts;
	while(a){
		dgts.push('0' + (a%10));
		a/=10;
	}
	string s = "";
	while(dgts.size()!=0){
		s += dgts.top();
		dgts.pop();
	}
	return s;
}

bool presentInRoutingTable(string ipAddress){
	int sz = routingTable.size();
	routingTableEntry rte;
	for(int i = 0; i<sz; i++){
		rte = routingTable[i];
		if(rte.destination==ipAddress){
			return true;
		}
	}
	return false;
}

//using this function to reconstruct routing table of neighbor from received string
vector<RoutingTableEntry> reconstructRoutingTable(string s){
	vector<RoutingTableEntry> tmp;
	RoutingTableEntry rteTemp;
	stringstream ss;
	ss << s;
	string dummy, ip, nexthop;
	int cost_this;
	ss >> dummy >> dummy; //cutting off "table" and neighbor ip from string - they are used as needed before coming here
	while(ss >> ip >> nexthop >> cost_this){
		//cout<<"tommy home "<< ip <<"x"<<nexthop<<"x"<<cost_this<<endl;
		rteTemp.destination = ip;
		rteTemp.nextHop = nexthop;
		rteTemp.cost = cost_this;
		tmp.push_back(rteTemp);
	}
	return tmp;
}
void printRT(){
	int sz = routingTable.size();
	routingTableEntry rte;
	cout<<"Destination\tNext Hop\tCost\n";
	for(int i = 0; i<sz; i++){
		rte = routingTable[i];
		cout<<rte.destination<<"\t"<<rte.nextHop<<"\t"<<rte.cost<<endl;
	}
}

void printAnyRoutingTable(vector<RoutingTableEntry> rt){
	int sz = rt.size();
	routingTableEntry rte;
	cout<<"Destination\tNext Hop\tCost\n";
	for(int i = 0; i<sz; i++){
		rte = rt[i];
		cout<<rte.destination<<"\t"<<rte.nextHop<<"\t"<<rte.cost<<endl;
	}
}
void printNeighbors(){
	int sz = neighbor_cost.size();
	for(int i = 0; i<sz; i++){
		cout<<neighbor_cost[i].first<< "  " <<neighbor_cost[i].second<<endl;
	}
}

void initializeRrouter(string ipAddress, string topoFile){
	string router1ip, router2ip;
	int cost_here;

	routersListTemp.clear();

	string s_tmp;
	ifstream inputFile;
	inputFile.open (topoFile.c_str());
	while( getline(inputFile, s_tmp) ){
		stringstream ss_tmp;
		ss_tmp << s_tmp;
		ss_tmp>>router1ip>>router2ip>>cost_here;
		//cout<<router1ip<< " : " <<router2ip<< " : " <<cost_here<<endl;
		if(router1ip==ipAddress){
			//router 2 is neighbor
			routingTableEntry rte;
			rte.destination = router2ip;
			rte.nextHop = router2ip;
			rte.cost = cost_here;
			routingTable.push_back(rte);
			neighbor_cost.push_back(make_pair(router2ip, cost_here));
			neighbor_cost_org.push_back(make_pair(router2ip, cost_here));

			lastClk.push_back(make_pair(router2ip, 0));
		}
		else if(router2ip==ipAddress){
			//router 1 is neighbor
			routingTableEntry rte;
			rte.destination = router1ip;
			rte.nextHop = router1ip;
			rte.cost = cost_here;
			routingTable.push_back(rte);
			neighbor_cost.push_back(make_pair(router1ip, cost_here));
			neighbor_cost_org.push_back(make_pair(router1ip, cost_here));

			lastClk.push_back(make_pair(router1ip, 0));
		}
		else{
			//router 1 and/or 2 may or may not be neighbor
			routersListTemp.push_back(router1ip);
			routersListTemp.push_back(router2ip);
		}
	}

	int sz = routersListTemp.size();
	for(int i = 0; i<sz; i++){
		//check in existing routing table if present
		if(!presentInRoutingTable(routersListTemp[i])){
			routingTableEntry rte;
			rte.destination = routersListTemp[i];
			rte.nextHop = NULLHOP;
			rte.cost = INF;
			routingTable.push_back(rte);
		}
	}
	inputFile.close();
	sort(routingTable.begin(), routingTable.end());
	printRT();
}

void updateThisRoutingTable(string neighbor_ip, vector<RoutingTableEntry> neighbor_table)
{
	//find cost to neighbor
	int cost_edge;
	int sz = neighbor_cost.size();
	for(int i = 0; i<sz; i++){
		if(neighbor_cost[i].first == neighbor_ip){
			cost_edge = neighbor_cost[i].second;
			break;
		}
	}
	int print_routing_table_incaseof_change = 0;
	//iterate over this router's destinations
	int sz_this = routingTable.size();
	for(int i = 0; i<sz_this; i++){
		int prev_cost = routingTable[i].cost; //stores the cost to destination before updating

		//destination is neighbor itself - direct edge modification case
		if(routingTable[i].destination==neighbor_ip){
			//if nexthop is neighbor too - this edge is used to get to neighbor - so must be modified
			//or if using that edge costs less than other route
			if(routingTable[i].nextHop==neighbor_ip || cost_edge < routingTable[i].cost){
				routingTable[i].cost = cost_edge;
				routingTable[i].nextHop = neighbor_ip;
				if(routingTable[i].cost != prev_cost){
					print_routing_table_incaseof_change = 1;
				}
			}
		}
		//find that destination in neighbors table
		int sz_that = neighbor_table.size();
		for(int j = 0; j<sz_that; j++){
			if(routingTable[i].destination == neighbor_table[j].destination){
				int cost_through_neighbor = cost_edge + neighbor_table[j].cost;
				//update if the neighbor was the nexthop to destination - because the cost of <this--to--neighbor> or <neighbor--to--destination> might have changed
				//update if going through neighbor costs less
				if(routingTable[i].nextHop == neighbor_ip || cost_through_neighbor < routingTable[i].cost){
					routingTable[i].cost = cost_through_neighbor;
					routingTable[i].nextHop = neighbor_ip;
					if(routingTable[i].cost != prev_cost){
						print_routing_table_incaseof_change = 1;
					}
				}
				break;
			}
		}	
		
	}
	if(print_routing_table_incaseof_change){
		cout<<"Updated table after getting table from "<<neighbor_ip<<endl;
		printRT();
	}
}

void updateRoutingTableOnEdgeCostChange(string neighbor_ip, int new_cost){
	int sz_cost = neighbor_cost.size();
	int old_cost;
	for(int i = 0; i < sz_cost; i++){
		if(neighbor_cost[i].first==neighbor_ip){
			
			old_cost = neighbor_cost[i].second;
			neighbor_cost[i].second = new_cost;

			//check in my routing table - the destinations for which next hop is this neighbor
			int sz = routingTable.size();
			for(int j = 0; j<sz; j++){
				if(routingTable[j].nextHop==neighbor_ip){
					routingTable[j].cost = routingTable[j].cost - old_cost + new_cost;
				}
			}
			break;
		}
	}

}

//utility for cost <ip1> <ip2> <value> command
string decodeIP(string s){
	//s is 4 chars
	string ip = "";
	unsigned char uc;
	for(int i = 0; i<4; i++){
		uc = s[i];
		int x = uc;
		ip += intToString(x);
		if(i!=3) ip += ".";
	}
	return ip;
}

int decodeCost(string s){
	//s is 2 chars - xy
	unsigned char x = s[0];
	unsigned char y = s[1];
	int xx = x, yy = y;
	return xx + yy * 256;
}

void updateEdgeCost(string msg){
	string ip1encoded = msg.substr(4,4);
	string ip1 = decodeIP(ip1encoded);
	string ip2encoded = msg.substr(8,4);
	string ip2 = decodeIP(ip2encoded);
	string cost_encoded = msg.substr(12,2);
	int cost_decoded = decodeCost(cost_encoded);

	cout<<"decoded: "<<ip1<< " "<<ip2<<" " <<cost_decoded<<endl;

	if(ip1==this_router_ip){
		//update in neighbor_cost vector
		int sz = neighbor_cost.size();
		for(int i = 0; i<sz; i++){
			//add code to check if the link down - if down do nothing
			if(neighbor_cost[i].first==ip2){
				if(neighbor_cost[i].second != INF){
					//link is not down - so must update edge cost

					//neighbor_cost[i].second = cost_decoded;
					updateRoutingTableOnEdgeCostChange(ip2, cost_decoded);

					neighbor_cost_org[i].second = cost_decoded;
				}
				break;
			}
		}
	}

	else if(ip2==this_router_ip){
		//update in neighbor_cost vector
		int sz = neighbor_cost.size();
		for(int i = 0; i<sz; i++){
			//add code to check if the link down - if down do nothing
			if(neighbor_cost[i].first==ip1){
				if(neighbor_cost[i].second != INF){
					//link is not down - so must update edge cost

					//neighbor_cost[i].second = cost_decoded;
					updateRoutingTableOnEdgeCostChange(ip1, cost_decoded);

					neighbor_cost_org[i].second = cost_decoded;
				}
				break;
			}
		}
	}
}

void detectLinkFailure(){
	//remove cout gulo
	//cout<<"detecting Linl failure------------------"<<endl;
	int sz = lastClk.size();
	for(int i = 0; i<sz; i++){
		//cout<<myclock<<" - "<<lastClk[i].second<<endl;
		
		//check last clock and if already not down
		if((myclock - lastClk[i].second)>3 && neighbor_cost[i].second!=INF){
			//remove
			//cout<<"down this link"<<endl;

			//this link is dead
			//push back for lastClk and neighbor_cost is done at the same time
			//so i-th in neighbor_cost is of same ip -> [lastClk[i].first = neighbor_cost[i].first]
			//link is just down - do not change the original cost vector
			
			//neighbor_cost[i].second = INF;
			updateRoutingTableOnEdgeCostChange(lastClk[i].first, INF);
		}
	}
}

string destinationFromForwardMsg(string msg){
	stringstream ss;
	ss << msg;
	string ip;
	ss >> ip; // actually "frwd"
	ss >> ip; //ip
	return ip;
}

string mainTextFromForwardMsg(string msg){
	stringstream ss;
	ss << msg;
	int start = 0;
	string dummy, slen;
	int len;

	ss >> dummy; //cut off - frwd
	start += (dummy.length()+1);
	ss >> dummy; //cut off - destination ip
	start += (dummy.length()+1);
	ss >> slen;
	start += (slen.length()+1);

	stringstream tmp;
	tmp << slen;
	tmp >> len;

	string mainText = "";
	for(int i = 0; i<len; i++){
		mainText += msg[start+i];
	}
	return mainText;
}

void forwardMessage(string destIP, string fwd_text){
	int sz = routingTable.size();
	for(int i = 0; i<sz; i++){
		if(routingTable[i].destination==destIP){
			string nexthop = routingTable[i].nextHop;
			string main_text = mainTextFromForwardMsg(fwd_text);

			int len = fwd_text.length();

			char buffer1[1024];
			for(int i = 0; i<len; i++){
				buffer1[i] = fwd_text[i];
			}
			buffer1[len] = '\0';


			struct sockaddr_in neighbor_address;
			neighbor_address.sin_family = AF_INET;
			neighbor_address.sin_port = htons(4747);
			neighbor_address.sin_addr.s_addr = inet_addr(nexthop.c_str());

			sendto(sockfd, buffer1, len+1, 0, (struct sockaddr*) &neighbor_address, sizeof(sockaddr_in));
			cout<<main_text<<" packet forwarded to "<<nexthop<<endl;
			break;
		}
	}
}


//this function probably needs modification 
	//one parameter needed - ip of the router to whom the string will be sent
	//based on the ip and nexthop take necessary action - adding poisson reverse
string getRoutingTableAsString(){
	string s = "";
	int sz = routingTable.size();
	for(int i = 0; i<sz; i++){
		s+=routingTable[i].destination;
		s+=" ";
		s+=routingTable[i].nextHop;
		s+=" ";
		s+=intToString(routingTable[i].cost);
		s+="\n";
	}
	return s;
}

string getPoisonedRoutingTableAsString(string neighbor_ip){
	string s = "";
	int sz = routingTable.size();
	for(int i = 0; i<sz; i++){
		s+=routingTable[i].destination;
		s+=" ";
		s+=routingTable[i].nextHop;
		s+=" "; 
		if(routingTable[i].nextHop==neighbor_ip){
			s+=intToString(INF);
			s+="\n";
		}else{	
			s+=intToString(routingTable[i].cost);
			s+="\n";
		}
	}
	return s;
}

int main(int argc, char const *argv[])
{
	// router ip-address topology-file
	// int sockfd;
	int bind_flag;
	//int bytes_received;
	socklen_t addrlen;
	//char buffer[1024];
	struct sockaddr_in client_address;
	struct sockaddr_in driver_address;

	myclock = 0;

	if(argc != 3){
		printf("%s <ip address> <topology-file>\n", argv[0]);
		exit(1);
	}
	string ipAddress, topoFile;
	ipAddress = argv[1];
	topoFile = argv[2];
	this_router_ip = ipAddress;

	client_address.sin_family = AF_INET;
	client_address.sin_port = htons(4747);
	client_address.sin_addr.s_addr = inet_addr(argv[1]);

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	bind_flag = bind(sockfd, (struct sockaddr*) &client_address, sizeof(sockaddr_in));

	if(!bind_flag){
		printf("Connection established...\n");
		
	}
	else{
		printf("Problem with connection\n");
		exit(1);	
	}
	
	//initialize this router
	initializeRrouter(ipAddress, topoFile);
	//up and running

	while(true){
		char buffer[1024];
		int bytes_received;
		memset(buffer, '\0', sizeof(buffer)); //buffer cleared
		bytes_received = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) &driver_address, &addrlen);
		//if(bytes_received!=-1){
		string msg(buffer);
		//what this router got, test print
		//printf("[%s:%d]: %d\n", inet_ntoa(driver_address.sin_addr), ntohs(driver_address.sin_port), bytes_received);
		//cout<<msg<<endl;

		string msg_prefix = msg.substr(0,4);
		//cout<<msg_prefix<<" prefix"<<endl;
		//show ip
		if (msg_prefix=="show")	
		{
			// show is a prefix of msg
			cout<<":::Routing Table View requested from driver.py:::"<<endl;
			printRT();
			//printNeighbors();
			//cout<<getRoutingTableAsString()<<"\nXXXXXXXXX"<<endl;
		}
		//}
		else if (msg_prefix=="clk ")
		{

			myclock++;
			
			//cout<<myclock<<" NOW"<<endl;

			//time to detect link failure
			detectLinkFailure();

			//clk found - send table to neighbors
			//cout<<msg<<endl;
			int sz = neighbor_cost.size();

			//this portion is commented out after adding poisoned reverse
			/*
			char buffer1[1024];
			string rtstring = "tbl " + ipAddress + "\n" + getRoutingTableAsString();
			int len = rtstring.length();
			for(int i = 0; i<len; i++){
				buffer1[i] = rtstring[i];
			}
			buffer1[len] = '\0';
			*/
			for(int i = 0; i<sz; i++){
				//cout<<"TEST:\n";
				//cout<<getPoisonedRoutingTableAsString(neighbor_cost[i].first);

				char buffer1[1024];
				string rtstring = "tbl " + ipAddress + "\n" + getPoisonedRoutingTableAsString(neighbor_cost[i].first);
				int len = rtstring.length();
				for(int j = 0; j<len; j++){
					buffer1[j] = rtstring[j];
				}
				buffer1[len] = '\0';
				
				struct sockaddr_in neighbor_address;
				neighbor_address.sin_family = AF_INET;
				neighbor_address.sin_port = htons(4747);
				neighbor_address.sin_addr.s_addr = inet_addr(neighbor_cost[i].first.c_str());

				sendto(sockfd, buffer1, len+1, 0, (struct sockaddr*) &neighbor_address, sizeof(sockaddr_in));
			}

		}
		else if(msg_prefix=="tbl ")
		{
			//routing table received from neighbor
			vector<RoutingTableEntry> neighborTable;
			string neighborIP, dummy;
			stringstream full_msg;
			full_msg << msg;
			full_msg >> dummy;
			full_msg >>neighborIP;

			//update last myclock for this neighbor in vector
			int sz_lc = lastClk.size();
			for(int i = 0; i<sz_lc; i++){
				if(lastClk[i].first == neighborIP){
					if(neighbor_cost[i].second == INF){
						//this link was down - but now got a packet - so link up happened - update cost
						neighbor_cost[i].second = neighbor_cost_org[i].second;
					}
					lastClk[i].second = myclock;
					//remove
					//cout<<"tbl from "<<neighborIP<<" at "<<myclock<<endl;
					break;
				}
			}

			//cout<<"got table from " <<neighborIP<<endl;
			neighborTable = reconstructRoutingTable(msg);
			//printAnyRoutingTable(neighborTable);
			//cout<<"Updated Table after getting table from "<<neighborIP<<endl;

			//update routing table - pass neighbor's ip and table
			updateThisRoutingTable(neighborIP, neighborTable);
			//printRT();
		}
		else if(msg_prefix=="cost"){
			//cout<<"shit got "<<msg<<endl;
			updateEdgeCost(msg);
 		}
 		else if(msg_prefix=="send"){
 			//decode needed
 			//encoded => send<ip1><ip2><msg-len><msg>
 			//decoded => fwrd xxx.xxx.xxx.xxx xxx.xxx.xxx.xxx msg

 			string ip1 = decodeIP(msg.substr(4,4));
 			string ip2 = decodeIP(msg.substr(8,4));
 			int msg_len = decodeCost(msg.substr(12,2));
 			//cout<<msg.length()<<"X"<<endl;
 			//cout<<msg<<endl;
 			//string main_text = msg.substr(14,msg_len-1);
 			//cout<<main_text<<endl;
 			//cout<<msg_len<<endl;
 			char main_text[msg_len+1];
 			for(int i = 0; i<msg_len; i++){
 				main_text[i] = buffer[14+i]; //string[14+i] didnt work here, neither did substring
 			}
 			main_text[msg_len] = '\0';
 			//printf("Text: %s\n", main_text);
 			string text(main_text);
 			//string new_msg = "fwrd " + ip1 + " " + ip2 + " " + intToString(msg_len) + "\n" + text + "\n";
 			string new_msg = "frwd " + ip2 + " " + intToString(msg_len) + " " + text;
 			cout<<new_msg<<endl;

 			if(ip1==ip2){
 				//this is the destination too
 				cout<<text<<" packet reached destination."<<endl; 
 			}
 			else{
 				forwardMessage(ip2, new_msg);
 			}
 		}
 		else if(msg_prefix=="frwd"){
 			string destination = destinationFromForwardMsg(msg);
 			string main_text = mainTextFromForwardMsg(msg);
 			if(destination == this_router_ip){
 				cout<<main_text<<" packet reached destination."<<endl;
 			}
 			else{
 				forwardMessage(destination, msg);
 			}
 		}
	}

	close(sockfd);

	return 0;
}