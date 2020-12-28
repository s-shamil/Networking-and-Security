#include <bits/stdc++.h>
using namespace std;

#define ull unsigned long long int
typedef basic_string<unsigned char> ustring;

std::ostream& operator << (std::ostream& os, const std::basic_string<unsigned char>& str){
    for(auto ch : str)
        os << static_cast<char>(ch);
    return os;
}

//metrices are copied here
int PI[] = {58, 50, 42, 34, 26, 18, 10, 2,
      60, 52, 44, 36, 28, 20, 12, 4,
      62, 54, 46, 38, 30, 22, 14, 6,
      64, 56, 48, 40, 32, 24, 16, 8,
      57, 49, 41, 33, 25, 17, 9, 1,
      59, 51, 43, 35, 27, 19, 11, 3,
      61, 53, 45, 37, 29, 21, 13, 5,
      63, 55, 47, 39, 31, 23, 15, 7};

int CP_1[] = {57, 49, 41, 33, 25, 17, 9,
        1, 58, 50, 42, 34, 26, 18,
        10, 2, 59, 51, 43, 35, 27,
        19, 11, 3, 60, 52, 44, 36,
        63, 55, 47, 39, 31, 23, 15,
        7, 62, 54, 46, 38, 30, 22,
        14, 6, 61, 53, 45, 37, 29,
        21, 13, 5, 28, 20, 12, 4};

int CP_2[] = {14, 17, 11, 24, 1, 5, 3, 28,
        15, 6, 21, 10, 23, 19, 12, 4,
        26, 8, 16, 7, 27, 20, 13, 2,
        41, 52, 31, 37, 47, 55, 30, 40,
        51, 45, 33, 48, 44, 49, 39, 56,
        34, 53, 46, 42, 50, 36, 29, 32};

int E[] = {32, 1, 2, 3, 4, 5,
     4, 5, 6, 7, 8, 9,
     8, 9, 10, 11, 12, 13,
     12, 13, 14, 15, 16, 17,
     16, 17, 18, 19, 20, 21,
     20, 21, 22, 23, 24, 25,
     24, 25, 26, 27, 28, 29,
     28, 29, 30, 31, 32, 1};

int PI_2[] = {35, 38, 46, 6, 43, 40, 14, 45,
		33, 19, 26, 15, 23, 8, 22, 10, 
		12, 11, 5, 25, 27, 21, 16, 31,
		28, 32, 34, 24, 9, 37, 2, 1};

int P[] = {16, 7, 20, 21, 29, 12, 28, 17,
     1, 15, 23, 26, 5, 18, 31, 10,
     2, 8, 24, 14, 32, 27, 3, 9,
     19, 13, 30, 6, 22, 11, 4, 25};

int PI_1[] = {40, 8, 48, 16, 56, 24, 64, 32,
        39, 7, 47, 15, 55, 23, 63, 31,
        38, 6, 46, 14, 54, 22, 62, 30,
        37, 5, 45, 13, 53, 21, 61, 29,
        36, 4, 44, 12, 52, 20, 60, 28,
        35, 3, 43, 11, 51, 19, 59, 27,
        34, 2, 42, 10, 50, 18, 58, 26,
        33, 1, 41, 9, 49, 17, 57, 25};

int SHIFT[] = {1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};


//code starts here
int encryption_mode = 1;
ustring key, plaintext;
ull key_ull;
vector<ull> keys_for_iterations;

//encryption - task 1
vector<ustring> grouping(ustring inp){
    //cout<<"GROUPING: "<<inp.length()<<endl;
    vector<ustring> tmp;
    int len = inp.length();

	ustring s;
    s.clear();
	s += inp[0];
    for(int i = 1; i<len; i++){
        if(i%8==0){
			tmp.push_back(s);
            
			s.clear();
        }
		s += inp[i];
    }
	int space_needed = 8 - s.length();
	for(int i = 0; i<space_needed; i++) s+=' ';
	tmp.push_back(s);
	return tmp;
}

void printBits(ull x){
	for(ull i = 63; ; i--){
		if(x & (1LL<<i)) cout<<1;
		else cout<<0;
		if(i%8==0) cout<<' ';
		if(i==0) break;
	}
	cout<<endl;
}

//this left rotates a 28 bit number by 1
ull rotate_28_bit(ull num){
	num = num<<1;
	if(num & (1LL<<28)){
		num |= (1LL);
		num ^= (1LL<<28);
	}
	return num;
}

