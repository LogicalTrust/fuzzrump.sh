#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <rump/rump.h>
#include <rump/rump_syscalls.h>
#include <rump/netconfig.h>


#define CHECK(x) \
    if ((x) != 0) { \
        fprintf(stderr, "failed\n"); \
        exit(EXIT_FAILURE); \
    }

/* use it with caution */
#define RUMPNS(x) rumpns_ ## x

extern void RUMPNS(fuzzrump_ip_input)(void *, size_t len);
extern void RUMPNS(fuzzrump_ip6_input)(void *, size_t len);

#ifndef __AFL_INIT
#define __AFL_INIT()
#define __AFL_LOOP(x)   (x--)
#endif


void
usage(int status)
{
        fprintf(stderr, "options: [-46h] [-i afl_iterations]\n");
        exit(status);
}

int
main(int argc, char **argv)
{
        char buf[65536], ch;
        int len, iter = 1;
        char mode4, mode6;
        struct lwp *curlwp;
        void (*target)(void *, size_t);
        
        target = RUMPNS(fuzzrump_ip_input);
        
        while((ch = getopt(argc, argv, "hi:46")) != -1) {
                switch(ch) {
                case 'h':
                        usage(EXIT_SUCCESS);
                case 'i':
                        iter = atoi(optarg);
                        break;
                case '4':
                        target = RUMPNS(fuzzrump_ip_input);
                        break;
                case '6':
                        target = RUMPNS(fuzzrump_ip6_input);
                        break;
                case '?':
                        usage(EXIT_FAILURE);
                        break;
                }
        }

        CHECK(rump_init());
        rump_pub_lwproc_newlwp(0);

        __AFL_INIT();
        while(__AFL_LOOP(iter)) {
                len = fread(buf, 1, sizeof(buf), stdin);
                (*target)(buf, len);
        }

        return 0;
}
