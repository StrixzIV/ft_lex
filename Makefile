NAME        = ft_lex
LIBL        = libl.a

CC          = clang
CFLAGS      = -Wall -Wextra -Werror

CXX         = clang++
CXXFLAGS    = -Wall -Wextra -Werror -std=c++17 -Iinclude

SRC_DIR     	= src
OBJ_DIR    	 	= obj
TEMPLATE_DIR 	= template
LIBL_DIR    	= libl

TEMPLATE_FILE = $(TEMPLATE_DIR)/template.c
GENERATED_SRC = $(SRC_DIR)/template_data.cpp
GENERATED_OBJ = $(OBJ_DIR)/template_data.o

SRCS        = $(wildcard $(SRC_DIR)/*.cpp)
OBJS        = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS)) $(GENERATED_OBJ)

LIBL_SRCS   = $(wildcard $(LIBL_DIR)/*.c)
LIBL_OBJS   = $(patsubst $(LIBL_DIR)/%.c, $(OBJ_DIR)/%.o, $(LIBL_SRCS))

all: clang $(NAME) $(LIBL)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^
	rm -f $(GENERATED_SRC)

$(GENERATED_SRC): $(TEMPLATE_FILE)
	@echo "🛠️ Generating C++ source from $< ..."
	@{ \
		echo '#include <cstddef>'; \
		xxd -i $< | sed 's/unsigned char template_template_c/extern const unsigned char TEMPLATE_C_START/g; s/unsigned int template_template_c_len/extern const std::size_t TEMPLATE_C_SIZE/g'; \
	} > $@

$(GENERATED_OBJ): $(GENERATED_SRC) | $(OBJ_DIR)
	@echo "Compiling embedded data: $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(LIBL): $(LIBL_OBJS)
	ar rcs $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(LIBL_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clang:
	python3 generate_compile_commands.py

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME) $(LIBL)

re: fclean all

.PHONY: all clean fclean re
