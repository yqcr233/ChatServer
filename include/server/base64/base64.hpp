#ifndef BASE64_HPP
#define BASE64_HPP
#include <string>

/**
 * 将base64编码的数据还原为原二进制数据字符串
 */
std::string base64_decode(const std::string &encoded_string);

/**
 * 将原二进制数据字符串使用base64编码
 */
std::string base64_encode(const std::string &origin_string);

#endif