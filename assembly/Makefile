.ONESHELL:

QAS = ../build/qas
SOURCES = qos.asm syscall.asm puts.asm shell.asm strcmp.asm restart.asm globals.asm

all: qos.bin

qos.bin: $(QAS) $(SOURCES)
	$(QAS) $(SOURCES) -o qos.bin
