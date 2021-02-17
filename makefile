OUT := graphic
OBJS := main.o

CFLAGS := -Iinclude

# Detect Windows Subsystem for Linux
WSLENV ?= notwsl
ifndef WSLENV
	CC := x86_64-w64-mingw32-gcc
	LDFLAGS := -Llib/wsl
	LDLIBS := -lglfw3 -lgdi32 -lopengl32
endif

# Detect MacOS
ifeq ($(shell uname -s),Darwin)
	CC := clang
	CFLAGS += -DGL_SILENCE_DEPRECATION
	LDFLAGS := -Llib/mac
	LDLIBS := -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit
endif

TMP := .$(OUT)

$(OUT): $(OBJS)
	touch $(TMP).c
	make $(TMP)
	mv $(TMP) $(OUT)
	rm -f $(TMP)*

$(TMP): $(TMP).o $(OBJS)

.PHONY: clean
clean:
	rm -f *.o
	rm -f $(OUT)
	rm -f $(TMP)*
