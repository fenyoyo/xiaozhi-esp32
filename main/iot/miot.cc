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
        if (msg.success == false)
        {
            return msg;
        }
        Message response;
        std::string request = createRequest(command, payload);
        std::string data = msg.build(request, token_);
        // ESP_LOGE(TAG, "data:%s", data);
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
            return response;
        }
        err = ::send(socket_handle, data.data(), data.size(), 0);
        if (err < 0)
        {
            ESP_LOGE(TAG, "发送失败");
            return response;
        }

        struct timeval timeout;
        timeout.tv_sec = 1; // 设置超时时间为 5 秒
        timeout.tv_usec = 0;

        if (setsockopt(socket_handle, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1)
        {
            ESP_LOGE(TAG, "设置timeout失败");
            close(socket_handle);
            return response;
        }

        std::string rx_buffer;
        rx_buffer.resize(512);
        int len = recv(socket_handle, rx_buffer.data(), rx_buffer.size(), 0);
        if (len < 0)
        {
            ESP_LOGE(TAG, "接收失败");
            return response;
        }

        close(socket_handle);
        // Message response;
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

        err = ::send(socket_handle, helo_char, sizeof(helo_char), 0);
        if (err < 0)
        {
            ESP_LOGE(TAG, "发送失败");
        }

        std::string rx_buffer;
        rx_buffer.resize(32);
        struct timeval timeout;
        timeout.tv_sec = 1; // 设置超时时间为 1 秒
        timeout.tv_usec = 0;

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
        close(socket_handle);
        msg.parse(rx_buffer);
        msg.success = true;
        return msg;
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

}
