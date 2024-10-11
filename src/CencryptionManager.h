/**
 * Project 用例图
 */


#ifndef _CENCRYPTIONMANAGER_H
#define _CENCRYPTIONMANAGER_H

class CencryptionManager {
public: 
    void Attribute1;
    
/**
 * @param int encryptionMethod
 * @param string encryptionKey
 * @param char* filePackage
 */
char* encrypt(void int encryptionMethod, void string encryptionKey, void char* filePackage);
    
/**
 * @param int encryptionMethod
 * @param string encryptionKey
 * @param char* filePackage
 */
char* dencrypt(void int encryptionMethod, void string encryptionKey, void char* filePackage);
};

#endif //_CENCRYPTIONMANAGER_H