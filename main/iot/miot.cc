#include <iot/miot.h>
#include <iot/protocol.h>
#include "miot.h"
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/task.h>
#include <projdefs.h>
#include <esp_log.h>
#include <lwip/sockets.h>
#include <netdb.h>
#define TAG "Miot"
namespace iot
{

    Miot::Miot(const std::string &ip, const std::string &token)
    {
        ip_ = ip;
        token_ = token;
    }

    Message Miot::Send(const std::string &command, const std::string &payload)
    {
        Message msg = Handshake();
        if (msg.header.stamp == 0)
        {
            return msg;
        }
        std::string request = createRequest(command, payload);
        // Message msg;
        // msg.header.deviceID = deviceID;
        // msg.header.stamp = deviceStamp;
        std::string data = msg.build(request, token_);
        ESP_LOGE(TAG, "data:%s", data.data());
        int socket_handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (socket_handle < 1)
        {
            ESP_LOGE(TAG, "句柄初始化失败");
        }
        struct sockaddr_in dest_addr{};
        inet_pton(AF_INET, ip_.data(), &dest_addr.sin_addr);

        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(54321);

        int err = connect(socket_handle, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err != 0)
        {
            ESP_LOGI(TAG, "连接失败");
        }
        else
        {
            ESP_LOGE(TAG, "连接成功");
        }

        err = ::send(socket_handle, data.data(), data.size(), 0);
        if (err < 0)
        {
            ESP_LOGI(TAG, "发送失败");
        }
        else
        {
            ESP_LOGE(TAG, "指令发送成功");
        }

        struct timeval timeout;
        timeout.tv_sec = 0; // 设置超时时间为 5 秒
        timeout.tv_usec = 300 * 1000;

        if (setsockopt(socket_handle, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1)
        {
            ESP_LOGE(TAG, "设置timeout失败");
            close(socket_handle);
            return msg;
        }

        std::string rx_buffer;
        rx_buffer.resize(256);
        int len = recv(socket_handle, rx_buffer.data(), rx_buffer.size(), 0);
        if (len < 0)
        {
            ESP_LOGI(TAG, "接收失败");
            return msg;
        }
        else
        {
            ESP_LOGE(TAG, "接收成功指令返回消息");
        }

        close(socket_handle);
        Message response;
        response.parse(rx_buffer);
        response.success = true;
        return response;
    }

    Message Miot::Handshake()
    {
        Message msg;
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

        int socket_handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (socket_handle < 1)
        {
            ESP_LOGE(TAG, "句柄初始化失败");
        }
        struct sockaddr_in dest_addr{};
        inet_pton(AF_INET, ip_.data(), &dest_addr.sin_addr);

        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(54321);

        int err = connect(socket_handle, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err != 0)
        {
            ESP_LOGE(TAG, "连接失败");
        }
        else
        {
            ESP_LOGI(TAG, "连接成功");
        }

        err = ::send(socket_handle, helo_char, sizeof(helo_char), 0);
        if (err < 0)
        {
            ESP_LOGE(TAG, "发送失败");
        }
        else
        {
            ESP_LOGI(TAG, "握手发送成功");
        }

        std::string rx_buffer;
        rx_buffer.resize(512);
        struct timeval timeout;
        timeout.tv_sec = 0; // 设置超时时间为 5 秒
        timeout.tv_usec = 400 * 1000;

        if (setsockopt(socket_handle, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1)
        {
            ESP_LOGE(TAG, "设置timeout失败");
            close(socket_handle);
            return msg;
        }
        int len = recv(socket_handle, rx_buffer.data(), rx_buffer.size(), 0);
        if (len < 0)
        {
            ESP_LOGE(TAG, "接收失败");
            return msg;
        }
        else
        {
            ESP_LOGE(TAG, "接收成功握手消息");
        }
        ESP_LOGE(TAG, "接收数据%s", rx_buffer.c_str());
        close(socket_handle);
        msg.parse(rx_buffer);
        return msg;
    }
    Message Miot::InitHandshake(const std::string &ip)
    {
        ESP_LOGI(TAG, "InitHandshake");
        Message msg;
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
        std::string rx_buffer = socketSend(ip, std::string((char *)helo_char, sizeof(helo_char)));
        msg.parse(rx_buffer);
        return msg;
    }
    Message Miot::getDeviceProperties(const std::string &ip, const std::string &token, uint32_t deviceID, uint32_t deviceStamp)
    {

        return Message();
    }
    std::string Miot::createRequest(const std::string &command, const std::string &parameters)
    {
        std::string request = "{\"id\": " + std::to_string(id) + ", \"method\": \"" + command + "\"";
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

    std::string Miot::socketSend(const std::string &ip, const std::string &data)
    {
        int socket_handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (socket_handle < 1)
        {
            ESP_LOGE(TAG, "句柄初始化失败");
        }
        struct sockaddr_in dest_addr{};
        inet_pton(AF_INET, ip.data(), &dest_addr.sin_addr);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(54321);

        int err = connect(socket_handle, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err != 0)
        {
            ESP_LOGE(TAG, "连接失败");
        }
        else
        {
            ESP_LOGE(TAG, "连接成功");
        }

        err = ::send(socket_handle, data.data(), data.size(), 0);
        if (err < 0)
        {
            ESP_LOGE(TAG, "发送失败");
        }
        else
        {
            ESP_LOGE(TAG, "握手发送成功");
        }

        std::string rx_buffer;
        rx_buffer.resize(512);
        struct timeval timeout;
        timeout.tv_sec = 0; // 设置超时时间为 5 秒
        timeout.tv_usec = 300 * 1000;

        if (setsockopt(socket_handle, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1)
        {
            ESP_LOGE(TAG, "设置timeout失败");
            close(socket_handle);
            return "";
        }
        int len = recv(socket_handle, rx_buffer.data(), rx_buffer.size(), 0);
        if (len < 0)
        {
            ESP_LOGE(TAG, "接收失败");
            return "";
        }
        else
        {
            ESP_LOGE(TAG, "接收成功握手消息");
        }
        ESP_LOGE(TAG, "接收数据%s", rx_buffer.c_str());
        close(socket_handle);
        return rx_buffer;
    }

}
