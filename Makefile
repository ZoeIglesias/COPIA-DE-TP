all: 
	make -C cpu
	make -C filesystem
	make -C kernel
	make -C memoria

clean:
	make clean -C cpu
	make clean -C filesystem
	make clean -C kernel
	make clean -C memoria

recompile: clean all