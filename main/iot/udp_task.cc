// udp_task.cpp
#include "udp_task.h"
#include <esp_log.h>
#include "iot/protocol.h"
#define TAG "UDP_TASK"
UdpTask::UdpTask(const std::string host, const std::string token, std::string command, std::string parameters)
    : m_udpHost(host), m_token(token), m_command(command), m_parameters(parameters), m_taskHandle(nullptr), m_running(false) {}

UdpTask::~UdpTask()
{
    stop();
}

void UdpTask::start()
{
    if (m_running)
        return;

    // 通过xTaskCreate创建任务，将this指针作为参数传递
    BaseType_t result = xTaskCreate(
        &UdpTask::taskFunction, // 静态函数
        "UDP_TASK",
        4096,
        this, // 传递this指针
        4,
        &m_taskHandle);

    if (result == pdPASS)
    {
        m_running = true;
        ESP_LOGI(TAG, "Task started");
    }
    else
    {
        ESP_LOGE(TAG, "Failed to create task!");
    }
}

void UdpTask::stop()
{
    ESP_LOGI(TAG, "Stopping task");
    if (udp_fd_ != -1)
    {
        close(udp_fd_);
        udp_fd_ = -1;
    }
    if (m_taskHandle != nullptr)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);

        vTaskDelete(m_taskHandle);
        m_taskHandle = nullptr;
        m_running = false;
        ESP_LOGI(TAG, "Task stopped");
    }
}

// 静态成员函数：将void*参数转换为类实例并调用run()
void UdpTask::taskFunction(void *arg)
{
    UdpTask *instance = static_cast<UdpTask *>(arg);
    instance->run();
}

// 实际任务逻辑（在类中实现）
void UdpTask::run()
{
    while (true)
    {
        // ESP_LOGI(TAG, "Task running");
        // ESP_LOGI(TAG, "Running on core %d", xPortGetCoreID());
        // ESP_LOGI(TAG, "Task running:%s", m_udpHost.c_str());
        if (udpConnect())
        {
            unsigned char helo_char[] = {
                0x21,
                0x31,
                0x00,
                0x20,
                0xff,
                0xff,
                0xff,
                0xff,
                0xff,
                0xff,
                0xff,
                0xff,
                0xff,
                0xff,
                0xff,
                0xff,
                0xff,
                0xff,
                0xff,
                0xff,
                0xff,
                0xff,
                0xff,
                0xff,
                0xff,
                0xff,
                0xff,
                0xff,
                0xff,
                0xff,
                0xff,
                0xff,
            };

            int err = ::send(udp_fd_, helo_char, sizeof(helo_char), 0);
            if (err < 0)
            {
                ESP_LOGE(TAG, "发送失败");
            }
            else
            {
                std::string handData;
                handData.resize(1500);
                int ret = recv(udp_fd_, handData.data(), handData.size(), 0);
                if (ret <= 0)
                {
                    ESP_LOGE(TAG, "接收失败");
                }
                else
                {
                    handData.resize(ret);
                    // ESP_LOGI(TAG, "接收到数据2：%s", handData.c_str());
                    iot::Message msg;
                    msg.parse(handData);

                    iot::Message response;
                    std::string request = createRequest(m_command, m_parameters);
                    ESP_LOGI(TAG, "send request:%s", request.data());
                    std::string build = msg.build(request, m_token);
                    int err = ::send(udp_fd_, build.c_str(), build.size(), 0);
                    if (err < 0)
                    {
                        ESP_LOGE(TAG, "发送失败");
                    }
                    else
                    {
                        std::string iotData;
                        iotData.resize(1500);
                        int ret = recv(udp_fd_, iotData.data(), iotData.size(), 0);
                        if (ret <= 0)
                        {
                            ESP_LOGE(TAG, "接收失败");
                        }
                        else
                        {
                            iotData.resize(ret);
                            // ESP_LOGI(TAG, "接收到数据：%s", iotData.c_str());
                            iot::Message msg2;
                            msg2.parse(iotData);
                            auto body = msg2.decrypt(m_token);
                            msg2.success = true;
                            // ESP_LOGI(TAG, "解密数据：%s", msg2.data.c_str());
                            if (m_callback)
                            {
                                m_callback(true, body);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            ESP_LOGE(TAG, "Failed to connect to %s:%d", m_udpHost.c_str(), port);
        }

        // std::string data;
        // data.resize(1500);
        // int ret = recv(udp_fd_, data.data(), data.size(), 0);
        // if (ret <= 0)
        // {
        //     ESP_LOGE(TAG, "接收失败");
        //     break;
        // }
        stop();
        // vTaskDelete(NULL);

        // 添加业务逻辑（如UDP处理）
        // if (should_stop) break; // 可添加退出条件
    }
}

bool UdpTask::udpConnect()
{
    // ESP_LOGI(TAG, "udp_fd_:%d", udp_fd_);
    if (udp_fd_ >= 0)
    {
        close(udp_fd_); // 关闭之前的套接字
        udp_fd_ = -1;   // 标记为无效
    }

    // ESP_LOGE(TAG, "run 1");
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    // host is domain
    // ESP_LOGE(TAG, "run 2");
    struct hostent *server = gethostbyname(m_udpHost.c_str());
    if (server == NULL)
    {
        ESP_LOGE(TAG, "Failed to get host by name");
        return false;
    }
    // ESP_LOGE(TAG, "run 3");
    memcpy(&server_addr.sin_addr, server->h_addr, server->h_length);
    // ESP_LOGE(TAG, "run 4");
    udp_fd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_fd_ < 0)
    {
        ESP_LOGE(TAG, "Failed to create socket");
        return false;
    }
    // ESP_LOGE(TAG, "run 5");
    int ret = connect(udp_fd_, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0)
    {
        ESP_LOGE(TAG, "Failed to connect to %s:%d", m_udpHost.c_str(), port);
        close(udp_fd_);
        udp_fd_ = -1;
        return false;
    }
    // ESP_LOGE(TAG, "run 6");

    connected_ = true;
    return true;
}

std::string UdpTask::createRequest(const std::string &command, const std::string &parameters)
{
    std::string request = "{\"id\": " + std::to_string(1) + ", \"method\": \"" + command + "\"";
    if (!parameters.empty())
    {
        request += ", \"params\": " + parameters;
    }
    else
    {
        request += ", \"params\": []";
    }
    request += "}";
    return request;
}
