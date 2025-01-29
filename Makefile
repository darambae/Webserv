NAME= webserv
SRC= $(wildcard src/*.cpp)
OBJ= $(SRC:.cpp=.o)
CC= c++
CFLAGS= -Wall -Wextra -Werror -std=c++98 -Iinclude

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

clean:
	rm -f $(OBJ)
	rm -f log.txt

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

