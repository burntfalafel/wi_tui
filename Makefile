CC=clang
LIB = lib/libwpa_client.a
LDLIBS= -lform -lmenu -lncurses -lpthread -D CONFIG_CTRL_IFACE -D CONFIG_CTRL_IFACE_UNIX
CCFLAGS=-c -Wall
INCLUDEDIR = include
SRC_DIR := src
SRC := $(wildcard $(SRC_DIR)/*.c)
TARGET = witui
OBJECTS = witui.o frontend.o util.o wpa_ctrl.o

all: $(TARGET)

$(TARGET): $(OBJECTS) 
	$(CC) -o $@ $^ $(LDLIBS)

%.o: $(SRC_DIR)/%.c 
	$(CC) -I$(INCLUDEDIR) $(CCFLAGS) $< $(LIB) -o $@ $(LDLIBS)

.PHONY: clean

clean:
		rm -f *.o $(TARGET) 
