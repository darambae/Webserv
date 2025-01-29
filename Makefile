NAME= webserv
SRC= $(wildcard src/*.cpp)
OBJ= $(SRC:.cpp=.o)
CC= c++
CPPFLAGS= -Wall -Wextra -Werror -std=c++98 -Iinclude

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CPPFLAGS) $(OBJ) -o $(NAME)

clean:
	rm -f $(OBJ)

clean_logs:
	# Remove log files
	rm -f log.txt

fclean: clean
	rm -f $(NAME)
.PHONY: all clean fclean re