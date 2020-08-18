#ifndef _MYTOOLS_H_
#define _MYTOOLS_H_

#include <Arduino.h>

#define CONTENT_TYPE_HTML           F("text/html")
#define CONTENT_TYPE_CSS            F("text/css")
#define CONTENT_TYPE_XML            F("text/xml")
#define CONTENT_TYPE_TEXT           F("text/plain")
#define CONTENT_TYPE_JAVASCRIPT     F("application/javascript")
#define CONTENT_TYPE_JSON           F("application/json")
#define CONTENT_TYPE_PDF            F("application/x-pdf")
#define CONTENT_TYPE_ZIP            F("application/x-zip")
#define CONTENT_TYPE_GZIP           F("application/x-gzip")
#define CONTENT_TYPE_PNG            F("image/png")
#define CONTENT_TYPE_GIF            F("image/gif")
#define CONTENT_TYPE_JPEG           F("image/jpeg")
#define CONTENT_TYPE_ICON           F("image/x-icon")

String getContentType(String _filename);

String formatBytes(size_t _bytes);

template<typename Base, typename T>
inline bool instanceof(const T*) {
    return std::is_base_of<Base, T>::value;
}

#endif