CC=clang
LIB = lib/libwpa_client.a
LDLIBS= -lform -lmenu -lncurses -lpthread -D CONFIG_CTRL_IFACE -D CONFIG_CTRL_IFACE_UNIX
CCFLAGS=-c -Wall
INCLUDEDIR = include
SRC_DIR := src
SRC := $(wildcard $(SRC_DIR)/*.c)
TARGET = witui
OBJECTS = $(SRC:$(SRC_DIR)/%.c=%.o)

all: $(TARGET)

$(TARGET): $(OBJECTS) 
	$(CC) -o $@ $^ $(LIB) $(LDLIBS)

%.o: $(SRC_DIR)/%.c 
	$(CC) -I$(INCLUDEDIR) $(CCFLAGS) $<  -o $@ 

.PHONY: clean

clean:
		rm -f *.o $(TARGET) 
