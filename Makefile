# directories
MSPGCC_ROOT_DIR = ../../../tools/msp430-gcc
MSPGCC_BIN_DIR = $(MSPGCC_ROOT_DIR)/bin
MSPGCC_INCLUDE_DIR =$(MSPGCC_ROOT_DIR)/include
INCLUDE_DIRS = $(MSPGCC_INCLUDE_DIR)
LIB_DIRS = $(MSPGCC_INCLUDE_DIR)
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin
DEBUG_BIN_DIR = ~/programming/tools/mspdebug
DEBUG_DRIVERS_DIR = /usr/lib

# toolchain
CC = $(MSPGCC_BIN_DIR)/msp430-elf-gcc
GDB = $(MSPGCC_BIN_DIR)/msp430-elf-gdb
DEBUG = LD_LIBRARY_PATH=$(DEBUG_DRIVERS_DIR) $(DEBUG_BIN_DIR)/mspdebug

# files
TARGET = $(BIN_DIR)/templcd
SOURCES = main.c
OBJECT_NAMES = $(SOURCES:.c=.o)
OBJECTS = $(patsubst %, $(OBJ_DIR)/%, $(OBJECT_NAMES))

# flags
MCU = msp430f5529
WFLAGS = -Wall -Wextra -Werror -Wshadow
CFLAGS = -mmcu=$(MCU) $(WFLAGS) $(addprefix -I, $(INCLUDE_DIRS))
DEBUGFLAGS = -O0 -g3 -ggdb -gdwarf-2
LDFLAGS = -mmcu=$(MCU) $(addprefix -L, $(LIB_DIRS))

# build
$(TARGET): $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) $^ -o $@

# compiling
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c -o $@ $^

# assemble only
main.s: main.c
	$(CC) -S $(CFLAGS) $(DEBUGFLAGS) -o $@ $^

# phony targets
.PHONY: all clean assemble

all: $(TARGET)

assemble: main.s

clean:
	rm -rf $(BUILD_DIR)
	rm -rf ./*.s

flash: $(TARGET)
	$(DEBUG) tilib "prog $(TARGET)"

gdb_listen: $(TARGET)
	$(DEBUG) tilib "gdb"

gdb_connect: $(TARGET)
	$(GDB) $(TARGET)
