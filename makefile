VERSION 	:= 0.1.0

CC 			:= gcc
CFLAGS 		:= -Wall -Wextra -Wno-missing-field-initializers -std=gnu23 -I. -DPROGRAM_VERSION=\"$(VERSION)\"

BUILD_DIR 	:= build
TARGET  	:= $(BUILD_DIR)/qref
SRCS 		:= qref.c registry.c lookup.c $(wildcard topics/*.c)
OBJS 		:= $(patsubst %.c, $(BUILD_DIR)/%.o,$(SRCS))

.PHONY: all clean run

all: compile_commands.json $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $^
	size $@ $^

$(BUILD_DIR)/%.o: %.c qref.h makefile
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

compile_commands.json: makefile
	@printf '[\n' > $@
	@first=1; for src in $(SRCS); do \
		[ $$first -eq 0 ] && printf ',\n' >> $@; first=0; \
		printf '  {\n    "directory": "%s",\n    "arguments": ["%s", "-Wall", "-Wextra", "-Wno-missing-field-initializers", "-std=gnu23", "-I.", "-DPROGRAM_VERSION=\\\"%s\\\"", "-c", "%s"],\n    "file": "%s"\n  }' \
			"$(CURDIR)" "$(CC)" "$(VERSION)" "$$src" "$$src" >> $@; \
	done
	@printf '\n]\n' >> $@


clean:
	rm -rf $(BUILD_DIR) compile_commands.json

run: all
	./$(TARGET)
