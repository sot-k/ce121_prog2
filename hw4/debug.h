#ifndef __dbg_h__
#define __dbg_h__

#include <stdio.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#ifdef DEBUG
#define debug(M, ...) fprintf(stderr, KMAG "[DEBUG]" KNRM " %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define debug(...) if(0);
#endif

#define log_error(M, ...) fprintf(stderr, KRED "[ERROR]" KNRM " %s:%d " M "\n", __FILE__, __LINE__,  ##__VA_ARGS__)
#define log_warn(M, ...) fprintf(stderr, KYEL "[WARN]" KNRM  " %s:%d " M "\n", __FILE__, __LINE__,  ##__VA_ARGS__)
//#define log_info(M, ...) fprintf(stderr, KGRN "[INFO]" KNRM " %s:%d " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define log_info(M, ...) fprintf(stderr, KGRN "[INFO]" KNRM ": " M "\n",  ##__VA_ARGS__)

#endif
