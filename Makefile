NAME := ft_ping
SRCS := ft_ping.c 
OBJS := $(SRCS:.c=.o)
CFLAGS := -Wall -Wextra -Werror
LDFLAGS := -fsanitize=address -g

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<


fclean:
	$(MAKE) clean
	rm -rf $(NAME)

clean:
	rm -rf $(OBJS)
