/**
 * Project 用例图
 */


#ifndef _CENCRYPTIONMANAGERTEST_H
#define _CENCRYPTIONMANAGERTEST_H

#include "commonStructures.h"
#include "CencryptionManager.h"


class CencryptionManagerTest {
public: 
    
/**
 * @param int encryptionMethod
 * @param string encryptionKey
 * @param char* filePackage
 */
bool encryptionTest(int encryptionMethod, string encryptionKey, dataPackage filePackage);
    
};
#endif //_CENCRYPTIONMANAGER_H