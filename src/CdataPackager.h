/**
 * Project 用例图
 */


#ifndef _CDATAPACKAGER_H
#define _CDATAPACKAGER_H

#include "commonStructures.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <cstring>
#include <fstream>
#include <utime.h>

class CdataPackager {
public: 
   
/**
 * @param vector<string> fileList
 * @param vector<string> fileFilters
 */
dataPackage packageFiles(vector<string> fileList, vector<string> fileFilters);
    
/**
 * @param string destFolder
 * @param char* package
 */
bool unpackageFile(string destFolder, dataPackage package);
    
/**
 * @param string rootFolder
 */
vector<string> enumerateFiles(string rootFolder);
    
bool packageWriteToDisk(dataPackage package, string dest);

dataPackage readPackageFromDisk(string src);

};


#endif //_CDATAPACKAGER_H