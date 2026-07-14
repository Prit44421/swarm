#include "bencode.h"



BencodeVal BencodeDecoder::decoder(){
    if(data[pos]=='d'){
        return decode_dict();
    }
    else if(data[pos]=='l'){
        return decode_list();
    }
    else if(data[pos]=='i'){
        return decode_int();
    }
    else if(isdigit(data[pos])){
        return decode_str();
    }
    else{
        //todo error
        cout<<"ERROR\n";
    }
}

BencodeVal BencodeDecoder::decode_int(){
    pos++;
    string i="";
    while(data[pos]!='e'){
        i+=data[pos++];
    }
    pos++;
    return BencodeVal(static_cast<int64_t>(stoll(i)));
}

BencodeVal BencodeDecoder::decode_list(){
    pos++;
    vector<BencodeVal> v;
    while(data[pos]!='e'){
        v.push_back(decoder());
    }
    pos++;
    return BencodeVal(v);
}

BencodeVal BencodeDecoder::decode_str(){
    string l="";
    while(data[pos]!=':'){
        l+=data[pos++];
    }
    pos++;
    int length=stoi(l);
    string s=data.substr(pos,length);
    pos+=length;
    return BencodeVal(s);
}

BencodeVal BencodeDecoder::decode_dict(){
    pos++;
    map<string, BencodeVal> m;
    while(data[pos]!='e'){
        string k=get<string>(decode_str().value);
        BencodeVal v=decoder();
        m.insert_or_assign(k,v);
    }
    pos++;
    return BencodeVal(m);
}


string BencodeEncoder::encoder(BencodeVal & data){
    if(holds_alternative<int64_t>(data.value)){
        return "i"+to_string(get<int64_t>(data.value))+"e";
    }
    if(holds_alternative<string>(data.value)){
        return to_string((get<string>(data.value)).size())+":"+get<string>(data.value);
    }
    if(holds_alternative<vector<BencodeVal>>(data.value)){
        string s="l";
        for(auto i:get<vector<BencodeVal>>(data.value)){
            s+=encoder(i);
        }
        s+="e";
        return s;
    }
    if(holds_alternative<map<string,BencodeVal>>(data.value)){
        string s="d";
        for(auto i:get<map<string,BencodeVal>>(data.value)){
            s+=to_string(i.first.size())+":"+i.first;
            s+=encoder(i.second);
        }
        s+="e";
        return s;
    }

    return "";
}