//returns rotated 56 bit key and 48 but permutated key 
pair<ull, ull> make_key_i(ull key_ull, int i){
	//last 56 bit are useful - dividing 28 bits portions
	ull right_extractor = (1LL<<28) - 1;
	ull left_extractor = ((1LL<<56) - 1) & (~right_extractor);
	ull right = (key_ull & right_extractor);
	ull left = (key_ull & left_extractor)>>28;
	//left --- right
	int shifter = SHIFT[i];
	for(int i = 0; i<shifter; i++){
		left = rotate_28_bit(left);
		right = rotate_28_bit(right);
	}
	//now key_new is formed by concatenation
	ull key_new_56 = (left<<28) | right;
	ull key_new_48 = 0;
	for(int i = 47; ; i--){
		int whichbit = CP_2[47-i];
		whichbit = 56 - whichbit; // 1 indexed addr given by PI so 64 comes here
		//cout<<whichbit<<' ';
		if(key_new_56 & (1LL<<whichbit)){
			//set 1 in this position of new num
			key_new_48 |= (1LL<<i);
		}
		if(i==0) break;
	}
	return make_pair(key_ull, key_new_48);
}

//makes 64 bit ull and then uses CP_1 to build 56 bit key in LSB side
ull make_key(ustring key){
	//key has 8 chars = 64 bits
	ull num = 0;
	int len = key.length();
	//building the ull num
	for(int i = len-1, shifter = 0; i>=0; i--, shifter+=8){
		ull nw = (ull) key[i];
		num |= (nw<<shifter);
	}

	//transpose and reduce ull num using CP_1
	ull newnum = 0;
	for(int i = 55; ; i--){
		int whichbit = CP_1[55-i];
		whichbit = 64 - whichbit; // 1 indexed addr given by PI so 64 comes here
		//cout<<whichbit<<' ';
		if(num & (1LL<<whichbit)){
			//set 1 in this position of new num
			newnum |= (1LL<<i);
		}
		if(i==0) break;
	}
	//cout<<endl;
	//printBits(newnum);
	//cout<<endl;
	return newnum;
}

ull iteration_func(ull Ri, ull Ki){
	//Ri 32 bit, Ki 48 bit
	ull e = 0;
	for(int i = 47; ; i--){
		int whichbit = E[47-i];
		whichbit = 32 - whichbit;
		if(Ri & (1LL<<whichbit)){
			e |= (1LL<<i);
		}
		if(i==0) break;
	}
	
	//jor 48 bit
	ull jor = (e^Ki);
	ull sample32 = 0;
	for(int i = 31; ; i--){
		int whichbit = PI_2[31-i];
		whichbit = 48 - whichbit;
		if(jor & (1LL<<whichbit)){
			sample32 |= (1LL<<i);
		}
		if(i==0) break;
	}
	ull permuted_sample_32 = 0;
	for(int i = 31; ; i--){
		int whichbit = P[31-i];
		whichbit = 32 - whichbit;
		if(sample32 & (1LL<<i)){
			permuted_sample_32 |= (1LL<<whichbit);
		}
		if(i==0) break;
	}
	return permuted_sample_32;
}	

void printuc(unsigned char x){
	for(ull i = 7; ; i--){
		if(x & (1<<i)) cout<<1;
		else cout<<0;
		if(i%8==0) cout<<' ';
		if(i==0) break;
	}
	cout<<endl;
}

