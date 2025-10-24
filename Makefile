# Makefile for Bat_Sonar_Listener
BUILD_DIR = build

.PHONY: all build run clean lint format

all: build

build:
	cmake -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release
	cmake --build $(BUILD_DIR) --config Release

run:
	./$(BUILD_DIR)/Bat_Sonar_Listener

clean:
	rm -rf $(BUILD_DIR)

lint:
	@echo "Running Cppcheck..."
	cppcheck --enable=all --inconclusive --std=c17 \
		-I/usr/include -I/usr/local/include \
		--suppress=missingIncludeSystem . || true

	@echo "Running clang-tidy..."
	clang-tidy $(shell find src -name '*.c') main.c \
		-- -std=c17 -I/usr/include -I/usr/local/include || true

format:
	@echo "Formatting source code with clang-format..."
	clang-format -i --style=Google $(shell find src -name '*.c' -o -name '*.h') main.c

