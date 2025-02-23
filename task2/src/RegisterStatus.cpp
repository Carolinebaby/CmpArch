#include "RegisterStatus.hpp"
std::string RegisterStatus::str(void)
{
    return (busy ? "Yes" : "No") + std::string("\t") + (value.length()?value:"ins"+std::to_string(entry));
}

void RegisterStatus::set(bool b, int e)
{
    busy = b, entry = e;
}

void RegisterStatus::set(bool b, std::string v, int e)
{
    busy = b, value = v, entry = e;
}