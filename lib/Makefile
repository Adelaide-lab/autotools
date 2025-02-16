src 	:= $(sort $(shell find . -type f -name "*.c"))
objs	:= $(patsubst %.c, %.o, $(src))
incs	:= $(shell find . -type f \( -name "*.h" -o -name "*.inc" \) | xargs -I {} dirname {} | sort -u)
lib		:= libutil.a

CFLAGS := -I$(incs)
LDFLAGS := -static

all: $(lib)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(lib): $(objs)
	@ar rcs $@ $(objs)

clean:
	@rm -f $(objs)