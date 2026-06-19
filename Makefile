NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread
RM = rm -rf

SOURCES = coder.c codexion.c dongles.c heap.c heap_utils.c monitor.c parser.c \
		  setup.c

OBJS = $(SOURCES:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

lint:
	norminette
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(NAME) 3 500 100 100 100 3 50 fifo
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(NAME) 3 150 200 100 100 5 50 edf
	valgrind --tool=helgrind ./$(NAME) 3 500 100 100 100 3 50 edf
	valgrind --tool=helgrind ./$(NAME) 3 150 200 100 100 5 50 fifo

.PHONY: all clean fclean re lint
