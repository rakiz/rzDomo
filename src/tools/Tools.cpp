#include "Tools.h"

String myTools::getContentType(const String &filename) {
    if (filename.endsWith(".htm") || filename.endsWith(".html")) return CONTENT_TYPE_HTML;
    if (filename.endsWith(".css")) return CONTENT_TYPE_CSS;
    if (filename.endsWith(".js")) return CONTENT_TYPE_JAVASCRIPT;
    if (filename.endsWith(".png")) return CONTENT_TYPE_PNG;
    if (filename.endsWith(".gif")) return CONTENT_TYPE_GIF;
    if (filename.endsWith(".jpg") || filename.endsWith(".jpeg")) return CONTENT_TYPE_JPEG;
    if (filename.endsWith(".ico")) return CONTENT_TYPE_ICON;
    if (filename.endsWith(".xml")) return CONTENT_TYPE_XML;
    if (filename.endsWith(".pdf")) return CONTENT_TYPE_PDF;
    if (filename.endsWith(".zip")) return CONTENT_TYPE_ZIP;
    if (filename.endsWith(".gz")) return CONTENT_TYPE_GZIP;
    if (filename.endsWith(".json")) return CONTENT_TYPE_JSON;
    return CONTENT_TYPE_TEXT;
}

String myTools::formatBytes(size_t bytes) { // convert sizes in bytes to KB and MB
    String size;
    size.reserve(10);
    if (bytes < 1024) {
        size = String((long) bytes);
        size.concat("B");
    } else if (bytes < (1024 * 1024)) {
        size = String(bytes / 1024.0);
        size.concat("KB");
    } else if (bytes < (1024 * 1024 * 1024)) {
        size = String(bytes / 1024.0 / 1024.0);
        size.concat("MB");
    } else {
        size = String(bytes / 1024.0 / 1024.0 / 1024.0);
        size.concat("GB");
    }
    return size;
}

bool myTools::stringConcat(char *dest, const __FlashStringHelper *str, unsigned int destMaxLength) {
    if (!str) return false;

    unsigned int strLength = strlen_P((PGM_P) str);
    if (strLength == 0) return true;

    unsigned int destLength = strlen(dest);

    unsigned int newLength = destLength + strLength;
    if (newLength > destMaxLength) {
        Serial.printf("ERROR: We cannot concatenate the following strings within a buffer of %d (it needs at least %d)\r\n\t%s\r\n\t%s\r\n",
                      destMaxLength, newLength, dest, (PGM_P) str);
        return false;
    }

    memcpy_P(dest + destLength, (PGM_P) str, strLength + 1);
    return true;
}

bool myTools::stringConcat(char *dest, const char *cstr, unsigned int destMaxLength) {
    unsigned int strLength = strlen_P((PGM_P) cstr);
    unsigned int destLength = strlen(dest);

    unsigned int newLength = destLength + strLength;
    if (!cstr)
        return false;
    if (strLength == 0)
        return true;
    if (newLength > destMaxLength) {
        Serial.printf("ERROR: We cannot concatenate the following strings within a buffer of %d\r\n%s\r\n%s",
                      destMaxLength, dest, cstr);
        return false;
    }

    memmove_P(dest + destLength, (PGM_P) cstr, strLength + 1);
//    memcpy_P(dest + destLength, (PGM_P) cstr, strLength + 1);
    dest[newLength] = 0;
    return true;
}

bool myTools::stringConcat(char *dest, float num, unsigned int destMaxLength) {
    char buf[20];
    char* string = dtostrf(num, 4, 2, buf);
    return myTools::stringConcat(dest, string, destMaxLength);
}

bool myTools::stringConcat(char *dest, unsigned char num, unsigned int destMaxLength) {
    char buf[1 + 3 * sizeof(unsigned char)];
    sprintf(buf, "%d", num);
    return myTools::stringConcat(dest, buf, destMaxLength);
}

bool myTools::stringConcat(char *dest, int num, unsigned int destMaxLength) {
    char buf[2 + 3 * sizeof(int)];
    sprintf(buf, "%d", num);
    return myTools::stringConcat(dest, buf, destMaxLength);
}

bool myTools::stringConcat(char *dest, unsigned int num, unsigned int destMaxLength) {
    char buf[1 + 3 * sizeof(unsigned int)];
    utoa(num, buf, 10);
    return myTools::stringConcat(dest, buf, destMaxLength);
}

bool myTools::stringConcat(char *dest, long num, unsigned int destMaxLength) {
    char buf[2 + 3 * sizeof(long)];
    sprintf(buf, "%ld", num);
    return myTools::stringConcat(dest, buf, destMaxLength);
}

bool myTools::stringConcat(char *dest, unsigned long num, unsigned int destMaxLength) {
    char buf[1 + 3 * sizeof(unsigned long)];
    ultoa(num, buf, 10);
    return myTools::stringConcat(dest, buf, destMaxLength);
}
