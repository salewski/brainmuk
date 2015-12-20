# Get absolute path to containing directory: http://stackoverflow.com/a/324782
TOP := $(dir $(CURDIR)/$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))
LOCAL_INCLUDE_DIR := $(realpath $(TOP)include)

# Special compiler flags
CFLAGS := -std=c11 -Wall -pedantic $(CFLAGS)
CPPFLAGS := -I$(LOCAL_INCLUDE_DIR) $(CPPFLAGS)
# Create .d for each .o file.
CPP_ADD_DEFINES = -MMD

NAME = brainmuk
VERSION = 0.1.0

BIN = $(NAME)
TESTBIN = tests/$(NAME)_tests
SRCS = $(wildcard src/*.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))
DEPS := $(patsubst %.c,%.d,$(SRCS))

DISTNAME = $(NAME)-$(VERSION)

### Symbolic targets ###

all: $(BIN)

clean:
	-$(RM) $(BIN) $(TESTBIN)
	-$(RM) $(OBJS)
	-$(RM) $(DEPS)
	-$(RM) $(DISTNAME).tar.gz

dist: $(DISTNAME).tar.gz

test: tests/brainmuk_tests
	./$<

watch:
	# Requires rerun <https://github.com/alexch/rerun> to be installed.
	@if hash rerun ; \
		then exit 0; \
		else echo "Requires rerun:\n\t$$ gem install rerun"; exit -1; fi
	-rerun -cxp '**/*.{c,h}' -- make test

### Actual targets! ###

# Include dependencies from -d
-include $(DEPS)

# Generates the binaries from the objects
$(BIN): $(BIN).c $(OBJS)
$(TESTBIN): $(TESTBIN).c $(OBJS)

# Override the general C to object file conversion to generate defines.
%.o: %.c
	$(COMPILE.c) $(CPP_ADD_DEFINES) -o $@ $<

$(DISTNAME).tar.gz: $(SRCS) Makefile LICENSE README.md
	git archive HEAD --prefix=$(DISTNAME)/ | gzip > $@

.PHONY: all clean dist test watch