Welcome to the fuzzrump.sh repository. It contains basic tools to build and torture rump kernels using various fuzzers. It is based on [buildrump.sh](https://github.com/rumpkernel/buildrump.sh), since then following changes were made:

 * Realign from NetBSD-7 to -CURRENT (9.99.x)
 * Support building with Sanitizers
 * Support building with AFL
 * Added tiny intercept.so library to catch various function calls that are hooked by the sanitizers.
 * Added various fuzzers

Please note that this is a playground for ongoing research. Few good souls did a talks about the project, if you have spare time you can watch them:

* In English - https://media.ccc.de/v/Camp2019-10334-anykernels_meet_fuzzing
* In Polish - https://www.youtube.com/watch?v=2GKd0Z82bds 
* Slides - https://bsd-pl.org/assets/talks/2019-10-10_1_Mateusz-Kocielski_anykernels_meet_fuzzing.pdf

# Building rump kernel

Clone repo:

    $ git clone https://github.com/LogicalTrust/fuzzrump.sh
    $ cd fuzzrump.sh

This repo contains only toolset needed to build it and orchestra to torture it. The heart of the project - rumpkernel - is in another repository which is forked directly from the NetBSD source. You have to fetch it:

    $ ./buildrump.sh checkoutgit

Now we are ready to build the toolset. Now we are ready to build the toolset. Note that you want to adapt -j option (it specifies the number of jobs to run simultaneously).

    $ ./buildrump.sh -j8 tools

Now we have all the zoo needed to build the rumpkernel:

    $ ./buildrump.sh -j8 build

We're done, last step is to install it:

    $ ./buildrump.sh install

If everything went good, you should have rump directory that contains rump servers, libraries and all the stuff needed to mess with it.

## Building rump kernel with sanitizers

    ./buildrump.sh -d rump.asan -o obj.asan -j8 -F CFLAGS="-fsanitize=address" -F LDFLAGS="-fsanitize=address" tools build install

## Building rump with AFL++

    AFL_CC_COMPILER=LLVM CC=afl-cc ./buildrump.sh -d rump.afl-asan -o obj.afl-asan -j8 -F CFLAGS="-fsanitize=address" -F LDFLAGS="-fsanitize=address" tools build install

For more information check: https://github.com/AFLplusplus/AFLplusplus/blob/stable/docs/env_variables.md

## Building fuzz helpers

    cd fuzz/
    
    $CC -o net_direct_cov test_net_direct.c -I ../rump/include/ -L ../rump/lib/ -pthread -lrump -lrumpuser -lrumpvfs -lrumpvfs_fifofs -lrumpnet -lrumpnet_net -lrump -lrumpnet_netinet -lrumpnet_netinet6 -lrumpnet_local
    
    $CC -o test_fs test_fs.c -I ../rump/include/ -L ../rump/lib/ -Wl,--no-as-needed -lrump -lrumpdev -lrumpdev_disk -lrumpnet_net -lrumpnet -lrumpnet_local -lrumpvfs_fifofs -lrumpvfs -lrumpfs_ffs -lrumpfs_ext2fs -lrumpfs_msdos -lrumpfs_cd9660 -lrumpfs_efs -lrumpfs_ext2fs -lrumpfs_fdesc -lrumpfs_ffs -lrumpfs_hfs -lrumpfs_kernfs -lrumpfs_lfs -lrumpfs_mfs -lrumpfs_msdos -lrumpfs_nfsserver -lrumpfs_nfs -lrumpfs_nilfs -lrumpfs_ntfs -lrumpfs_sysvbfs -lrumpfs_tmpfs -lrumpfs_udf -lrumpfs_union -lrumpfs_v7fs
    
XXX: Add -fsanitize=address in case of ASAN and set appropiate $CC

# Results
TBD
# Possible projects
TBD
