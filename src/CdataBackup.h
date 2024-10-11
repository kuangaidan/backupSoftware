/**
 * Project 用例图
 */


#ifndef _CDATABACKUP_H
#define _CDATABACKUP_H

class CdataBackup {
public: 
    CconditionManager* conditionManager;
    string 密钥;
    char 按时还是定时;
    time 开始备份的时间;
    
/**
 * @param CcompressionManager* compressionManager
 */
void setCompressionManager(void CcompressionManager* compressionManager);
    
/**
 * @param CencryptionManager* encryptionManager
 */
void setEncryptionManager(void CencryptionManager* encryptionManager);
    
/**
 * @param string folderLocation
 */
bool backup(void string folderLocation);
    
void checkBackupTimer();
    
/**
 * @param string DestinationFolder
 */
bool restoreData(void string DestinationFolder);
private: 
    CwatchDogTimer watchDogTimer;
    CcompressionManager* compressionManager;
    CencryptionManager* encryptionManager;
    
/**
 * @param string rootFolder
 */
bool packageData(void string rootFolder);
};

#endif //_CDATABACKUP_H