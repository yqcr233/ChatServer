#include "base64.hpp"
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <vector>

std::string base64_decode(const std::string &encoded_string)
{
    BIO *b64 = BIO_new(BIO_f_base64());
    BIO *bio = BIO_new_mem_buf(encoded_string.data(), encoded_string.length());

    // 不添加换行符
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

    // 将 bio 和 b64 连接起来
    bio = BIO_push(b64, bio);

    // 计算解码后的大小
    // Base64 解码后的大小大约是原字符串的 3/4
    size_t decoded_length = (encoded_string.length() * 3) / 4;
    std::vector<unsigned char> buffer(decoded_length);

    // 解码
    int length = BIO_read(bio, buffer.data(), buffer.size());

    if (length <= 0)
    {
        BIO_free_all(bio);
        return "";
    }

    std::string result(reinterpret_cast<char *>(buffer.data()), length);
    BIO_free_all(bio);

    return result;
}

std::string base64_encode(const std::string &origin_string)
{
    BIO *b64 = BIO_new(BIO_f_base64());
    BIO *bio = BIO_new(BIO_s_mem());

    // 不添加换行符
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

    // 将 bio 和 b64 连接起来
    bio = BIO_push(b64, bio);

    // 编码
    BIO_write(bio, reinterpret_cast<const unsigned char *>(origin_string.data()), origin_string.length());
    BIO_flush(bio);

    // 获取编码后的数据
    BUF_MEM *buffer_ptr;
    BIO_get_mem_ptr(bio, &buffer_ptr);
    std::string result(buffer_ptr->data, buffer_ptr->length);

    BIO_free_all(bio);
    return result;
}
