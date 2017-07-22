#include"unit.h"

//去掉前后空格，因为field传来的char*有尾部空格
std::string& Unit::trim(std::string &s) {
    if (s.empty()) {
        return s;
     }
    s.erase(0,s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}
