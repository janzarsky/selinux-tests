SOURCES = $(wildcard *.c)

###########################################################################

EXECUTABLE = libsemanage-tests
CFLAGS += -g -Wall -Werror -std=gnu99
LDFLAGS += -lcunit -lsemanage -lselinux
OBJECTS = $(SOURCES:.c=.o) 

ifdef VERS_MAJOR
DEFINES += -D VERS_MAJOR=$(VERS_MAJOR)
endif

ifdef VERS_MINOR
DEFINES += -D VERS_MINOR=$(VERS_MINOR)
endif

ifdef VERS_RELEASE
DEFINES += -D VERS_RELEASE=$(VERS_RELEASE)
endif

all: $(EXECUTABLE) 

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(DEFINES) -c $*.c -o $*.o

clean distclean: 
	rm -rf $(OBJECTS) $(EXECUTABLE)

test: all 
	./$(EXECUTABLE)

