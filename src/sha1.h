#include <openssl/sha.h>
#include <string>
using namespace std;

string sha1_hash(string & data){
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(data.data()), data.size(), hash);
    return string(reinterpret_cast<char*>(hash), SHA_DIGEST_LENGTH);
}