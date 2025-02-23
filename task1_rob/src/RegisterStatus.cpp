#include "RegisterStatus.hpp"
std::string RegisterStatus::str(void)
{
    return (busy ? "Yes" : "No") + std::string("\t") + (value.length()?value:"ins"+std::to_string(entry));
}

// 设置 busy, entry
void RegisterStatus::set(bool b, int e)
{
    busy = b, entry = e;
}

// 设置 busy, value, entry
void RegisterStatus::set(bool b, std::string v, int e)
{
    busy = b, value = v, entry = e;
}