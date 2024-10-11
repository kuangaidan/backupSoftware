/**
 * Project 用例图
 */


#ifndef _CWATCHDOGTIMER_H
#define _CWATCHDOGTIMER_H

class CwatchDogTimer {
public: 
    bool active;
    time_t backUpTime;
    
bool checkTime();
    
void disable();
    
/**
 * @param time_t newBackupTime
 */
void start(void time_t newBackupTime);
};

#endif //_CWATCHDOGTIMER_H