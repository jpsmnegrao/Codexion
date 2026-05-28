NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread
RM = rm -rf

SOURCES = codexion.c parser.c setup.c run_sim.c monitor.c scheduler.c coder.c log.c \
		  dongles.c

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
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(NAME) 2 800 200 100 100 5 50 fifo 
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(NAME)

.PHONY: all clean fclean re lint
