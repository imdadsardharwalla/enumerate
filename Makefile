.DELETE_ON_ERROR:

BINARY            := enumerate
C_SRCS            := main.c

LIBS              :=
CFLAGS            := $(CFLAGS) -std=c99 -Wall -Werror -O2

# Uncomment the line below to produce a debug build
# CFLAGS            := $(CFLAGS) -O0 -g3

SRC_DIR           := src
BUILD_DIR         := build

OBJS              := $(C_SRCS:%.c=$(BUILD_DIR)/%.o)
DEPS              := $(OBJS:%.o=%.d)

MK_PATH           := $(abspath $(lastword $(MAKEFILE_LIST)))

# Default target named after binary
$(BINARY): $(BUILD_DIR)/$(BINARY)

# Actual target of the binary: depends on all .o files
$(BUILD_DIR)/$(BINARY): $(OBJS) | $(BUILD_DIR)
    # Link .o files
	$(CC) -o $@ $^ $(LIBS) $(CFLAGS)

# Include the .d files (generated below)
-include $(DEPS)

# Build target for each object file.
# Potential dependency on header files is covered by
# calling '-include $(DEPS)'
$(OBJS): $(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(MK_PATH) | $(BUILD_DIR)
    # The -MMD flag additionally creates a .d file with
    # the same name as the .o file
	$(CC) -MMD -c -o $@ $< $(CXXFLAGS)

# Target is the build directory
$(BUILD_DIR):
    # Create the directory if it doesn't yet exist
	mkdir -p $@

.PHONY: clean
clean:
    # Remove all generated files
	rm -rf build/
