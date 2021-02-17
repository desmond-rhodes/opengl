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

TMP := .$(OUT)

$(OUT): $(OBJS)
	touch $(TMP).o
	make $(TMP)
	rm -f $(TMP).o
	mv $(TMP) $(OUT)

$(TMP): $(TMP).o $(OBJS)

.PHONY: clean
clean:
	rm -f *.o
	rm -f $(OUT)
