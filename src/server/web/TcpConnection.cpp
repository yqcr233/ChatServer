#include "tcp_connection.hpp"
#include "logger.hpp"
#include "socket_.hpp"
#include "channel.hpp"
#include "event_loop.hpp"
#include <sys/sendfile.h>

// 检查主loop是否为空
static EventLoop *CheckLoopNotNull(EventLoop *loop)
{
    if (loop == nullptr)
    {
        LOG_FATAL("%s:%s:%d mainLoop is null!\n", __FILE__, __FUNCTION__, __LINE__);
    }
    return loop;
}

TcpConnection::TcpConnection(EventLoop *loop, const string &name, int sockfd, const InetAddress &localAddr, const InetAddress &peerAddr) : _loop(CheckLoopNotNull(loop)), _name(name), _state(kConnecting), _reading(true), _socket(new Socket(sockfd)),
                                                                                                                                           _channel(new Channel(loop, sockfd)), _localAddr(localAddr), _peerAddr(peerAddr),
                                                                                                                                           _highWaterMask(64 * 1024 * 1024) // 64M
{
    _channel->setReadCallback(std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
    _channel->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    _channel->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    _channel->setErrorCallback(std::bind(&TcpConnection::handleError, this));

    LOG_INFO("TcpConnection::ctor[%s] at fd=%d\n", _name.c_str(), sockfd);
    _socket->setKeepAlive(true);
}

TcpConnection::~TcpConnection()
{
    LOG_INFO("TcpConnection::dtor[%s] at fd=%d state=%d\n", _name.c_str(), _channel->fd(), (int)_state);
}

void TcpConnection::send(const string &buf)
{
    if (_state == kConnected)
    {
        if (_loop->isInLoopThread())
        {
            fprintf(stdout, "在工作线程中直接执行send\n");
            sendInLoop(buf.c_str(), buf.size());
        }
        else
        {
            fprintf(stdout, "不在工作线程中，先放入工作队列后再执行\n");
            // _loop->runInLoop(std::bind(&TcpConnection::sendInLoop, this, buf.c_str(), buf.size()));
            _loop->runInLoop([this, data = string(buf)] {
                sendInLoop(data.data(), data.size());
            });
        }
    }
}

void TcpConnection::sendInLoop(const void *data, size_t len)
{
    ssize_t nwrote;
    size_t remaining = len;
    bool faultError = false;

    /**
     * tcpconnection已经通过shutdown关闭写
     */
    if (_state == kDisconnected)
    {
        LOG_ERROR("disconnected, give up writing");
    }

    /**
     *  当channe第一次发送数据或缓冲区没有数据时手动执行write
     */
    if (!_channel->isWriting() && _outputBuffer.readableBytes() == 0)
    {
        fprintf(stdout, "sendInLoop中发送消息： \n");
        if (len > 4) {
            uint32_t msg_len = 0;
            memcpy(&msg_len, data, 4); // 读取长度字段
            msg_len = ntohl(msg_len);  // 从网络字节序转换为主机字节序

            fprintf(stdout, "消息长度字段 (网络字节序): %u\n", msg_len);
            fprintf(stdout, "消息体 (JSON): %.*s\n", (int)(len - 4), (const char*)data + 4);
        } else {
            fprintf(stdout, "数据过短，无法解析\n");
        }
        nwrote = write(_channel->fd(), data, len);
        fprintf(stdout, "消息一次性发送完毕\n");
        if (nwrote)
        {
            /**
             * 成功写入文件描述符
             */
            remaining = len - nwrote;

            if (remaining == 0 && _writeCompleteCallback)
            {
                /**
                 * 这里将_writeCompleteCallback放入等待队列是为了避免回调函数妨碍当前sendinloop函数的调用
                 */
                _loop->queueInLLoop(bind(_writeCompleteCallback, shared_from_this()));
            }
        }
        else
        {
            nwrote = 0;
            /**
             * EWOULDBLOCK是非阻塞io在文件描述符缓冲区满后无法写入时的正常返回，相当于EAGAIN
             */
            if (errno != EWOULDBLOCK)
            {
                LOG_ERROR("TcpConnection::sendInLoop");
                if (errno == EPIPE || errno == ECONNRESET) // SIGPIPE RESET
                {
                    faultError = true;
                }
            }
        }
    }

    /**
     * 数据没有一次性发送完，剩余的需要放入缓冲区中，并注册channel的写事件，
     * 实时监控写出文件描述符缓冲区是否有空间。
     */
    if (!faultError && remaining > 0)
    {
        size_t oldLen = _outputBuffer.readableBytes(); // 目前缓冲区剩余未发数据长度
        if (oldLen + remaining >= _highWaterMask && oldLen < _highWaterMask && _highWaterMarkCallback)
        {
            _loop->queueInLLoop(bind(_highWaterMarkCallback, shared_from_this(), oldLen + remaining));
        }
        _outputBuffer.append((char *)data + nwrote, remaining); // 将剩余数据放入缓冲区中
        if (!_channel->isWriting())
        { // 注册channel的写事件监听
            _channel->enableWriting();
        }
    }
}

/**
 * 关闭半连接
 */
void TcpConnection::shutdown()
{
    if (_state == kConnected)
    {
        setState(kDisconnected);
        _loop->runInLoop(bind(&TcpConnection::shutdownInLoop, this));
    }
}

void TcpConnection::shutdownInLoop()
{
    /**
     * 说明当前outputBuffer的数据全部向外发送完成，
     * 如果当前写事件未结束，则延迟等待写事件结束，
     * 并在handleWrite函数中调用shutdownInLoop关闭半连接
     */
    if (!_channel->isWriting())
    {
        _socket->shutdownWrite();
    }
}

/**
 * 链接的创建和销毁，都需要调用server中关于连接的回调函数
 */
void TcpConnection::connectEstablished()
{
    setState(kConnected);
    /**
     * channel持有上级connection指针引用，
     * 避免在执行回调函数时，上级被释放而引发未定义行为
     */
    _channel->tie(shared_from_this());
    _channel->enableReading(); // 开始只监听读入，因为写事件在文件描述符缓冲区空闲时会一直触发

    _connectionCallback(shared_from_this()); // 执行server中连接回调
}

void TcpConnection::connectDestroyed()
{
    if (_state == kConnected)
    {
        setState(kDisconnected);
        _channel->disableAll();
        _connectionCallback(shared_from_this());
    }
    _channel->remove();
}

void TcpConnection::handleRead(TimeStamp time)
{
    int saveErrno = 0;
    ssize_t n = _inputBuffer.readFd(_channel->fd(), &saveErrno);
    if (n > 0)
    {
        _messageCallback(shared_from_this(), &_inputBuffer, time);
    }
    else if (n == 0)
    {
        handleClose();
    }
    else
    {
        errno = saveErrno;
        LOG_ERROR("TcpConnection::handleRead");
        handleError();
    }
}

void TcpConnection::handleWrite()
{
    if (_channel->isWriting())
    {
        int saveErrno = 0;
        ssize_t n = _outputBuffer.writeFd(_channel->fd(), &saveErrno);
        if (n > 0)
        {
            _outputBuffer.retrieve(n);
            if (_outputBuffer.readableBytes() == 0)
            {
                /**
                 * 数据已经彻底写出完毕
                 */
                _channel->disableWriting();
                if (_writeCompleteCallback)
                {
                    _loop->queueInLLoop(bind(_writeCompleteCallback, shared_from_this()));
                }
                if (_state == kDisconnected)
                {
                    /**
                     * 延迟等待数据全部发送完毕后关闭半连接
                     */
                    shutdownInLoop();
                }
            }
        }
        else
        {
            LOG_ERROR("TcpConnection::handleWrite");
            handleError();
        }
    }
    else
    {
        LOG_ERROR("TcpConnection fd=%d is down, no more writing", _channel->fd());
    }
}

void TcpConnection::handleClose()
{
    LOG_INFO("TcpConnection::handleClose fd=%d state=%d\n", _channel->fd(), (int)_state);
    setState(kDisconnected);
    _channel->disableAll();

    // 通知server，执行回调
    _connectionCallback(shared_from_this()); // 连接回调
    _closeCallback(shared_from_this());      // remove掉server中的连接
}

void TcpConnection::handleError()
{
    int optval;
    socklen_t optlen = sizeof(optval);
    int err = 0;
    /**
     * 获取套接字上发生的具体错误，
     * errno虽然是线程局部的，并且监听错误，但是可能在多个系统调用间被修改
     * SO_ERROR返回的是这个socket上挂起的错误
     */
    if (getsockopt(_channel->fd(), SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
    {
        err = errno;
    }
    else
    {
        err = optval;
    }
    LOG_ERROR("TcpConnection::handleError name:%s - SO_ERROR:%d\n", _name.c_str(), err);
}