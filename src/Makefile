COMPILER_PATH = g++

# Options
CFLAGS   = -std=c++17 -O3 -Wall -Wextra
LDFLAGS  = -lb15fdrv
OBJECTS  = main.o Sender.o receiver.o Generaldriver.o Checksum.o
OUT      = main.elf

COMPILE = $(COMPILER_PATH) $(CFLAGS)

main: $(OBJECTS)
	$(COMPILE) $(OBJECTS) -o $(OUT) $(LDFLAGS)

help:
	@echo "This Makefile has the following targets:"
	@echo "make main .... to compile"
	@echo "make clean ... to delete objects and executables"
	
clean:
	@echo "Cleaning..."
	rm -f $(OBJECTS) $(OUT) *.bin gnuplotscript.gp

.cpp.o:
	$(COMPILE) -c $< -o $@