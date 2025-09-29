BUILD_DIR = build
EXE = game

# detect OS
ifeq ($(OS),Windows_NT)
    RM = rmdir /S /Q
    EXE_EXT = .exe
    RUN = $(BUILD_DIR)\$(EXE)$(EXE_EXT)
else
    RM = rm -rf
    EXE_EXT =
    RUN = ./$(BUILD_DIR)/$(EXE)$(EXE_EXT)
endif

compile: build
	cmake --build $(BUILD_DIR)

build:
	cmake -S . -B $(BUILD_DIR) -G "Ninja" -DCMAKE_EXPORT_COMPILE_COMMANDS=on

clean:
	$(RM) $(BUILD_DIR)

run: compile
	$(RUN)

all: run
