OUT := graphic
OBJS := main.o

CXXFLAGS := -Wall -std=c++17 -Iinclude
LDLIBS := -lglfw3

# Detect Windows Subsystem for Linux
WSLENV ?= notwsl
ifndef WSLENV
	CXX := x86_64-w64-mingw32-g++-posix
	LDFLAGS := -Llib/wsl
	LDLIBS += -lgdi32 -lopengl32
	LDLIBS += -static-libgcc -static-libstdc++ -static -lwinpthread
endif

# Detect MacOS
ifeq ($(shell uname -s),Darwin)
	CXX := clang++
	CPPFLAGS := -DGL_SILENCE_DEPRECATION
	LDFLAGS := -Llib/mac
	LDLIBS += -framework Cocoa -framework OpenGL -framework IOKit
endif

TMP := .$(OUT)

$(OUT): $(OBJS)
	touch $(TMP).cc
	make $(TMP)
	mv $(TMP) $(OUT)
	rm -f $(TMP)*

$(TMP): $(TMP).cc $(OBJS)

.PHONY: clean
clean:
	rm -f *.o
	rm -f $(OUT)
	rm -f $(TMP)*
