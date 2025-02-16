libs-y += binutils/
libs-y += fileio/
libs-y += git/
libs-y += system/
libs-y += tests/
libs-y += demos/

tools = $(PWD)/tools/
libs  = $(PWD)/lib
CFLAGS := -I$(libs)/include -L$(libs)
LDFLAGS := -Wl,-rpath=$(libs)

export tools libs CFLAGS LDFLAGS

subdirs := $(patsubst %/,%,$(libs-y))

.PHONY: all clean
all:
	@for dir in $(subdirs); do \
		$(MAKE) -C $$dir all; \
	done

clean:
	@for dir in $(subdirs); do \
		$(MAKE) -C $$dir clean; \
	done
