#include <bits/stdc++.h>
using namespace std;
#define DEBUG 0

string cipher_text, plain_text;
vector<char> most_used_char;
vector<string> words_confirmed;
char mapper[26];
int cnt_in_cipher[26];

void count_top_three(){
    int len = cipher_text.length();
    for(int i = 0; i<len; i++){
        cnt_in_cipher[ (cipher_text[i]-'A') ]++;
    }
    vector<pair<int,int> > char_cnt;
    for(int i = 0; i<26; i++){
        char_cnt.push_back(make_pair(cnt_in_cipher[i], i));
    }
    sort(char_cnt.begin(), char_cnt.end());
    reverse(char_cnt.begin(), char_cnt.end());
    
    if(DEBUG){
        cout<<char(char_cnt[0].second+'A')<< " : "<< char_cnt[0].first<<endl;
        cout<<char(char_cnt[1].second+'A')<< " : "<< char_cnt[1].first<<endl;
        cout<<char(char_cnt[2].second+'A')<< " : "<< char_cnt[2].first<<endl;    
    }
    for(int i = 0; i<3; i++){
        mapper[char_cnt[i].second] = most_used_char[i];
    }
    /*
    mapper[char_cnt[0].second] = most_used_char[0];
    mapper[char_cnt[1].second] = most_used_char[1];
    mapper[char_cnt[2].second] = most_used_char[2];
    */
    for(int i = 1; i<3; i++){
        if(char_cnt[i-1].first == char_cnt[i].first){
            printf("Ambiguity found in most frequent character number %d and %d!!!\n", i, i+1);
        }
    }
    /*
    if(char_cnt[0].first==char_cnt[1].first || char_cnt[1].first==char_cnt[2].first){
        //ADD CODE
        printf("Ambiguity found in most frequent characters!!!\n");
    }
    */
    
    for(int i = 0; i<len; i++){
        plain_text[i] = mapper[cipher_text[i]-'A'];
    }
    if(DEBUG){
        cout<<plain_text<<endl;
    }
}

bool mapped(char ch){
    int idx = ch - 'A';
    char img = mapper[idx];
    if(img>='a' && img<='z') return true;
    return false;
}

void decipher_by_word(string s){
    int match = 0, match_start = -1;

    int n = s.length();
    int len = cipher_text.length();

    //running for template
    for(int i = 0; i<len; i++){
        //string s starts at i
        int run = 1;
        for(int j = 0; j<n; j++){
            for(int k = j+1; k<n; k++){
                if(cipher_text[i+j]==cipher_text[i+k] && s[j]!=s[k]){
                    run = 0;
                }
                if(cipher_text[i+j]!=cipher_text[i+k] && s[j]==s[k]){
                    run = 0;
                }
                if(run == 0) break;
            }
            if(run == 0) break;
        }
        if(run==1){
            //check for consistency by already mapped characters
            for(int ii = i; ii<i+n; ii++){
                if(mapped(cipher_text[ii])){
                    if(mapper[cipher_text[ii]-'A'] != s[ii-i]){
                        run = 0;
                        break;
                    }
                }
            }
        }
        if(run==1){
            //perfect match found
            for(int ii = i; ii<i+n; ii++){
                if(!mapped(cipher_text[ii])){
                    mapper[cipher_text[ii]-'A'] = s[ii-i];
                }
            }
            for(int ii = 0; ii<len;ii++){
                plain_text[ii] = mapper[cipher_text[ii]-'A']; 
            }
        }
    }
}

int main(){
    freopen("substitution-21.txt", "r", stdin);
    for(int i = 0; i<26; i++) {
        mapper[i] = 'A'+i;
        cnt_in_cipher[i] = 0;
    }
    cin>>cipher_text;
    plain_text = cipher_text;
    
    string tmp;
    for(int i = 0; i<3; i++){
        cin>>tmp;
        char ch = tmp[0];
        most_used_char.push_back(ch);
    }

    while(true){
        cin>>tmp;
        int len = tmp.length();
        string ins;
        if(tmp[len-1]==',') ins = tmp.substr(0, len-1);
        else ins = tmp;
        transform(ins.begin(), ins.end(), ins.begin(), ::tolower);
        words_confirmed.push_back(ins);
        if(tmp[len-1]!=',') break;
    }
    count_top_three();
    int words_count = words_confirmed.size();

    for(int i = 0; i<words_count; i++){
        decipher_by_word(words_confirmed[i]);
    }
    if(DEBUG){
        for(int i = 0; i<26; i++){
            cout<<char('A'+i)<<" is mapped to " << mapper[i]<< endl;
        }
    }

    cout<<"Plaintext message:\n";
    cout<<plain_text<<endl<<endl;
    //test if input received correctly
    /*
    cout<<cipher_text<<endl<<plain_text<<endl;
    for(int i = 0; i<most_used_char.size(); i++) cout<<most_used_char[i]<<' ';
    cout<<endl;

    for(int i = 0; i<words_confirmed.size(); i++) cout<<words_confirmed[i]<<' ';
    cout<<endl;
    */
    
    char plain_2_cipher_map[26];
    memset(plain_2_cipher_map, '#', sizeof(plain_2_cipher_map));
    for(int i = 0; i<26; i++) {
        plain_2_cipher_map[mapper[i]-'a'] = 'A'+i; 
    }
    if(DEBUG){
        for(int i = 0; i<26; i++) {
            cout<<plain_2_cipher_map[i]<<' '<<char('a'+i)<<endl;; 
        }
        cout<<endl;
    }


    cout<<"Mapping:\n";
    for(int i = 0; i<26; i++){
        cout<<char('a'+i)<<" = "<<plain_2_cipher_map[i]<<"\n";
    }
    cout<<endl;

    string reverse_cipher = "";
    int plen = plain_text.length();
    for(int i = 0; i<plen; i++){
        if(plain_text[i]<'a' || plain_text[i]>'z') reverse_cipher += '#';
        else reverse_cipher += plain_2_cipher_map[plain_text[i]-'a'];
    } 
    if(DEBUG){
        cout<<"ORIGINAL CIPHER TEXT: "<<cipher_text<<endl;
        cout<<"REVERSED CIPHER TEXT: "<<reverse_cipher<<endl;
    }
    int cnt = 0;
    for(int i = 0; i<plen;i++){
        if(cipher_text[i]==reverse_cipher[i]) {
            cnt++;
        }
    }
    if(DEBUG){
        cout<<cnt<<' '<<plen<<endl;
    }
    cout<<"REVERSED CIPHER TEXT: "<<reverse_cipher<<endl;
    double acc = (cnt*100.0)/plen;
    printf("Accuracy: %0.2lf", acc);
    cout<<"%"<<endl;
    return 0;
}
