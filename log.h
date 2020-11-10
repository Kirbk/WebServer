#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define DEF   (KNRM)

inline void info(char* msg) {
    fprintf(stdout, "%s[INFO] %s%s\n", KGRN, msg, DEF);
}

inline void warning(char* msg) {
    fprintf(stdout, "%s[WARNING] %s%s\n", KYEL, msg, DEF);
}

inline void fatal(char* msg) {
    fprintf(stderr, "%s[FATAL] %s%s\n", KRED, msg, DEF);
}

inline void additional(char* msg) {
    fprintf(stdout, "%s    [->] %s%s\n", KWHT, msg, DEF);
}

inline void system_out() {
    fprintf(stdout, "%s    [*] %s", KCYN, DEF);
}

inline void logger(char* msg, int level) {
    switch (level) {
    case 0:
        info(msg);
    case 1:
        warning(msg);
    case 2:
        fatal(msg);
    }
}


#endif