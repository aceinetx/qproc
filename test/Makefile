.ONESHELL:

QAS = ../build/qas
all: a.bin quine.bin

quine.bin: $(QAS) quine.asm
	$(QAS) quine.asm -o quine.bin

a.bin: $(QAS) a.asm
	$(QAS) a.asm -o a.bin

$(QAS):
	cd ..
	mkdir build -p && cd build
	cmake ..
	make -j$(nproc)
	cd ../test
