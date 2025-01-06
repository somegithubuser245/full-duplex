#include "headers/Sender.h"
#include "headers/Checksum.h"
#include "headers/Config.h"
#include "headers/FrameTypes.h"

extern bool isFirstPeer;

Sender::Sender(GeneralDriver &gdrv, std::string Datei) : gdrv(gdrv), Datei(Datei) {
    Package_counter = 0;
    Packege_teilung();
}

void Sender::bitssendung(std::string Package) {
    // uint8_t outputMask = isFirstPeer ? 0x0F : 0xF0;
    // sendWithLock(outputMask, false); this is useless and leads to bugs when using RPi!
    gdrv.sendWithLock(FLAG, true);
    gdrv.sendWithLock(DATA, true);

    for (char zeichen : Package) {
        gdrv.sendWithLock(zeichen >> 4, true);
        gdrv.sendWithLock(zeichen, true);
    }

    gdrv.sendWithLock(static_cast<uint8_t>(Package_counter), true);
    gdrv.sendWithLock(0x0, true); //clear last bits
    // std::cerr << "Reset PORTA after sending." << std::endl;
    Package_counter++;
}

void Sender::Packege_teilung() {
    const size_t chunkSize = PACKAGE_SIZE;
    Packages.reserve((Datei.size() + chunkSize - 1) / chunkSize);
    static const std::string nullPadding(chunkSize, '0');  // Pre-create padding

    for (size_t i = 0; i < Datei.size(); i += chunkSize) {
        std::string chunk = Datei.substr(i, chunkSize);
        if (chunk.size() < chunkSize) {
            chunk.append(nullPadding, 0, chunkSize - chunk.size());
        }
        // std::move reallocates memory (не надо копировать и вставлять чанк)
        Packages.push_back(std::move(chunk));
        // std::cerr << chunk << std::endl;
    }
}

void Sender::Package_sendung() {
    sync();

    for (int i = 0; i < Packages.size() - 1; i++) {
        bitssendung(completePackage(Packages[i]));
    }

    last_package();
}

void Sender::sync() {
    int sync_counter = 0;

    while(sync_counter < 4) {
        gdrv.sendWithLock(SYNC, true);
        uint8_t answer = gdrv.readWithLock();

        if (answer == SYNC) {
            std::cerr << "SYNC ACK RECEIVED!\n";
            sync_counter++;
        }
    }

    return;
}

void Sender::last_package() {
    bitssendung(completePackage(Packages[Packages.size() - 1]));
}

std::string Sender::completePackage(std::string package) {
    uint16_t checksum = Checksum::crc16(package);
    // Convert checksum to two bytes
    char highByte = (checksum >> 8);
    char lowByte = checksum;
    // Append checksum bytes to package
    package += highByte;
    package += lowByte;
    return package;
}
