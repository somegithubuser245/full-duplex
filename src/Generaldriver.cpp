#include <bitset>
#include <iostream>
#include "headers/Generaldriver.h"

GeneralDriver::GeneralDriver(RPiDriver &drv, bool isFirstPeer) : drv(drv), isFirstPeer(isFirstPeer) {
    isSenderActive = true;
    isReceiverActive = false;
}

uint8_t GeneralDriver::readWithLock() {
    std::unique_lock<std::mutex> lock(b15f_mutex);
    cv.wait(lock, [this] { return isReceiverActive; });
    uint8_t bits;

    if (isFirstPeer) {
        bits = drv.getRegister(nullptr) >> 4;
    } else {
        bits = drv.getRegister(nullptr) & 0x0F;
    }

    // std::cerr << "Read bits: " << std::bitset<4>(bits) << std::endl;  // keep for debugging

    isReceiverActive = false;
    isSenderActive = true;
    cv.notify_one();

    return bits;
}

void GeneralDriver::sendWithLock(uint8_t data, bool portA) {
    std::unique_lock<std::mutex> lock(b15f_mutex);
    cv.wait(lock, [this]{ return isSenderActive; });

    if (portA) {
        if (isFirstPeer) {
            drv.setRegister(nullptr, data & 0x0F);
        } else {
            drv.setRegister(nullptr, (data << 4) & 0xF0);
        }
    } else {
        drv.setRegister(nullptr, data);
    }
    
    isSenderActive = false;
    isReceiverActive = true;
    cv.notify_one();
}