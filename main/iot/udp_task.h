#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "string"
#include <esp_log.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <functional>
class UdpTask
{
public:
    UdpTask(std::string m_udpHost, std::string m_token, std::string m_command, std::string parameters);
    ~UdpTask();

    void start(); // 启动任务
    void stop();  // 停止任务
    using ResultCallback = std::function<void(bool success, const std::string &response)>;
    void setCallback(ResultCallback clallback)
    {
        m_callback = clallback;
    };

private:
    static void taskFunction(void *arg); // 静态任务函数
    void run();                          // 实际任务逻辑
    bool udpConnect();

    std::string createRequest(const std::string &command, const std::string &parameters);
    int port = 54321;
    int udp_fd_;
    bool connected_ = false;
    std::string m_udpHost;
    std::string m_token;
    std::string m_command;
    std::string m_parameters;
    TaskHandle_t m_taskHandle = nullptr; // 任务句柄
    uint32_t m_stackSize;
    UBaseType_t m_priority;
    bool m_running = false;
    ResultCallback m_callback = nullptr;
};