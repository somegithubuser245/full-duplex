#include "headers/Sender.h"
#include "headers/Checksum.h"

extern std::mutex b15f_mutex;
extern std::condition_variable cv;
extern bool isSenderActive;
extern bool isReceiverActive;
extern std::array<bool, 16> lostPackages; 

extern bool isFirstPeer;

Sender::Sender(RPiDriver &drv, std::string Datei) : drv(drv), Datei(Datei) {
    Package_counter = 0;
    Packege_teilung();
}

void Sender::bitssendung(std::string Package) {
    uint8_t outputMask = isFirstPeer ? 0x0F : 0xF0;
    // sendWithLock(outputMask, false); this is useless and leads to bugs when using RPi!
    sendWithLock(Flag_bits, true);
    sendWithLock(DATA_TYPE, true);

    for (char zeichen : Package) {
        sendWithLock(zeichen >> 4, true);
        sendWithLock(zeichen, true);
    }

    sendWithLock(static_cast<uint8_t>(Package_counter), true);
    sendWithLock(0x0, true); //clear last bits
    // std::cerr << "Reset PORTA after sending." << std::endl;
    Package_counter++;
}

void Sender::Packege_teilung() {
    const size_t chunkSize = PACKAGE_SIZE;
    Packages.reserve((Datei.size() + chunkSize - 1) / chunkSize);
    static const std::string nullPadding(8, '0');  // Pre-create padding

    for (size_t i = 0; i < Datei.size(); i += chunkSize) {
        std::string chunk = Datei.substr(i, chunkSize);
        if (chunk.size() < chunkSize) {
            chunk.append(nullPadding, 0, chunkSize - chunk.size());
        }
        // std::move reallocates memory (не надо копировать и вставлять чанк)
        Packages.push_back(std::move(chunk));
        std::cerr << chunk << std::endl;
    }
}

void Sender::Package_sendung() {
    Flag_bits = 0x0F;

    for (int i = 0; i < Packages.size() - 1; i++) {
        bitssendung(completePackage(Packages[i]));
    }

    last_package();
}

void Sender::last_package() {
    bitssendung(completePackage(Packages[Packages.size() - 1]));
}

void Sender::sendWithLock(uint8_t data, bool portA) {
    std::unique_lock<std::mutex> lock(b15f_mutex);
    cv.wait(lock, []{ return isSenderActive; });

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
    cv.notify_all();
}

std::string Sender::completePackage(std::string package) {
    uint16_t checksum = Checksum::crc16(package);
    // Convert checksum to two bytes
    char highByte = (checksum >> 8) & 0xFF;
    char lowByte = checksum & 0xFF;
    // Append checksum bytes to package
    package += highByte;
    package += lowByte;
    return package;
}
