#include <curl/curl.h>
#include <iomanip>
#include <sstream>

#include "tracker.h"
#include "bencode.h"




string Tracker::url_encode(string& s){
    ostringstream en;
    en.fill('0');
    en << hex;
    for(unsigned char c : s){
        if(isalnum(c) || c=='-' || c=='_' || c=='.' || c=='~'){
            en << c;
        }
        else{
            en << uppercase << "%" << setw(2) << int(c) << nouppercase;
        }
    }
    return en.str();
}

string Tracker::peer_id(){
    string id="-SWM037-";
    for(int i=0;i<12;i++){
        id+=to_string(rand()%10);
    }
    return id;
}

size_t Write_callback(char* p, size_t s, size_t nmeb, string* data){
    data->append(p,s*nmeb);
    return s*nmeb;
}

vector<Peer> Tracker::query(TorrentFile& t){
    vector<Peer> peers;

    string url="";
    url+=t.announce;
    url+="?info_hash="+url_encode(t.info_hash);
    url+="&peer_id="+peer_id();
    url+="&port=6881";
    url+="&uploaded=0";
    url+="&downloaded=0";
    url+="&left="+to_string(t.length);
    url+="&compact=1";

    string res="";
    CURL* c=curl_easy_init();

    if(!c){
        cout<<"c is nullptr";
        return vector<Peer>{};
    }
    curl_easy_setopt(c,CURLOPT_URL, url.c_str());
    curl_easy_setopt(c,CURLOPT_WRITEFUNCTION, Write_callback);
    curl_easy_setopt(c,CURLOPT_WRITEDATA, &res);

    curl_easy_perform(c);
    curl_easy_cleanup(c);

    // cout<<res<<"\n";
    cout<<"\nres size: "<<res.size()<<"\n";

    BencodeDecoder bd(res);
    BencodeVal result=bd.decoder();
    string peers_str=get<string>((get<map<string,BencodeVal>>(result.value).at("peers")).value);
    
    for(size_t i=0; i < peers_str.size(); i += 6) {
        
        const uint8_t* p = reinterpret_cast<const uint8_t*>(peers_str.data() + i);
        
        string ip = to_string(p[0]) + "." + to_string(p[1]) + "." + to_string(p[2]) + "." + to_string(p[3]);
        
        uint16_t port = (static_cast<uint16_t>(p[4]) << 8) | static_cast<uint16_t>(p[5]);
        
        peers.push_back({ip, port});
    }
    return peers;
}