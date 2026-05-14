# Strictly portable flags:
#   -std=c99        we use C99 (snprintf etc); swap for -ansi / -std=c89 if needed
#   -Wall -Wextra   catch common mistakes
#   -Wpedantic      reject non-standard extensions
#   -g              debug symbols (remove for release)
#   -fsanitize=...  catch memory and UB bugs at runtime (remove for musl/older gcc)
CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -Wpedantic -g

TESTS   = test_parse_request_line test_parse_request_headers

.PHONY: all test clean

all: test

# Build and run all test binaries
# - `platform_id` and `$(TESTS)` are dependencies that will be built before running this `test` target
# - `| test_results` is an "order-only prerequisite", it ensures that the `test_results` dir exists but doen't rebuild
#   just because the timestamp changed
# - `@` suppresses Make from echoing the command
# - `$$` is how to escape an $
# - `TEST_OUTPUT_FILE` is the filename test results will be written to, it's exported so the test files can access it
# - Before looping through the tests, truncate the file with an empty string in write mode to clear it
test: platform_id $(TESTS) | test_results
	@export TEST_OUTPUT_FILE=$$(./platform_id); \
	> "$$TEST_OUTPUT_FILE"; \
	for t in $(TESTS); do ./$$t || exit 1; done

# This is a special binary for creating custom file names for each system.
# - When `platform_id.c` or `platform.h` change, rebuild the `platform_id` binary
# - `$@` is a Make automatic variable meaning "the target of this rule", so it expands to platform_id
platform_id: platform_id.c platform.h
	$(CC) $(CFLAGS) -o $@ platform_id.c

# This will match any files that start with `test_` and the `%` acts like a capture group in a regex, will be replaced
# with whatever it takes the place of in the matched files.
test_%: test_%.c %.c parse_utils.c test.h %.h parse_utils.h
	$(CC) $(CFLAGS) -o $@ $< $*.c parse_utils.c

# Ensure the results directory exists
test_results:
	mkdir -p test_results

clean:
	rm -f $(TESTS) platform_id
