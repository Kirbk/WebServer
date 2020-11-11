#ifndef __CONTENT_TYPES_H__
#define __CONTENT_TYPES_H__

#include <string.h>
#include <stdio.h>

#include "content_type_macros.h"

typedef struct {
    char* extension;
    char* type;
} content_type;

static const content_type http_content_types[] = {
    { "html", HTML },
    { "css", CSS },
    { "csv", CSV },
    { "js", JS_TEXT },
    { "txt", PLAIN },
    { "xml", XML },

    { "jar", JAR },
    { "ogx", OGG_APP },
    { "pdf", PDF },
    { "xhtml", XHTML },
    { "swf", FLASH },
    { "json", JSON },
    { "jsonld", LD_JSON },
    { "xml", XML },
    { "zip", ZIP },

    { "mp3", MP3 },
    { "wav", WAV },
    { "oga", OGG_A },

    { "gif", GIF },
    { "jpg", JPEG },
    { "jpeg", JPEG },
    { "png", PNG },
    { "tiff", TIFF },
    { "svg", SVG },

    { "mpeg", MPEG },
    { "mp4", MP4 },
    { "avi", AVI },
    { "webm", WEBM },
    { "ogv", OGG_V },

    { "", "" } // Allow looping
};

static content_type get_content_type(char* extension) {
    int c = 0;
    while (strcmp(http_content_types[c].extension, extension) && strcmp(http_content_types[c].extension, "")) c++;

    return http_content_types[c]; // This will not work out of scope... Fix this.
}

static void get_content_type_s(char* type_buffer, char* extension) {
    content_type cs = get_content_type(extension);
    sprintf(type_buffer, "%s", cs.type);
}


#endif