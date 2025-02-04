NAME= webserv
SRC= $(wildcard src/*.cpp)
OBJ_DIR= obj/
OBJ= $(SRC:%.cpp=$(OBJ_DIR)%.o)
CC= c++
CPPFLAGS= -g -Wall -Wextra -Werror -std=c++98 -Iinclude

all: $(NAME)

$(OBJ_DIR)%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -c $< -o $@

$(NAME): $(OBJ)
	$(CC) $(CPPFLAGS) $(OBJ) -o $(NAME)

clean:
	rm -rf $(OBJ_DIR)

clean_logs:
	# Remove log files
	rm -f log.txt

fclean: clean
	rm -f $(NAME)
.PHONY: all clean fclean re