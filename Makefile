NAME= webserv
SRC= $(wildcard src/*.cpp)
OBJ_DIR= obj/
OBJ= $(SRC:%.cpp=$(OBJ_DIR)%.o)
CC= c++
CPPFLAGS= -Wall -Wextra -Werror -std=c++98 -Iinclude

all: setup $(NAME)

setup:
	@if ! which python3 > /dev/null || ! which php > /dev/null; then \
		echo "Error: python3 and/or php isn't installed in this computer. Please install them manually"; \
		exit 1; \
	fi
	@which python3 > python3_path.txt
	@which php > php_path.txt
	@chmod +x data/cgi-bin/*.py
	@chmod 777 data/cgi-bin/record.json
	@chmod +x data/cgi-bin/*.php

$(OBJ_DIR)%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -c $< -o $@

$(NAME): $(OBJ)
	$(CC) $(CPPFLAGS) $(OBJ) -o $(NAME)

clean:
	rm -rf $(OBJ_DIR)
	rm -f log.txt python3_path.txt php_path.txt
	rm -f data/upload/*

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
