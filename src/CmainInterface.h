/**
 * Project 用例图
 */


#ifndef _CMAININTERFACE_H
#define _CMAININTERFACE_H

class CmainInterface {
public: 
    CdataBackup myDataBackup;
    CencryptionManager myEncryptionManager;
    CcompressionManager myCompressionManager;
    CdataRestoration myDataRestoration;
    
void startGUI();
    
void main();
};

#endif //_CMAININTERFACE_H