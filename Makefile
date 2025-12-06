# Many thanks to Job Vranish - see https://spin.atomicobject.com/2016/08/26/makefile-c-projects/

CLIENT_EXEC := chat_client
SERVER_EXEC := chat_server

BUILD_DIR := ./build
SRC_DIR := ./src


# Source files
CLIENT_SRCS := $(shell find $(SRC_DIR)/client -name "*.c")
SERVER_SRCS := $(shell find $(SRC_DIR)/server -name "*.c")
COMMON_SRCS := $(shell find $(SRC_DIR) -maxdepth 1 -name "*.c")

# Object files
CLIENT_OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(CLIENT_SRCS)) \
               $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(COMMON_SRCS))
SERVER_OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SERVER_SRCS)) \
               $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(COMMON_SRCS))

# Include directories
INC_DIRS := $(shell find $(SRC_DIR) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

#Flags
CPPFLAGS := $(INC_FLAGS) -MMD -MP
CFLAGS ?= -Wall -Wextra -g
LDFLAGS  := -pthread -lpthread

# Default target
all: $(BUILD_DIR)/$(CLIENT_EXEC) $(BUILD_DIR)/$(SERVER_EXEC)


$(BUILD_DIR)/$(CLIENT_EXEC): $(CLIENT_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/$(SERVER_EXEC): $(SERVER_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $^ -o $@ $(LDFLAGS)


# Generic rule for building .c files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Include dependencies
-include $(CLIENT_OBJS:.o=.d)
-include $(SERVER_OBJS:.o=.d)

# Clean build
.PHONY: clean
clean:
	$(RM) -r $(BUILD_DIR)
