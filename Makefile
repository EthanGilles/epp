# Variables
BUILD_DIR = build
OUTPUT_BINARY = please
LOCAL_BIN = /usr/local/bin

# Default target
all: 
	make build

# Clean target
clean:
	rm -f out* test*

# Test 
test:
	./build/please examples/testing.pls
	./out

# Install target
install:
	mkdir build 
	cmake -S . -B build
	cmake --build build/
	cp $(BUILD_DIR)/$(OUTPUT_BINARY) $(LOCAL_BIN)/please
	chmod +x $(LOCAL_BIN)/please


reinstall:
	cmake --build build/
	cp $(BUILD_DIR)/$(OUTPUT_BINARY) $(LOCAL_BIN)/please
	chmod +x $(LOCAL_BIN)/please

install-test:
	please examples/install.pls -o test
	./test

lines-project:
	./count-lines.py src/main.cpp src/include/*

lines-assembly:
	./count-lines.py out.asm
