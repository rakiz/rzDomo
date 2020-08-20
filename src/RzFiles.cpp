#include "tools/Tools.h"
#include "RzFiles.h"

RzFiles::RzFiles() {
    RzFiles::loadConfiguration();
}

void RzFiles::setup() { // Start the SPIFFS and list all contents
    SPIFFS.begin();                             // Start the SPI Flash File System (SPIFFS)
    {
        Serial.println(F("SPIFFS started."));
        FSInfo fs_info{};
        SPIFFS.info(fs_info);

        Serial.printf(
                "FS Info: \r\n\t totalBytes: %s\r\n\t usedBytes: %s\r\n\t blockSize: %s\r\n\t pageSize: %s\r\n\t maxOpenFiles: %zu\r\n\t maxPathLength: %zu\r\n",
                myTools::formatBytes(fs_info.totalBytes).c_str(), myTools::formatBytes(fs_info.usedBytes).c_str(),
                myTools::formatBytes(fs_info.blockSize).c_str(), myTools::formatBytes(fs_info.pageSize).c_str(),
                fs_info.maxOpenFiles, fs_info.maxPathLength);
    }

    {
        char buffer[80];
        Serial.println(F("Contents:"));
        Dir dir = SPIFFS.openDir("/");
        while (dir.next()) {                      // List the file system contents
            String fileName = dir.fileName();
            String contentType = myTools::getContentType(fileName);
            size_t fileSize = dir.fileSize();
            time_t creation = dir.fileCreationTime();
            struct tm *timeInfo = localtime(&creation);
            strftime(buffer, 80, "%F %T", timeInfo);
            Serial.printf("\tFS File: %s, created: %s, size: %s (%s)\r\n", fileName.c_str(), buffer,
                          myTools::formatBytes(fileSize).c_str(), contentType.c_str());
        }
        Serial.println();
    }
}

void RzFiles::loop(ulong referenceTime) {
    // nothing to do for now. Maybe at sometime some cleanup
}

const char *RzFiles::getId() {
    return "files";
}

const char *RzFiles::getDisplayName() {
    return "File system";
}

const char *RzFiles::getPrefix() {
    return "Files";
}

const char *RzFiles::getJsonConfig() {
    char *config = new char[1];
    *config = 0;
    return config;
}

void RzFiles::loadConfiguration() {

}

void RzFiles::saveConfiguration() {

}
