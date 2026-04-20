#include "aesgcm/aesgcm.hpp"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <fstream>
#include <sstream>
#include <stdexcept>

std::string AesGcmManager::encrypt(const std::string &plaintext)
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
        throw std::runtime_error("创建上下文失败");

    // 生成随机IV
    std::string iv(IV_SIZE, 0);
    RAND_bytes(reinterpret_cast<unsigned char *>(&iv[0]), IV_SIZE);

    // 初始化加密上下文
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr,
                           reinterpret_cast<const unsigned char *>(key.c_str()),
                           reinterpret_cast<const unsigned char *>(iv.c_str())) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("加密初始化失败");
    }

    // 分配输出缓冲区
    std::string ciphertext(plaintext.size() + EVP_MAX_BLOCK_LENGTH, 0);
    int len = 0, ciphertext_len = 0;

    // 加密数据
    if (EVP_EncryptUpdate(ctx,
                          reinterpret_cast<unsigned char *>(&ciphertext[0]),
                          &len,
                          reinterpret_cast<const unsigned char *>(plaintext.c_str()),
                          plaintext.size()) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("加密失败");
    }
    ciphertext_len = len;

    // 完成加密
    if (EVP_EncryptFinal_ex(ctx,
                            reinterpret_cast<unsigned char *>(&ciphertext[ciphertext_len]),
                            &len) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("加密完成失败");
    }
    ciphertext_len += len;

    // 获取认证标签
    std::string tag(TAG_SIZE, 0);
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, TAG_SIZE,
                            reinterpret_cast<void *>(&tag[0])) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("获取认证标签失败");
    }

    EVP_CIPHER_CTX_free(ctx);

    // 返回格式: iv(12) + tag(16) + ciphertext
    return iv + tag + ciphertext.substr(0, ciphertext_len);
}

std::string AesGcmManager::decrypt(const std::string &encrypted)
{
    if (encrypted.size() < IV_SIZE + TAG_SIZE)
        throw std::runtime_error("加密数据太短");

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
        throw std::runtime_error("创建上下文失败");

    // 提取 IV 和 Tag
    std::string iv = encrypted.substr(0, IV_SIZE);
    std::string tag = encrypted.substr(IV_SIZE, TAG_SIZE);
    std::string ciphertext = encrypted.substr(IV_SIZE + TAG_SIZE);

    // 初始化解密上下文
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr,
                           reinterpret_cast<const unsigned char *>(key.c_str()),
                           reinterpret_cast<const unsigned char *>(iv.c_str())) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("解密初始化失败");
    }

    // 分配输出缓冲区
    std::string plaintext(ciphertext.size(), 0);
    int len = 0, plaintext_len = 0;

    // 解密数据
    if (EVP_DecryptUpdate(ctx,
                          reinterpret_cast<unsigned char *>(&plaintext[0]),
                          &len,
                          reinterpret_cast<const unsigned char *>(ciphertext.c_str()),
                          ciphertext.size()) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("解密失败");
    }
    plaintext_len = len;

    // 设置认证标签
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, TAG_SIZE,
                            reinterpret_cast<void *>(&tag[0])) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("设置认证标签失败");
    }

    // 完成解密（会验证标签）
    if (EVP_DecryptFinal_ex(ctx,
                            reinterpret_cast<unsigned char *>(&plaintext[plaintext_len]),
                            &len) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("认证标签验证失败 - 数据可能被篡改");
    }
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return plaintext.substr(0, plaintext_len);
}