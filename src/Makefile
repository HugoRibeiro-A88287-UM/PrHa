CCRASP = arm-buildroot-linux-gnueabihf-gcc
CCLAPTOP = gcc
CFLAGS = -g -I. 
DEPS = -pthread -lrt  -lstdc++  -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_objdetect -lopencv_imgcodecs

CSRCFLODER=$(wildcard *.c)
CPPSRCFLODER=$(wildcard *.cpp)

.PHONY: all
all: PrHaProgram 

PrHaProgram:
	$(CCLAPTOP) $(CSRCFLODER) $(CPPSRCFLODER) -o PrHaProgram.elf $(DEPS) $(CFLAGS)


.PHONY: clean
clean:
	rm -f *.elf
