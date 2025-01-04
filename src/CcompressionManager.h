/**
 * Project 用例图
 */
#include <vector>
#include <tuple>
#include <iomanip>
#include "CdataPackager.h"

#ifndef _CCOMPRESSIONMANAGER_H
#define _CCOMPRESSIONMANAGER_H

class CcompressionManager {
public: 

unsigned int createChecksumForCompressionTests(char *data, int size);

/**
 * @param int compressionMethod
 * @param char* filePackage
 */
dataPackage compress(int compressionMethod, dataPackage filePackage);
    
/**
 * @param int compressionMethod
 * @param char*filePacke
 */
dataPackage decompress(int compressionMethod, dataPackage filePackage);
};

#endif //_CCOMPRESSIONMANAGER_H