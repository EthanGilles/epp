# Variables
BUILD_DIR = build
OUTPUT_BINARY = epp.out
LOCAL_BIN = /usr/local/bin

# Default target
all: 
	make build

# Build target
build:
	cmake --build $(BUILD_DIR)

# Clean target
clean:
	rm -f out*

# Test target
test:
	./build/epp examples/main.epp
	./out 

# Install target
install:
	make build
	cp $(BUILD_DIR)/$(OUTPUT_BINARY) $(LOCAL_BIN)/epp
	chmod +x $(LOCAL_BIN)/epp