ustring encrypt_word(ustring word){
	//cout<<"Peyechi: "<<word<<endl;
	//word consists 8 characters so must be 64 bits - using ull to handle it
	ustring ret ;
    ret.clear();
	ull num = 0;
	int len = word.length();
	//building the ull num
	for(int i = len-1, shifter = 0; i>=0; i--, shifter+=8){
		ull nw = (ull) word[i];
		//cout<<"**"<<nw<<endl;
		num |= (nw<<shifter);
	}
	//cout<<"AGAIN: ";
	//printBits(num);
	//transpose ull num using PI 
	ull newnum = 0;
	for(int i = 63; ; i--){
		int whichbit = PI[63-i];
		whichbit = 64 - whichbit; // 1 indexed addr given by PI so 64 comes here
		//cout<<whichbit<<' ';
		if(num & (1LL<<whichbit)){
			//set 1 in this position of new num
			newnum |= (1LL<<i);
		}
		if(i==0) break;
	}
	//cout<<endl;
	
	//printBits(newnum);
	//cout<<endl;

	//making Li Ri 
	ull Li, Ri; //32 bits in LSB of Li Ri
	ull lsb_extractor = (1LL<<32)-1;
	ull msb_extractor = (~lsb_extractor);
	ull lsb = newnum & lsb_extractor;
	ull msb = newnum & msb_extractor;
	Li = msb>>32;
	Ri = lsb;

	//run iteration for 16 times
	for(int i = 0; i<16; i++){
		ull Ki;
		if(encryption_mode==1){
			//key in forward order
			Ki = keys_for_iterations[i];
		} else{
			//key in reverse order
			Ki = keys_for_iterations[15-i];
		}
		//cout<<"Iteration "<<i<<" KEY : "; printBits(Ki);
		ull L_new, R_new, tmp;
		L_new = Ri;
		tmp = iteration_func(Ri, Ki);
		R_new = (Li ^ tmp);
		Li = L_new;
		Ri = R_new;
		ull toprint = (Li<<32) | Ri;
		//cout<<"Iteration "<<i<<": "; printBits(toprint);
	}
	//swap and transpose
	ull swapped_num = ((Ri<<32) | Li);
	
	ull final_num = 0;
	for(int i = 63; ; i--){
		int whichbit = PI_1[63-i];
		whichbit = 64 - whichbit;
		if(swapped_num & (1LL<<whichbit)){
			final_num |= (1LL<<i);
		}
		if(i==0) break;
	}
	cout<<word<<"->CipherText Bits: "; printBits(final_num);

	//making the ustring from final_num
	int shifter = 7;
	unsigned char chnum = 0;
	for(int i = 63; ; i--){
		if(final_num & (1LL<<i)){
			chnum |= (1<<shifter);
		}
		shifter--;
		if(i%8==0){
			ret += chnum;
			//cout<<" # "<<chnum<<endl;
			//printf("# %u ", chnum);
			//putc(chnum, stdout);
			//cout<<endl;
			//printuc(chnum);
			//cout<<endl;
			chnum = 0;
			shifter = 7;
		}
		if(i==0) break;
	}
	//cout<<"word code: "<<endl;
	//cout<<"POP : ";
	//printBits(final_num); 
    
	//cout<<ret<<endl;
	ull num2 = 0;
	int len2 = ret.length();
	//building the ull num
	for(int i = len2-1, shifter = 0; i>=0; i--, shifter+=8){
		ull nw = 0;
		for(int j = 0; j<8; j++){
			if(ret[i] & (1<<j)){
				nw |= (1<<j);
			}
		}
		//printuc((unsigned char)(nw));
		//cout<<"x "<<nw<<endl;
		//printBits(nw);
		//printBits(num2);
		num2 |= (nw<<shifter);
		//printBits(num2);
	}
	//cout<<"POP : ";
	//printBits(num2);

	return ret;
}

int main(){
	freopen("input.txt", "r", stdin);
    cout<<"KEY: ";
    string key_, plaintext_;
    getline(cin, key_);
    cout<<"PLAINTEXT: ";
    getline(cin, plaintext_);
	cout<<endl;
    int l1 = key_.length(), l2 = plaintext_.length();
    //cout<<l1<<' '<<l2<<endl;
    for(int i = 0; i<l1; i++){
        key.push_back(key_[i]);
    }
    for(int i = 0; i<l2; i++){
        plaintext.push_back(plaintext_[i]);
    }
    //cout<<key<<endl<<plaintext<<endl;
    //cout<<"BEfore grp: "<<plaintext<<" : "<<plaintext.length();
    vector<ustring> grp = grouping(plaintext);

	key_ull = make_key(key); //makes 64 bit ull and then uses CP_1 to build 56 bit key in LSB side
	//printBits(key_ull);
	
	for(int i = 0; i<16; i++){
		pair<ull,ull> ret = make_key_i(key_ull, i);
		key_ull = ret.first;
		ull Ki = ret.second;
		keys_for_iterations.push_back(Ki);
	}

	/*
	for(int i = 0; i<grp.size(); i++){
		cout<<grp[i]<<"\\"<<endl;
	}
	*/
	ustring encrypted_text;
    encrypted_text.clear();

	int grp_elements_cnt = grp.size();
	for(int i = 0; i<grp_elements_cnt; i++){
        ustring ew = encrypt_word(grp[i]);
        //cout<<"WORD ENC : "<<i<<" : " <<ew<<endl;
		encrypted_text += ew;
	}
	cout<<"Ciphered Text: "<<encrypted_text<<endl;
	int len = encrypted_text.length();
	for(int i = len-1, shifter = 0; i>=0; i--, shifter+=8){
		ull nw = (ull) encrypted_text[i];
		//cout<<"got**"<<nw<<endl;
	}

	//decryption starts
	encryption_mode = 0;

	grp.clear();
	grp = grouping(encrypted_text);
	ustring decrypted_text;
	decrypted_text.clear();
    grp_elements_cnt = grp.size();
	/*
	for(int i = 0; i<grp.size(); i++){
		cout<<grp[i]<<"\\"<<endl;
	}
	*/
	
	for(int i = 0; i<grp_elements_cnt; i++){
		decrypted_text += encrypt_word(grp[i]);
	}
	cout<<"Deciphered Text: "<<decrypted_text<<endl;
    return 0;
}