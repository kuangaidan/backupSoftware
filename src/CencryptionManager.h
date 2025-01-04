/**
 * Project 用例图
 */


#ifndef _CENCRYPTIONMANAGER_H
#define _CENCRYPTIONMANAGER_H

#include "commonStructures.h"
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <cstring>


class CencryptionManager {
public: 


unsigned int createChecksumForEncryptionTests(char *data, int size);
    
/**
 * @param int encryptionMethod
 * @param string encryptionKey
 * @param char* filePackage
 */
dataPackage encrypt(int encryptionMethod, string encryptionKey, dataPackage filePackage);
    
/**
 * @param int encryptionMethod
 * @param string encryptionKey
 * @param char* filePackage
 */
dataPackage decrypt(int encryptionMethod, string encryptionKey, dataPackage filePackage);
};

#endif //_CENCRYPTIONMANAGER_H