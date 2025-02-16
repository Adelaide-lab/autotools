CC := gcc
SRC_DIR := src
LIB_DIR := lib
TOOLS_DIR := tools
PWD := $(shell pwd)

export PWD CC

.PHONY: lib all clean
all: lib $(TOOLS_DIR)/executables

lib:
	@$(MAKE) -C $(LIB_DIR)

$(TOOLS_DIR):
	@mkdir -p $(TOOLS_DIR)

$(TOOLS_DIR)/executables: lib $(TOOLS_DIR)
	@$(MAKE) -C $(SRC_DIR) MOVE_DIR=$(TOOLS_DIR)

clean:
	@$(MAKE) -C $(SRC_DIR) clean
	@$(MAKE) -C $(LIB_DIR) clean