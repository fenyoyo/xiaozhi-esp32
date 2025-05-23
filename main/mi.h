#ifndef _MI_H
#define _MI_H
#include <string>
#include "cJSON.h"
#include "iot_mqtt_protocol.h"
class Mi
{
public:
    Mi();
    ~Mi();

    bool GetMi();
    bool GetBinding() { return has_binding_; }
    bool GetMiBindingStatus() { return has_mi_binding_; };
    const std::string &GetErrMsg() { return err_msg; };
    bool GetOpenIot() { return open_iot_; };
    bool RegisterIot();

private:
    cJSON *root_;
    bool has_binding_ = false;
    bool has_mi_binding_ = false;
    std::string err_msg;
    IotMqttProtocol iot_mqtt_protocol_;
    bool open_iot_ = false;
    cJSON *device_list_ = cJSON_CreateArray();

    std::string processString(const std::string &input);
};

#endif // _MI_H