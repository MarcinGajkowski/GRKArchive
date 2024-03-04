GCC = gcc
# GCC = g++
LIBS = -ljpeg -lm -largtable2
SOURCES = $(wildcard  *.c)
OBJECTS = $(SOURCES:.c=.o)
CFLAGS = -g -Wall

ifeq ($(OS),Windows_NT)
EXECUTABLE = geometry.exe
else
EXECUTABLE = geometry
endif


all: $(SOURCES) $(EXECUTABLE)



$(EXECUTABLE): $(OBJECTS) 
	$(GCC) $(OBJECTS) -o $@ $(LIBS)

clean:
	rm -f $(EXECUTABLE) $(OBJECTS)

