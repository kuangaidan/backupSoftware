/**
 * Project 用例图
 */


#ifndef _CCONDITIONMANAGER_H
#define _CCONDITIONMANAGER_H

class CconditionManager {
public: 
    string encryptionKey;
    void Attribute1;
    time_t backupTime;
    string fileTypeFilters;
    
/**
 * @param int Method
 */
bool setEncryptionMethod(void int Method);
    
/**
 * @param string newKey
 */
bool setEncryptionKey(void string newKey);
    
/**
 * @param int Method
 */
bool setCompressionMethod(void int Method);
    
/**
 * @param bool on
 */
bool setWatchDogOn(void bool on);
    
void disableBackupTimer();
    
/**
 * @param string filterList
 */
bool setFileTypeFilters(void string filterList);
private: 
    int encryptionMethod;
    int compressionMethod;
    bool watchDogOn;
};

#endif //_CCONDITIONMANAGER_H