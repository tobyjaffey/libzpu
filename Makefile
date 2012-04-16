all:
	make -C libzpu
	make -C zpusim
	make -C zpusim-mini
	make -C asmtest
	make -C ctest
	make -C helloworld

clean:
	make -C libzpu clean
	make -C zpusim clean
	make -C zpusim-mini clean
	make -C asmtest clean
	make -C ctest clean
	make -C helloworld clean

