/**
 * Project 用例图
 */


#ifndef _CCOMPRESSIONMANAGER_H
#define _CCOMPRESSIONMANAGER_H

class CcompressionManager {
public: 
    void Attribute1;
    
/**
 * @param int compressionMethod
 * @param char* filePackage
 */
char* compress(void int compressionMethod, void char* filePackage);
    
/**
 * @param int compressionMethod
 * @param char*filePacke
 */
char* decompress(void int compressionMethod, void char*filePacke);
};

#endif //_CCOMPRESSIONMANAGER_H