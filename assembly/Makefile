.ONESHELL:

QAS = ../build/qas
SOURCES = qos.asm syscall.asm
all: qos.bin

qos.bin: $(QAS) $(SOURCES)
	$(QAS) $(SOURCES) -o qos.bin
