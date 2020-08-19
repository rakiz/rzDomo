#include "tools/Tools.h"
#include "RzFiles.h"

void RzFiles::setup() { // Start the SPIFFS and list all contents
    SPIFFS.begin();                             // Start the SPI Flash File System (SPIFFS)
    {
        Serial.println("SPIFFS started.");
        FSInfo fs_info{};
        SPIFFS.info(fs_info);

        Serial.printf(
                "FS Info: \r\n\t totalBytes: %s\r\n\t usedBytes: %s\r\n\t blockSize: %s\r\n\t pageSize: %s\r\n\t maxOpenFiles: %zu\r\n\t maxPathLength: %zu\r\n",
                formatBytes(fs_info.totalBytes).c_str(), formatBytes(fs_info.usedBytes).c_str(),
                formatBytes(fs_info.blockSize).c_str(), formatBytes(fs_info.pageSize).c_str(),
                fs_info.maxOpenFiles, fs_info.maxPathLength);
    }

    {
        char buffer[80];
        Serial.println("Contents:");
        Dir dir = SPIFFS.openDir("/");
        while (dir.next()) {                      // List the file system contents
            String fileName = dir.fileName();
            String contentType = getContentType(fileName);
            size_t fileSize = dir.fileSize();
            time_t creation = dir.fileCreationTime();
            struct tm *timeInfo = localtime(&creation);
            strftime(buffer, 80, "%F %T", timeInfo);
            Serial.printf("\tFS File: %s, created: %s, size: %s (%s)\r\n", fileName.c_str(), buffer,
                          formatBytes(fileSize).c_str(), contentType.c_str());
        }
        Serial.printf("\n");
    }
}

void RzFiles::loop(ulong referenceTime) {
    // nothing to do for now. Maybe at sometime some cleanup
}

File RzFiles::openRead(const char *path) {  // send the right file to the client (if it exists)
    if (SPIFFS.exists(path)) {
        return SPIFFS.open(path, "r"); // Open the file
    } else return File();
}

File RzFiles::openWrite(const char *path) {
    return SPIFFS.open(path, "w");
}

RzFiles::RzFiles() = default;

const char *RzFiles::getId() {
    return "files";
}

const char *RzFiles::getDisplayName() {
    return "File system";
}

const char *RzFiles::getPrefix() {
    return "Files";
}
