#include "rsa.hpp"
#include <fstream>
#include <sstream>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <iostream>

using namespace std;

RsaKeyManager::RsaKeyManager()
{
    if (!loadKeysFromFile())
    {
        cout << "load key file fail" << endl;
    }
}

RsaKeyManager::~RsaKeyManager()
{
    if (rsa_private)
        RSA_free(rsa_private);
    if (rsa_public)
        RSA_free(rsa_public);
}

std::string RsaKeyManager::decryptWithPrivateKey(const std::string &encryptedData)
{
    if (!rsa_private)
        return "";

    int rsa_size = RSA_size(rsa_private);
    unsigned char *decrypted = new unsigned char[rsa_size];

    int result = RSA_private_decrypt(encryptedData.size(),
                                     reinterpret_cast<const unsigned char *>(encryptedData.data()), decrypted, rsa_private, RSA_PKCS1_PADDING);
    string plaintext;

    if (result > 0)
    {
        plaintext.assign(reinterpret_cast<char *>(decrypted), result);
    }

    delete[] decrypted;
    return plaintext;
}

bool RsaKeyManager::loadKeysFromFile()
{
    string file_path = "private.pem";
    FILE *priv_file = fopen(file_path.c_str(), "r");
    if (priv_file)
    {
        rsa_private = PEM_read_RSAPrivateKey(priv_file, nullptr, nullptr, nullptr);
        if (!rsa_private)
        {
            ERR_print_errors_fp(stderr);
        }
        fclose(priv_file);
    }
    else
        return false;

    file_path = "public.pem";
    FILE *pub_file = fopen(file_path.c_str(), "r");
    if (pub_file)
    {
        /**
         * 这里公钥格式是以-----BEGIN PUBLIC KEY----- 开头的而不是-----BEGIN RSA PUBLIC KEY-----开头
         * 所以不能使用PEM_read_RSAPublicKey，而是PEM_read_RSA_PUBKEY
         */
        rsa_public = PEM_read_RSA_PUBKEY(pub_file, nullptr, nullptr, nullptr);
        if (!rsa_public)
        {
            ERR_print_errors_fp(stderr);
        }
        fclose(pub_file);
    }
    else
        return false;

    return true;
}
