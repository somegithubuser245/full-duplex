#ifndef V7_SENDER_H
#define V7_SENDER_H

#include <bitset>
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <cstdint>
#include <sstream>
#include <iomanip>
#include "package_vector.h"
#include "RPiDriver.h"
#include "FrameTypes.h"

class Sender {
private:
    RPiDriver &drv;
    std::string Datei="";
    std::vector <std::string> Packages;
    int Package_counter=0;
    bool Frame_sended=false;
    uint16_t crc16(const std::string& data);
    std::string crc16_to_string(uint16_t crc);
    std::string completePackage(std::string package);
    uint8_t Flag_bits;
public:
    Sender(RPiDriver &drv,std::string Datei);
    void PackageNumber_send(std::unique_lock<std::mutex>& lock);
    void Flag(std::unique_lock<std::mutex>& lock);
    void bitssendung(std::string Package);
    void Packege_teilung();
    void Package_sendung();
    void Package_dispatch();
    void Block_sender(std::unique_lock<std::mutex>& lock);
    void sendWithLock(uint8_t data, bool portA);
    void last_package();
};

#endif //V7_SENDER_H