srcs	:=	$(wildcard *.c)
objs	:= 	$(patsubst %.c, %.o, $(srcs))

all:$(srcs:.c=) $(objs)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -lutil -lpthread

%: %.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< -lutil -lpthread
	@mv $@ $(tools)

clean:
	@rm -rf *.o $(srcs:.c=)