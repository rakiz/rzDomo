#include "Tools.h"

String getContentType(String filename) {
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

String formatBytes(size_t bytes) { // convert sizes in bytes to KB and MB
    if (bytes < 1024) {
        return String(bytes) + "B";
    } else if (bytes < (1024 * 1024)) {
        return String(bytes / 1024.0) + "KB";
    } else if (bytes < (1024 * 1024 * 1024)) {
        return String(bytes / 1024.0 / 1024.0) + "MB";
    } else {
        return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
    }
}
