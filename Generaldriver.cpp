#include <bitset>
#include <iostream>
#include "headers/Generaldriver.h"

GeneralDriver::GeneralDriver(bool isFirstPeer, B15F &drv) : isFirstPeer(isFirstPeer), drv(drv){
    isSenderActive = true;
    isReceiverActive = false;

    if (isFirstPeer) {
        drv.setRegister(&DDRA, 0x0F);  // First 4 bits for reading, last 4 for writing
    } else {
        drv.setRegister(&DDRA, 0xF0);  // First 4 bits for writing, last 4 for reading
    }
}

uint8_t GeneralDriver::readWithLock() {
    std::unique_lock<std::mutex> lock(b15f_mutex);
    cv.wait(lock, [this] { return isReceiverActive; });
    uint8_t bits;

    if (isFirstPeer) {
        bits = drv.getRegister(&PINA) >> 4;
    } else {
        bits = drv.getRegister(&PINA) & 0x0F;
    }

     std::cerr << "Read bits: " << std::bitset<4>(bits) << std::endl;  // keep for debugging

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
            drv.setRegister(&PORTA, data & 0x0F);
        } else {
            drv.setRegister(&PORTA, (data << 4) & 0xF0);
        }
    } else {
        drv.setRegister(&PORTA, data);
    }
    
    isSenderActive = false;
    isReceiverActive = true;
    cv.notify_one();
}