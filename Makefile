NAME := ft_ping
SRCS := ft_ping.c 
OBJS := $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o:%.c
	$(CC) -c -o $@ $<


fclean:
	$(MAKE) clean
	rm -rf $(NAME)

clean:
	rm -rf $(OBJS)
