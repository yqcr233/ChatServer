# 定义了一个SRC_LIST变量，包含该目录下的所有源文件
aux_source_directory(. SRC_LIST)
aux_source_directory(./db DB_LIST)
aux_source_directory(./rsa RSA_LIST)
aux_source_directory(./base64 BASE64_LIST)
aux_source_directory(./aesgcm AESGCM_LIST)
aux_source_directory(./web WEB_LIST)
aux_source_directory(./redis REDIS_LIST)

find_package(OpenSSL REQUIRED)

# 指定生成可执行文件
add_executable(ChatServer ${SRC_LIST} ${DB_LIST} ${RSA_LIST} ${BASE64_LIST} ${AESGCM_LIST} ${WEB_LIST} ${REDIS_LIST})

# 指定可执行文件链接时需要的库文件
# target_link_libraries(ChatServer muduo_net muduo_base mysqlclient pthread OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(ChatServer mysqlclient pthread OpenSSL::SSL OpenSSL::Crypto hiredis)
