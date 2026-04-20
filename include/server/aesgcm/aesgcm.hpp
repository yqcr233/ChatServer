#ifndef AESGCM_HPP
#define AESGCM_HPP

#include <string>

class AesGcmManager
{
public:
    AesGcmManager(const std::string &key) { this->key = key; }
    ~AesGcmManager() {}

    std::string encrypt(const std::string &plaintext);
    std::string decrypt(const std::string &encrypted);

private:
    std::string key;
    static const int IV_SIZE = 12;  // 推荐12字节
    static const int TAG_SIZE = 16; // 128位认证标签
    static const int KEY_SIZE = 32; // 256位密钥
};

#endif