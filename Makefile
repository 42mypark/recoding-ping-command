NAME := ft_ping
SRCS := ft_ping.c show.c parse_arguments.c recv_pong.c send_ping.c ft_htons.c calc_statistics.c
OBJS := $(SRCS:.c=.o)
CFLAGS := -Wall -Wextra -Werror -g
LDFLAGS := -fsanitize=address -g
LIBFT := ft
LIBFT_DIR := ./libft
INCS := -I $(LIBFT_DIR)
LIBS := -L$(LIBFT_DIR) -l$(LIBFT)

all: $(NAME)

$(NAME): $(OBJS) $(LIBFT) 
	$(CC) $(LDFLAGS) -o $@ $(filter-out $(lastword $^), $^) $(LIBS)

%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $< $(INCS)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

fclean:
	$(MAKE) clean
	rm -rf $(NAME)
	rm -rf test

clean:
	rm -rf $(OBJS)

test: test.c
	$(CC) $(LDFLAGS) -o $@ test.c $(LIBS)