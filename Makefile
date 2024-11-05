# Variables
BUILD_DIR = build
OUTPUT_BINARY = please
LOCAL_BIN = /usr/local/bin

# Default target
all: 
	make build

# Clean target
clean:
	rm -f out*

# Test 
test:
	./build/please examples/testing.epp
	./out

# Install target
install:
	cmake --build build/
	cp $(BUILD_DIR)/$(OUTPUT_BINARY) $(LOCAL_BIN)/please
	chmod +x $(LOCAL_BIN)/please

install-test:
	please examples/hello.epp -o hello
	./hello

lines-project:
	./build/count-lines.sh src/main.cpp src/include/*

lines-assembly:
	./build/count-lines.sh out.asm
