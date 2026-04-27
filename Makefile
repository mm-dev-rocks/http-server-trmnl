# Strictly portable flags:
#   -std=c99        we use C99 (snprintf etc); swap for -ansi / -std=c89 if needed
#   -Wall -Wextra   catch common mistakes
#   -Wpedantic      reject non-standard extensions
#   -g              debug symbols (remove for release)
#   -fsanitize=...  catch memory and UB bugs at runtime (remove for musl/older gcc)
CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -Wpedantic -g

# If your toolchain supports it, add these to catch bugs early:
CFLAGS += -fsanitize=address,undefined

SRC     = http_parse.c
TESTS   = test_http_parse

.PHONY: all test clean

all: test

# Build and run all test binaries
test: $(TESTS)
	@echo ""
	@for t in $(TESTS); do ./$$t || exit 1; done

# Each test binary links the module under test with its test file
test_http_parse: test_http_parse.c $(SRC) test.h http_parse.h
	$(CC) $(CFLAGS) -o $@ test_http_parse.c $(SRC)

clean:
	rm -f $(TESTS)
