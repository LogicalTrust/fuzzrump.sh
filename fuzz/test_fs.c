#include <sys/types.h>
#include <errno.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/param.h>

#include <string.h>

#include <rump/rump.h>
#include <rump/rump_syscalls.h>


#define CHECK(x) \
    if ((x) != 0) { \
        fprintf(stderr, "failed\n"); \
        exit(EXIT_FAILURE); \
    }



static void
die(const char *fmt, ...)
{
        va_list va;

        va_start(va, fmt);
        vfprintf(stderr, fmt, va);
        va_end(va);
        fputs("\n", stderr);
        exit(1);
}

void
usage(int status)
{
        fprintf(stderr, "options: [-46h] [-i afl_iterations]\n");
        exit(status);
}

union u {
	unsigned i;
	char c;
};

union args_t {
    struct rump_ufs_args ufs;
    /* fake */
    char buf[1024];
};

int
main(int argc, char *argv[])
{
	union args_t args;
	union u u;
	char buf[8192], ch;
	int fd, rv, iter;
	char* TEXT = "LogicalTrust", *fstype = "ffs", *path = NULL;

    while((ch = getopt(argc, argv, "ht:f:i:")) != -1) {
            switch(ch) {
            case 'h':
                    usage(EXIT_SUCCESS);
            case 'i':
                    iter = atoi(optarg);
                    break;
            case 't':
                    fstype = strdup(optarg);
                    break;
            case 'f':
                    path = strdup(optarg);
                    break;
            case '?':
                    usage(EXIT_FAILURE);
                    break;
            }
    }

	CHECK(rump_init());

    if (path == NULL)
        die("path (-f) not provided");

#define MYFSDEV "/de-vice"
	rv = rump_pub_etfs_register(MYFSDEV, path, RUMP_ETFS_BLK);
	if (rv != 0)
		die("etfs reg failed: %d", rv);
	args.ufs.fspec =  (void *)(uintptr_t)MYFSDEV;
#undef MYFSDEV

	if (rump_sys_mkdir("/mnt", 0755) == -1)
		die("mkdir /mnt");
	if (rump_sys_mount(fstype, "/mnt", 0, &args, sizeof(args)) == -1)
		die("mount");

	int c = 2;
	while (c--) {
		//1 - create new file
		if ((fd = rump_sys_open("/mnt/README", RUMP_O_WRONLY | RUMP_O_CREAT)) != -1) {
			printf("write %zd of %lu\n", rump_sys_write(fd, TEXT, strlen(TEXT)), strlen(TEXT));
			rump_sys_close(fd);
		}

		//2 - fail on overwriting
		if ((fd = rump_sys_open("/mnt/README", RUMP_O_WRONLY | RUMP_O_CREAT | RUMP_O_EXCL)) != -1) {
			printf("write %zd of %lu\n", rump_sys_write(fd, TEXT, strlen(TEXT)), strlen(TEXT));
			rump_sys_close(fd);
		}

		//3 - create directory
		rump_sys_mkdir("/mnt/main", 0000777);
		rump_sys_mkdir("/mnt/main/dir1", 0000777);
		rump_sys_mkdir("/mnt/main/dir2", 0000777);

		
		//4 - remove directory
		rump_sys_rmdir("/mnt/main/dir2");
		rump_sys_rmdir("/mnt/main/dir3");
		rump_sys_rmdir("/mnt/main");

		//5 - move file
		rump_sys_link("/mnt/README", "/mnt/main/dir1/readme");
	}

	rump_sys_unmount("/mnt", 0);
	rump_sys_reboot(0, NULL);
	die("reboot failed to reboot");

	return 0;
}
