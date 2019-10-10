.DELETE_ON_ERROR:

BINARY            := enumerate
SOURCE            := enumerate.py

SRC_DIR           := src
BUILD_DIR         := build
WORK_DIR          := work

# Default target named after binary
$(BINARY): $(BUILD_DIR)/$(BINARY)

# Actual target of the binary
$(BUILD_DIR)/$(BINARY):
	pyinstaller $(SRC_DIR)/$(SOURCE)    \
	            --distpath $(BUILD_DIR) \
	            --workpath $(WORK_DIR)
	rm -rf $(WORK_DIR)
	rm -f $(BINARY).spec

.PHONY: clean
clean:
    # Remove all generated files
	rm -rf $(BUILD_DIR)
	rm -rf $(WORK_DIR)
	rm -f  $(BINARY).spec
