NAME	= codexion
CC	= gcc
CFLAGS	= -Wall -Wextra	-Werror

SRCS	= coders/simulation/clean_simulation.c\
	  coders/control_data/data_init.c\
	  coders/control_data/dongles.c\
	  coders/control_data/main.c\
	  coders/control_data/monitor.c\
	  coders/control_data/parse.c\
	  coders/simulation/simulation.c\
	  coders/simulation/utils_simulation.c\
	  coders/simulation/utils_simulation_2.c\
	  coders/simulation/utils.c\
	  coders/simulation/heap.c
OBJS	= $(SRCS:.c=.o)
DEPS	= codexion.h    

all: $(NAME)
	
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -pthread -o $(NAME)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

