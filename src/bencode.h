#pragma once
// #include <memory>
#include <variant>
#include <vector>
#include <map>
#include <string>
#include <iostream>

using namespace std;

struct BencodeVal{
    using val=variant<int64_t,vector<BencodeVal>,map<string,BencodeVal>,string>;
    val value;
    BencodeVal(val v){
        value=v;
    }
    void print_bencode(){
        if(holds_alternative<int64_t>(value)){
            cout<<get<int64_t>(value);
        }
        else if(holds_alternative<vector<BencodeVal>>(value)){
            cout<<"[";
            for(auto& v:get<vector<BencodeVal>>(value)){
                v.print_bencode();
                cout<<",";
            }
            cout<<"]";
        }
        else if(holds_alternative<map<string,BencodeVal>>(value)){
            cout<<"{";
            for(auto& [k,v]:get<map<string,BencodeVal>>(value)){
                cout<<k<<":";
                v.print_bencode();
                cout<<",";
            }
            cout<<"}";
        }
        else if(holds_alternative<string>(value)){
            cout<<get<string>(value);
        }
    }
};


class BencodeDecoder{
    private:
        string data;
        size_t pos=0;

        BencodeVal decode_int();
        BencodeVal decode_list();
        BencodeVal decode_dict();
        BencodeVal decode_str();

    public:
        BencodeDecoder(string& d){
            data=d;
        }
        BencodeVal decoder();

};



class BencodeEncoder{
    private:
        // BencodeVal data;
        // size_t pos=0;

    public:

        // BencodeEncoder(BencodeVal& d){
        //     data=d;
        // }
        string encoder(BencodeVal& data);

};