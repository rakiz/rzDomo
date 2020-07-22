#include "RzFiles.h"
#include "tools/Tools.h"

void RzFiles::setup() { // Start the SPIFFS and list all contents
    SPIFFS.begin();                             // Start the SPI Flash File System (SPIFFS)
    {
        Serial.println("SPIFFS started.");
        FSInfo fs_info;
        SPIFFS.info(fs_info);

        Serial.printf(
                "FS Info: \r\n\t totalBytes: %s\r\n\t usedBytes: %s\r\n\t blockSize: %s\r\n\t pageSize: %s\r\n\t maxOpenFiles: %d\r\n\t maxPathLength: %d\r\n",
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
            struct tm *timeinfo = localtime(&creation);
            strftime(buffer, 80, "%F %T", timeinfo);
            Serial.printf("\tFS File: %s, created: %s, size: %s (%s)\r\n", fileName.c_str(), buffer,
                          formatBytes(fileSize).c_str(), contentType.c_str());
        }
        Serial.printf("\n");
    }
}

void RzFiles::loop(unsigned long _currentMillis) {
    // nothing to do for now. Maybe at sometime some cleanup
}

/**
 * returned file MUST be closed 
 */
File RzFiles::fileRead(String _path) {  // send the right file to the client (if it exists)
    //Serial.println("handleFileRead: " + path);
    String pathWithGz = _path + ".gz";
    if (SPIFFS.exists(pathWithGz) ||
        SPIFFS.exists(_path)) {  // If the file exists, either as a compressed archive, or normal
        if (SPIFFS.exists(pathWithGz))                          // If there's a compressed version available
            _path += ".gz";                                         // Use the compressed version
        File file = SPIFFS.open(_path, "r");                    // Open the file
        return file;
    }
    return File();                                          // If the file doesn't exist, return false
}
