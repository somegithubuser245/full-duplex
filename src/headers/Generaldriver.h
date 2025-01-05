#ifndef V7_GDRIVER_H
#define V7_GDRIVER_H

#include <mutex>
#include <condition_variable>
#include <iomanip>
#include "RPiDriver.h"

class GeneralDriver {
private:
    RPiDriver &drv;
    std::mutex b15f_mutex;
    std::condition_variable cv;
    bool isSenderActive;
    bool isReceiverActive;
    bool isFirstPeer;

public:
    GeneralDriver(RPiDriver &drv, bool isFirstPeer);
    void sendWithLock(uint8_t data, bool portA);
    uint8_t readWithLock();
};

#endif // V7_GRDRIVER_H