OUT := graphic
OBJS := main.o

CXXFLAGS := -Wall -std=c++17
CPPFLAGS := -Iinclude
LDLIBS := -lglfw3

# Detect Windows Subsystem for Linux
ifdef WSLENV
	WSLENV :=
endif
WSLENV ?= notwsl
ifndef WSLENV
	CXX := x86_64-w64-mingw32-g++-posix
	LDFLAGS := -Llib/wsl
	LDLIBS += -lgdi32 -lopengl32
	LDLIBS += -static-libstdc++ -static -lpthread
endif

# Detect MacOS
ifeq ($(shell uname -s),Darwin)
	CXX := clang++
	CPPFLAGS := -DGL_SILENCE_DEPRECATION
	LDFLAGS := -Llib/mac
	LDLIBS += -framework Cocoa -framework OpenGL -framework IOKit
endif

$(OUT): $(OBJS)
	$(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o ._$@ && mv ._$@ $@

.PHONY: clean
clean:
	rm -f $(OBJS) $(OUT)
