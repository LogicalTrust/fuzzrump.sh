Repository used to torture Rump Kernels
=======================================

BUILD
=====

Ubuntu with gcc-6 as host:

Address Sanitizer:

 - `./buildrump.sh  tools`
 - `./buildrump.sh  -F CFLAGS="-fsanitize=thread -O0 -g" -F LDFLAGS="-ltsan -fuse-ld=gold" -j8  tools build`
 
Thread Sanitizer

 - `./buildrump.sh  tools`
 - `./buildrump.sh  -F CFLAGS="-fsanitize=address -O0 -g" -F LDFLAGS="-lasan -fuse-ld=gold" -j8  tools build`
 
TODO
====
