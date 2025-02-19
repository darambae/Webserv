NAME= webserv
SRC= $(wildcard src/*.cpp)
OBJ_DIR= obj/
OBJ= $(SRC:%.cpp=$(OBJ_DIR)%.o)
CC= c++
CPPFLAGS= -g -Wall -Wextra -Werror -std=c++98 -Iinclude

all: setup $(NAME)

setup:
	@which python3 > python3_path.txt
	@chmod +x data/cgi-bin/*.py

$(OBJ_DIR)%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -c $< -o $@

$(NAME): $(OBJ)
	$(CC) $(CPPFLAGS) $(OBJ) -o $(NAME)

# cgi-prep:
# 	sudo apt install python3
# 	chmod +x cgi-bin/*.py

clean:
	rm -rf $(OBJ_DIR)
	rm -f log.txt python3_path.txt
	rm -f data/upload/*

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re