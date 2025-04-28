#ifndef _MI_H
#define _MI_H
#include <string>
#include "cJSON.h"
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
    bool has_binding_ = false;
    bool has_mi_binding_ = false;
    std::string err_msg;

    bool open_iot_ = false;
    cJSON *device_list_ = cJSON_CreateArray();

    std::string processString(const std::string &input);
};

#endif // _MI_H