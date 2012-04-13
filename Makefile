all:
	make -C libzpu
	make -C zpusim
	make -C asmtest
	make -C ctest

clean:
	make -C libzpu clean
	make -C zpusim clean
	make -C asmtest clean
	make -C ctest clean

