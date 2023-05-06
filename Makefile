CLANG ?= clang-13
LLVM_STRIP ?= llvm-strip-13
ARCH := x86
INCLUDES := -I/usr/include -I/usr/include/x86_64-linux-gnu
LIBS_DIR := -L/usr/lib/lib64 -L/usr/lib/x86_64-linux-gnu
LIBS := -lbpf -lelf

.PHONY: all clean run

all: deny_unshare.skel.h deny_unshare.bpf.o deny_unshare

run: all
	sudo ./deny_unshare

clean:
	rm -f *.o
	rm -f deny_unshare.skel.h

deny_unshare.bpf.o: deny_unshare.bpf.c
	$(CLANG) -g -O2 -Wall -target bpf -D__KERNEL__ -D__TARGET_ARCH_$(ARCH) $(INCLUDES) -c $< -o $@
	$(LLVM_STRIP) -g $@ # Removes debug information

deny_unshare.skel.h: deny_unshare.bpf.o
	sudo bpftool gen skeleton $< > $@

deny_unshare: deny_unshare.c deny_unshare.skel.h
	$(CC) -g -Wall -c $< -o $@.o
	$(CC) -g -o $@ $(LIBS_DIR) $@.o $(LIBS)

.DELETE_ON_ERROR: