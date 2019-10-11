#include <stdio.h>

#include "hw4_lib.h"


int main(int argc, char const *argv[]) {
    pid_t a;

    log_info("%d",3);
    log_warn("WARN");
    debug("DEBUG");
    printf("%lu\n",sizeof(a));
    return 0;
}
