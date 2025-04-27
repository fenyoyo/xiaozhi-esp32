#ifndef _MI_H
#define _MI_H
#include <string>
class Mi
{
public:
    Mi();
    ~Mi();

    bool GetMi();
    bool GetBinding() { return has_binding_; }
    bool GetMiBindingStatus() { return has_mi_binding_; };
    const std::string &GetErrMsg() { return err_msg; };

private:
    bool has_binding_ = true;
    bool has_mi_binding_ = true;
    std::string err_msg;

    bool open_iot_ = true;

    std::string processString(const std::string &input);
};

#endif // _MI_H