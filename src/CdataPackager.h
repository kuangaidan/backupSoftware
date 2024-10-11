/**
 * Project 用例图
 */


#ifndef _CDATAPACKAGER_H
#define _CDATAPACKAGER_H

class CdataPackager {
public: 
    
/**
 * @param string rootFolder
 * @param string fileFilters
 */
bool packageFiles(void string rootFolder, void string fileFilters);
    
/**
 * @param string destFolder
 * @param char* package
 */
bool unpackageFile(void string destFolder, void char* package);
    
/**
 * @param string rootFolder
 * @param filterStruct filter
 */
vector<string> listOfFiles enumerateFiles(void string rootFolder, void filterStruct filter);
private: 
    void struct filterStruct;
    
/**
 * @param int fileDescriptor
 * @param char* buffer
 */
bool packageSingleFile(void int fileDescriptor, void char* buffer);
    
/**
 * @param string dest
 * @param char* fileData
 */
bool unpackageSingleFile(void string dest, void char* fileData);
};

#endif //_CDATAPACKAGER_H