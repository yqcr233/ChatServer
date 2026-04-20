-- MySQL dump 10.13  Distrib 8.0.42, for Linux (x86_64)
--
-- Host: localhost    Database: chat
-- ------------------------------------------------------
-- Server version	8.0.42-0ubuntu0.20.04.1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `OfflineMessage`
--

DROP TABLE IF EXISTS `OfflineMessage`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `OfflineMessage` (
  `userid` int NOT NULL COMMENT '用户id',
  `message` varchar(500) NOT NULL COMMENT '离线消息(存储Json字符串)'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='离线消息表';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `OfflineMessage`
--

LOCK TABLES `OfflineMessage` WRITE;
/*!40000 ALTER TABLE `OfflineMessage` DISABLE KEYS */;
/*!40000 ALTER TABLE `OfflineMessage` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `allgroup`
--

DROP TABLE IF EXISTS `allgroup`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `allgroup` (
  `id` int NOT NULL AUTO_INCREMENT COMMENT '组id',
  `groupname` varchar(50) NOT NULL COMMENT '组名称',
  `groupdesc` varchar(200) DEFAULT '' COMMENT '组功能描述',
  PRIMARY KEY (`id`),
  UNIQUE KEY `groupname` (`groupname`)
) ENGINE=InnoDB AUTO_INCREMENT=21 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='群组信息表';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `allgroup`
--

LOCK TABLES `allgroup` WRITE;
/*!40000 ALTER TABLE `allgroup` DISABLE KEYS */;
INSERT INTO `allgroup` VALUES (5,'first','first_group'),(15,'second','second_group'),(16,'third','third_group'),(17,'forth','forth_group'),(18,'lol_group',''),(19,'mmm','mmm'),(20,'lol group','xxx');
/*!40000 ALTER TABLE `allgroup` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `friendrequests`
--

DROP TABLE IF EXISTS `friendrequests`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `friendrequests` (
  `id` int NOT NULL AUTO_INCREMENT COMMENT '好友请求唯一标识ID',
  `fromid` int NOT NULL COMMENT '发送请求的用户ID',
  `toid` int NOT NULL COMMENT '接收请求的用户ID',
  `message` varchar(255) COLLATE utf8mb4_unicode_ci DEFAULT NULL COMMENT '请求附带的消息，可以为空',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=18 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='好友请求表';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `friendrequests`
--

LOCK TABLES `friendrequests` WRITE;
/*!40000 ALTER TABLE `friendrequests` DISABLE KEYS */;
INSERT INTO `friendrequests` VALUES (8,1,11,'你好，我想添加你为好友'),(17,1,11,'你好，我想添加你为好友');
/*!40000 ALTER TABLE `friendrequests` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `friends`
--

DROP TABLE IF EXISTS `friends`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `friends` (
  `userid` int NOT NULL COMMENT '用户id',
  `friendid` int NOT NULL COMMENT '好友id',
  PRIMARY KEY (`userid`,`friendid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='好友关系表';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `friends`
--

LOCK TABLES `friends` WRITE;
/*!40000 ALTER TABLE `friends` DISABLE KEYS */;
INSERT INTO `friends` VALUES (1,2),(1,3),(1,4),(1,6),(1,7),(1,8),(1,9),(1,10),(2,1),(2,10),(3,1),(4,1),(6,1),(7,1),(8,1),(9,1),(10,1),(10,2);
/*!40000 ALTER TABLE `friends` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `groupuser`
--

DROP TABLE IF EXISTS `groupuser`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `groupuser` (
  `groupid` int NOT NULL COMMENT '组id',
  `userid` int NOT NULL COMMENT '组员id',
  `grouprole` enum('creator','normal') DEFAULT 'normal' COMMENT '组内角色',
  PRIMARY KEY (`groupid`,`userid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='群组成员表';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `groupuser`
--

LOCK TABLES `groupuser` WRITE;
/*!40000 ALTER TABLE `groupuser` DISABLE KEYS */;
INSERT INTO `groupuser` VALUES (5,1,'creator'),(5,2,'normal'),(5,3,'normal'),(5,4,'normal'),(5,5,'normal'),(15,1,'creator'),(15,2,'normal'),(15,3,'normal'),(15,4,'normal'),(16,1,'creator'),(16,2,'normal'),(16,7,'normal'),(16,8,'normal'),(17,1,'creator'),(17,6,'normal'),(17,7,'normal'),(17,8,'normal'),(18,1,'creator'),(18,2,'normal'),(18,6,'normal'),(18,10,'normal'),(19,1,'creator'),(19,2,'normal'),(19,4,'normal'),(19,9,'normal'),(20,1,'creator'),(20,2,'normal'),(20,4,'normal'),(20,6,'normal'),(20,7,'normal');
/*!40000 ALTER TABLE `groupuser` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `message`
--

DROP TABLE IF EXISTS `message`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `message` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT COMMENT '消息记录ID',
  `to_id` int NOT NULL COMMENT '接收者用户id,群聊时为-1',
  `from_id` int NOT NULL COMMENT '发送者ID',
  `group_id` int NOT NULL DEFAULT '-1' COMMENT '群聊ID，-1表示无群组（私聊）',
  `content` text COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '消息内容',
  `state` int NOT NULL COMMENT '消息状态，0为离线',
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '消息创建时间',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=239 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `message`
--

LOCK TABLES `message` WRITE;
/*!40000 ALTER TABLE `message` DISABLE KEYS */;
INSERT INTO `message` VALUES (179,2,1,-1,'开始聊天吧~',1,'2026-04-02 15:09:28'),(180,3,1,-1,'开始聊天吧~',1,'2026-04-02 15:09:30'),(181,4,1,-1,'开始聊天吧~',1,'2026-04-02 15:09:30'),(182,6,1,-1,'开始聊天吧~',1,'2026-04-02 15:09:30'),(183,8,1,-1,'开始聊天吧~',1,'2026-04-02 15:09:31'),(184,9,1,-1,'开始聊天吧~',1,'2026-04-02 15:09:31'),(185,10,1,-1,'开始聊天吧~',1,'2026-04-02 15:09:32'),(186,10,2,-1,'开始聊天吧~',1,'2026-04-02 15:11:08'),(187,2,1,-1,'nihao',1,'2026-04-02 15:11:20'),(188,1,2,-1,'nihao',1,'2026-04-02 15:11:30'),(189,2,1,-1,'meishil',1,'2026-04-02 15:11:40'),(190,1,2,-1,'汉字',1,'2026-04-02 15:11:49'),(193,2,1,-1,'Text {\n                                        text: lastMessage\n                                        color: \"#666\"\n                                        width: parent.width - 40\n                                        font.pixelSize: 12\n                                        Layout.fillWidth: true\n                                        elide: Text.ElideRight\n                                        clip: true\n                                    }',1,'2026-04-02 15:49:39'),(194,2,1,-1,'// 消息气泡\n                                    Rectangle {\n                                        id: messageBubble\n                                        width: Math.min(messageContent.implicitWidth + 20, parent.parent.width * 0.7)\n                                        height: messageContent.implicitHeight + 20\n                                        radius: 10\n                                        color: isMe ? \"#4a6cf7\" : \"#f0f0f0\"\n\n                                        // 消息内容\n                                        Text {\n                                            id: messageContent\n                                            text: content\n                                            color: isMe ? \"white\" : \"black\"\n                                            wrapMode: Text.Wrap\n                                            width: parent.width - 20\n                                            anchors.centerIn: parent\n                                        }\n                                    }\n\n                                    // 时间',1,'2026-04-02 16:09:30'),(195,1,2,-1,'nigao',1,'2026-04-02 16:12:28'),(196,1,2,-1,'void Chatservice::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)\n{\n    /**\n     * msgid, fromid, frommsg, toid, createat, sessiontype\n     * 转发内容一致\n     */\n    int toid = js[\"toid\"].get<int>();\n    int fromid = js[\"fromid\"].get<int>();\n    string frommsg = js[\"frommsg\"];\n    int64_t createat = js[\"createat\"].get<int64_t>();\n    User user = _userModel.query(toid);\n\n    // 插入历史消息\n    _messageModel.insert(toid, fromid, -1, frommsg, 1, createat);\n\n    LOG_INFO << toid << \" \" << fromid << \" \" << user.getState();',1,'2026-04-02 16:13:46'),(197,2,1,-1,'void Chatservice::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)\n{\n    /**\n     * msgid, fromid, frommsg, toid, createat, sessiontype\n     * 转发内容一致\n     */\n    int toid = js[\"toid\"].get<int>();\n    int fromid = js[\"fromid\"].get<int>();\n    string frommsg = js[\"frommsg\"];\n    int64_t createat = js[\"createat\"].get<int64_t>();\n    User user = _userModel.query(toid);\n\n    // 插入历史消息\n    _messageModel.insert(toid, fromid, -1, frommsg, 1, createat);\n\n    LOG_INFO << toid << \" \" << fromid << \" \" << user.getState();',1,'2026-04-02 16:14:19'),(198,1,2,-1,'void Chatservice::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)\n{\n    /**\n     * msgid, fromid, frommsg, toid, createat, sessiontype\n     * 转发内容一致\n     */\n    int toid = js[\"toid\"].get<int>();\n    int fromid = js[\"fromid\"].get<int>();\n    string frommsg = js[\"frommsg\"];\n    int64_t createat = js[\"createat\"].get<int64_t>();\n    User user = _userModel.query(toid);\n\n    // 插入历史消息\n    _messageModel.insert(toid, fromid, -1, frommsg, 1, createat);\n\n    LOG_INFO << toid << \" \" << fromid << \" \" << user.getState();',1,'2026-04-02 16:14:27'),(199,2,1,-1,'void Chatservice::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)\n{\n    /**\n     * msgid, fromid, frommsg, toid, createat, sessiontype\n     * 转发内容一致\n     */\n    int toid = js[\"toid\"].get<int>();\n    int fromid = js[\"fromid\"].get<int>();\n    string frommsg = js[\"frommsg\"];\n    int64_t createat = js[\"createat\"].get<int64_t>();\n    User user = _userModel.query(toid);\n\n    // 插入历史消息\n    _messageModel.insert(toid, fromid, -1, frommsg, 1, createat);\n\n    LOG_INFO << toid << \" \" << fromid << \" \" << user.getState();',1,'2026-04-02 16:14:31'),(200,2,1,-1,'void Chatservice::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)\n{\n    /**\n     * msgid, fromid, frommsg, toid, createat, sessiontype\n     * 转发内容一致\n     */\n    int toid = js[\"toid\"].get<int>();\n    int fromid = js[\"fromid\"].get<int>();\n    string frommsg = js[\"frommsg\"];\n    int64_t createat = js[\"createat\"].get<int64_t>();\n    User user = _userModel.query(toid);\n\n    // 插入历史消息\n    _messageModel.insert(toid, fromid, -1, frommsg, 1, createat);\n\n    LOG_INFO << toid << \" \" << fromid << \" \" << user.getState();',1,'2026-04-02 16:14:39'),(201,1,2,-1,'void Chatservice::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)\n{\n    /**\n     * msgid, fromid, frommsg, toid, createat, sessiontype\n     * 转发内容一致\n     */\n    int toid = js[\"toid\"].get<int>();\n    int fromid = js[\"fromid\"].get<int>();\n    string frommsg = js[\"frommsg\"];\n    int64_t createat = js[\"createat\"].get<int64_t>();\n    User user = _userModel.query(toid);\n\n    // 插入历史消息\n    _messageModel.insert(toid, fromid, -1, frommsg, 1, createat);\n\n    LOG_INFO << toid << \" \" << fromid << \" \" << user.getState();',1,'2026-04-02 16:14:42'),(202,2,1,-1,'void Chatservice::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)\n{\n    /**\n     * msgid, fromid, frommsg, toid, createat, sessiontype\n     * 转发内容一致\n     */\n    int toid = js[\"toid\"].get<int>();\n    int fromid = js[\"fromid\"].get<int>();\n    string frommsg = js[\"frommsg\"];\n    int64_t createat = js[\"createat\"].get<int64_t>();\n    User user = _userModel.query(toid);\n\n    // 插入历史消息\n    _messageModel.insert(toid, fromid, -1, frommsg, 1, createat);\n\n    LOG_INFO << toid << \" \" << fromid << \" \" << user.getState();',1,'2026-04-02 16:14:45'),(203,1,2,-1,'void Chatservice::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)\n{\n    /**\n     * msgid, fromid, frommsg, toid, createat, sessiontype\n     * 转发内容一致\n     */\n    int toid = js[\"toid\"].get<int>();\n    int fromid = js[\"fromid\"].get<int>();\n    string frommsg = js[\"frommsg\"];\n    int64_t createat = js[\"createat\"].get<int64_t>();\n    User user = _userModel.query(toid);\n\n    // 插入历史消息\n    _messageModel.insert(toid, fromid, -1, frommsg, 1, createat);\n\n    LOG_INFO << toid << \" \" << fromid << \" \" << user.getState();',1,'2026-04-02 16:14:48'),(204,2,1,-1,'void Chatservice::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)\n{\n    /**\n     * msgid, fromid, frommsg, toid, createat, sessiontype\n     * 转发内容一致\n     */\n    int toid = js[\"toid\"].get<int>();\n    int fromid = js[\"fromid\"].get<int>();\n    string frommsg = js[\"frommsg\"];\n    int64_t createat = js[\"createat\"].get<int64_t>();\n    User user = _userModel.query(toid);\n\n    // 插入历史消息\n    _messageModel.insert(toid, fromid, -1, frommsg, 1, createat);\n\n    LOG_INFO << toid << \" \" << fromid << \" \" << user.getState();',1,'2026-04-02 16:14:51'),(205,1,2,-1,'void Chatservice::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)\n{\n    /**\n     * msgid, fromid, frommsg, toid, createat, sessiontype\n     * 转发内容一致\n     */\n    int toid = js[\"toid\"].get<int>();\n    int fromid = js[\"fromid\"].get<int>();\n    string frommsg = js[\"frommsg\"];\n    int64_t createat = js[\"createat\"].get<int64_t>();\n    User user = _userModel.query(toid);\n\n    // 插入历史消息\n    _messageModel.insert(toid, fromid, -1, frommsg, 1, createat);\n\n    LOG_INFO << toid << \" \" << fromid << \" \" << user.getState();',1,'2026-04-02 16:14:53'),(206,2,1,-1,'void Chatservice::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)\n{\n    /**\n     * msgid, fromid, frommsg, toid, createat, sessiontype\n     * 转发内容一致\n     */\n    int toid = js[\"toid\"].get<int>();\n    int fromid = js[\"fromid\"].get<int>();\n    string frommsg = js[\"frommsg\"];\n    int64_t createat = js[\"createat\"].get<int64_t>();\n    User user = _userModel.query(toid);\n\n    // 插入历史消息\n    _messageModel.insert(toid, fromid, -1, frommsg, 1, createat);\n\n    LOG_INFO << toid << \" \" << fromid << \" \" << user.getState();',1,'2026-04-02 16:14:56'),(207,2,1,-1,'void Chatservice::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)\n{\n    /**\n     * msgid, fromid, frommsg, toid, createat, sessiontype\n     * 转发内容一致\n     */\n    int toid = js[\"toid\"].get<int>();\n    int fromid = js[\"fromid\"].get<int>();\n    string frommsg = js[\"frommsg\"];\n    int64_t createat = js[\"createat\"].get<int64_t>();\n    User user = _userModel.query(toid);\n\n    // 插入历史消息\n    _messageModel.insert(toid, fromid, -1, frommsg, 1, createat);\n\n    LOG_INFO << toid << \" \" << fromid << \" \" << user.getState();',1,'2026-04-02 16:15:03'),(208,2,1,-1,'void Chatservice::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)\n{\n    /**\n     * msgid, fromid, frommsg, toid, createat, sessiontype\n     * 转发内容一致\n     */\n    int toid = js[\"toid\"].get<int>();\n    int fromid = js[\"fromid\"].get<int>();\n    string frommsg = js[\"frommsg\"];\n    int64_t createat = js[\"createat\"].get<int64_t>();\n    User user = _userModel.query(toid);\n\n    // 插入历史消息\n    _messageModel.insert(toid, fromid, -1, frommsg, 1, createat);\n\n    LOG_INFO << toid << \" \" << fromid << \" \" << user.getState();',1,'2026-04-02 16:15:05'),(209,2,1,-1,'void Chatservice::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)\n{\n    /**\n     * msgid, fromid, frommsg, toid, createat, sessiontype\n     * 转发内容一致\n     */\n    int toid = js[\"toid\"].get<int>();\n    int fromid = js[\"fromid\"].get<int>();\n    string frommsg = js[\"frommsg\"];\n    int64_t createat = js[\"createat\"].get<int64_t>();\n    User user = _userModel.query(toid);\n\n    // 插入历史消息\n    _messageModel.insert(toid, fromid, -1, frommsg, 1, createat);\n\n    LOG_INFO << toid << \" \" << fromid << \" \" << user.getState();',1,'2026-04-02 16:15:06'),(210,2,1,-1,'void Chatservice::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)\n{\n    /**\n     * msgid, fromid, frommsg, toid, createat, sessiontype\n     * 转发内容一致\n     */\n    int toid = js[\"toid\"].get<int>();\n    int fromid = js[\"fromid\"].get<int>();\n    string frommsg = js[\"frommsg\"];\n    int64_t createat = js[\"createat\"].get<int64_t>();\n    User user = _userModel.query(toid);\n\n    // 插入历史消息\n    _messageModel.insert(toid, fromid, -1, frommsg, 1, createat);\n\n    LOG_INFO << toid << \" \" << fromid << \" \" << user.getState();',1,'2026-04-02 16:15:08'),(211,1,2,-1,'void Chatservice::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)\n{\n    /**\n     * msgid, fromid, frommsg, toid, createat, sessiontype\n     * 转发内容一致\n     */\n    int toid = js[\"toid\"].get<int>();\n    int fromid = js[\"fromid\"].get<int>();\n    string frommsg = js[\"frommsg\"];\n    int64_t createat = js[\"createat\"].get<int64_t>();\n    User user = _userModel.query(toid);\n\n    // 插入历史消息\n    _messageModel.insert(toid, fromid, -1, frommsg, 1, createat);\n\n    LOG_INFO << toid << \" \" << fromid << \" \" << user.getState();',1,'2026-04-02 16:15:13'),(212,1,2,-1,'void Chatservice::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)\n{\n    /**\n     * msgid, fromid, frommsg, toid, createat, sessiontype\n     * 转发内容一致\n     */\n    int toid = js[\"toid\"].get<int>();\n    int fromid = js[\"fromid\"].get<int>();\n    string frommsg = js[\"frommsg\"];\n    int64_t createat = js[\"createat\"].get<int64_t>();\n    User user = _userModel.query(toid);\n\n    // 插入历史消息\n    _messageModel.insert(toid, fromid, -1, frommsg, 1, createat);\n\n    LOG_INFO << toid << \" \" << fromid << \" \" << user.getState();',1,'2026-04-02 16:15:15'),(213,1,2,-1,'void Chatservice::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)\n{\n    /**\n     * msgid, fromid, frommsg, toid, createat, sessiontype\n     * 转发内容一致\n     */\n    int toid = js[\"toid\"].get<int>();\n    int fromid = js[\"fromid\"].get<int>();\n    string frommsg = js[\"frommsg\"];\n    int64_t createat = js[\"createat\"].get<int64_t>();\n    User user = _userModel.query(toid);\n\n    // 插入历史消息\n    _messageModel.insert(toid, fromid, -1, frommsg, 1, createat);\n\n    LOG_INFO << toid << \" \" << fromid << \" \" << user.getState();',1,'2026-04-02 16:15:16'),(214,1,2,-1,'void Chatservice::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)\n{\n    /**\n     * msgid, fromid, frommsg, toid, createat, sessiontype\n     * 转发内容一致\n     */\n    int toid = js[\"toid\"].get<int>();\n    int fromid = js[\"fromid\"].get<int>();\n    string frommsg = js[\"frommsg\"];\n    int64_t createat = js[\"createat\"].get<int64_t>();\n    User user = _userModel.query(toid);\n\n    // 插入历史消息\n    _messageModel.insert(toid, fromid, -1, frommsg, 1, createat);\n\n    LOG_INFO << toid << \" \" << fromid << \" \" << user.getState();',1,'2026-04-02 16:15:17'),(215,1,2,-1,'void Chatservice::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)\n{\n    /**\n     * msgid, fromid, frommsg, toid, createat, sessiontype\n     * 转发内容一致\n     */\n    int toid = js[\"toid\"].get<int>();\n    int fromid = js[\"fromid\"].get<int>();\n    string frommsg = js[\"frommsg\"];\n    int64_t createat = js[\"createat\"].get<int64_t>();\n    User user = _userModel.query(toid);\n\n    // 插入历史消息\n    _messageModel.insert(toid, fromid, -1, frommsg, 1, createat);\n\n    LOG_INFO << toid << \" \" << fromid << \" \" << user.getState();',1,'2026-04-02 16:15:19'),(216,7,1,-1,'开始聊天吧~',1,'2026-04-03 05:18:05'),(217,2,1,-1,'hi',1,'2026-04-03 08:36:46'),(218,2,1,-1,'nihao',1,'2026-04-03 08:36:50'),(219,1,2,-1,'nihao',1,'2026-04-03 08:36:54'),(220,-1,1,20,'hi',1,'2026-04-08 08:13:02'),(221,-1,2,20,'hi bro',1,'2026-04-08 08:13:22'),(222,2,1,-1,'你吃饭了吗',1,'2026-04-10 14:20:42'),(223,2,1,-1,'good night',1,'2026-04-13 15:02:34'),(224,1,2,-1,'you too',1,'2026-04-13 15:02:59'),(225,-1,1,20,'nihao',1,'2026-04-14 09:23:57'),(226,1,2,-1,'jiami',1,'2026-04-16 16:33:41'),(227,2,1,-1,'decode',1,'2026-04-16 16:33:48'),(228,1,2,-1,'encode',1,'2026-04-16 16:33:54'),(229,1,2,-1,'这是加密后的消息',1,'2026-04-16 16:34:04'),(230,2,1,-1,'接收到消息',1,'2026-04-16 16:34:11'),(231,-1,2,20,'群聊加密消息',1,'2026-04-16 16:34:25'),(232,-1,1,20,'收到消息',1,'2026-04-16 16:34:31'),(233,1,2,-1,'晚上好',1,'2026-04-19 16:49:14'),(234,2,1,-1,'kkk',1,'2026-04-19 17:01:03'),(235,1,2,-1,'lll',1,'2026-04-19 17:01:06'),(236,-1,1,20,'ppp',1,'2026-04-19 17:01:37'),(237,1,2,-1,'hihao',1,'2026-04-20 15:04:28'),(238,2,1,-1,'nihao',1,'2026-04-20 15:04:36');
/*!40000 ALTER TABLE `message` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `user`
--

DROP TABLE IF EXISTS `user`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `user` (
  `id` int NOT NULL AUTO_INCREMENT COMMENT '用户id',
  `name` varchar(50) NOT NULL COMMENT '用户名',
  `password` varchar(50) NOT NULL COMMENT '用户密码',
  `state` enum('online','offline') DEFAULT 'offline' COMMENT '当前登录状态',
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=InnoDB AUTO_INCREMENT=13 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='用户表';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `user`
--

LOCK TABLES `user` WRITE;
/*!40000 ALTER TABLE `user` DISABLE KEYS */;
INSERT INTO `user` VALUES (1,'fjf','12345','offline'),(2,'jfj','54321','offline'),(3,'txdy','02468','offline'),(4,'yqcr','86420','offline'),(5,'yqcr666','666666','offline'),(6,'fff','111111','offline'),(7,'jjj','222222','offline'),(8,'aaa','333333','offline'),(9,'bbb','444444','offline'),(10,'EEE','555555','offline'),(11,'kkk','888888','offline'),(12,'ooo','999999','offline');
/*!40000 ALTER TABLE `user` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2026-04-21  0:06:43
