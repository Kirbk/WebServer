#include "log.h"

extern inline void info(char* msg);
extern inline void warning(char* msg);
extern inline void fatal(char* msg);
extern inline void additional(char* msg);
extern inline void system_out();
extern inline void logger(char* msg, int level);