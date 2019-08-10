# this file is in public domain

CFLAGS="-fsanitize=address"

all: interceptors.so

clean:
		rm *.so

interceptors.so: interceptors.c
		$(CC) -shared -fpic $(CFLAGS) -o $@ $< 
