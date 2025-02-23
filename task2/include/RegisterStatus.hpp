#ifndef REGISTERSTATUS_H
#define REGISTERSTATUS_H


#include<string>
struct RegisterStatus
{
    int entry;
    bool busy;
    std::string value = "";

    std::string str(void);
    void set(bool b, int e);
    void set(bool b, std::string v, int e);
};

#endif