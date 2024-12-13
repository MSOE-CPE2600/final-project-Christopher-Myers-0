CC=gcc
CFLAGS=-c -Wall -g
LDFLAGS=-lm -lpthread
SOURCES= main.c dft.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=main.out

all: $(SOURCES) $(EXECUTABLE)

# pull in dependency info for *existing* .o files
-include $(OBJECTS:.o=.d)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.c.o: 
	$(CC) $(CFLAGS) $< -o $@
	$(CC) -MM $< > $*.d

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE) *.d