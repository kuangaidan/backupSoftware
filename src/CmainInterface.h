/**
 * Project 用例图
 */


#ifndef _CMAININTERFACE_H
#define _CMAININTERFACE_H
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstdint>
using namespace std;

class CmainInterface {
public: 
    
void interactGUI();

vector<string> splitFilters(const char* filters);
    
};


#endif //_CMAININTERFACE_H