# SPDX-License-Identifier: GPL-3.0-or-later

VERSION 	:= 0.1.1

CC 			:= gcc
CFLAGS 		:= -Wall -Wextra -Wpedantic -Wno-missing-field-initializers -std=gnu23 -I. -DPROGRAM_VERSION=\"$(VERSION)\"
LDFLAGS 	:=

BUILD_DIR 	:= build
TARGET  	:= $(BUILD_DIR)/qref
SRCS 		:= qref.c registry.c lookup.c $(wildcard topics/*.c)
OBJS 		:= $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) -o $@ $^
	size $@

$(BUILD_DIR)/%.o: %.c qref.h makefile
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILD_DIR)

run: all
	./$(TARGET)
