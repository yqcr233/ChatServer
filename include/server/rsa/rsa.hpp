#ifndef RSA_HPP
#define RSA_HPP
#include <string>
#include <openssl/rsa.h>

class RsaKeyManager
{
public:
    RsaKeyManager();
    ~RsaKeyManager();

    std::string decryptWithPrivateKey(const std::string &encryptedData);

private:
    bool loadKeysFromFile();
    RSA *rsa_private = nullptr;
    RSA *rsa_public = nullptr;
};

#endif
