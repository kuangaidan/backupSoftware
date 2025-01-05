#ifndef COMMONSTRUCTURES_H
#define COMMONSTRUCTURES_H
#pragma once

#include <string>
#include <iostream>
#include <vector>
using namespace std;


struct filterStruct{
	int mode;	
		//0 = include
		//1 = exclude
		//>= 2 --> no effect
	vector<string> fileTypes;
};

struct dataPackage {
	char* dataPointer = nullptr;
	long size = -1;
};


#endif