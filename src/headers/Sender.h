#ifndef V7_SENDER_H
#define V7_SENDER_H

#include <bitset>
#include <iostream>
#include <vector>
#include <cstdint>
#include <sstream>
#include <iomanip>
#include "Generaldriver.h"

class Sender {
private:
    GeneralDriver &gdrv;
    std::string Datei="";
    std::vector <std::string> Packages;
    int Package_counter=0;
    bool Frame_sended=false;
    std::string completePackage(std::string package);
    uint8_t Flag_bits;

    void sync();
public:
    Sender(GeneralDriver &gdrv, std::string &Datei);
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