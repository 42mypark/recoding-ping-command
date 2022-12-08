NAME := ft_ping
SRCS := ft_ping.c show.c parse_arguments.c recv_ping.c send_ping.c ft_htons.c calc_statistics.c fatal_error_check.c
OBJS := $(SRCS:.c=.o)
CFLAGS := -Wall -Wextra -Werror -g
LDFLAGS := -fsanitize=address -g
LIBFT := libft.a
LIBFT_NAME := ft
LIBFT_DIR := ./libft
INCS := -I $(LIBFT_DIR)
LIBS := -L$(LIBFT_DIR) -l$(LIBFT_NAME)

all: $(NAME)

$(NAME): $(LIBFT_DIR)/$(LIBFT) $(OBJS)  
	$(CC) $(LDFLAGS) -o $@ $(filter-out $(firstword $^), $^) $(LIBS)

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

re: fclean all
