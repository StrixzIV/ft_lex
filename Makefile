NAME        = ft_lex
LIBL        = libl.a

CC          = clang
CFLAGS      = -Wall -Wextra -Werror

CXX         = clang++
CXXFLAGS    = -Wall -Wextra -Werror -std=c++17 -Iinclude

SRC_DIR     = src
OBJ_DIR     = obj
LIBL_DIR    = libl

SRCS        = $(wildcard $(SRC_DIR)/*.cpp)
OBJS        = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

LIBL_SRCS   = $(wildcard $(LIBL_DIR)/*.c)
LIBL_OBJS   = $(patsubst $(LIBL_DIR)/%.c, $(OBJ_DIR)/%.o, $(LIBL_SRCS))

all: $(NAME) $(LIBL)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(LIBL): $(LIBL_OBJS)
	ar rcs $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(LIBL_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME) $(LIBL)

re: fclean all

.PHONY: all clean fclean re
