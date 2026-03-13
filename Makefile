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

TEMPLATE_C_FILE = template/template.c
TEMPLATE_PY_FILE = template/template.py
GENERATED_SRC = $(SRC_DIR)/data.template.cpp
GENERATED_OBJ = $(OBJ_DIR)/data.template.o

SRCS        = $(wildcard $(SRC_DIR)/*.cpp)
OBJS        = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS)) $(GENERATED_OBJ)

LIBL_SRCS   = $(wildcard $(LIBL_DIR)/*.c)
LIBL_OBJS   = $(patsubst $(LIBL_DIR)/%.c, $(OBJ_DIR)/libl_%.o, $(LIBL_SRCS))

all: clang $(NAME) $(LIBL)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(GENERATED_SRC): $(TEMPLATE_FILE)
	@echo "🛠️ Generating C++ source from $< ..."
	@{ \
		echo '#include <cstddef>'; \
		\
		echo '// --- Embedded C Template (template/template.c) ---'; \
		xxd -i $(TEMPLATE_C_FILE) | sed 's/unsigned char template_template_c/extern const unsigned char TEMPLATE_C_START/g; s/unsigned int template_template_c_len/extern const std::size_t TEMPLATE_C_SIZE/g'; \
		\
		echo '// --- Embedded Python Template (template/template.py) ---'; \
		xxd -i $(TEMPLATE_PY_FILE) | sed 's/unsigned char template_template_py/extern const unsigned char TEMPLATE_PY_START/g; s/unsigned int template_template_py_len/extern const std::size_t TEMPLATE_PY_SIZE/g'; \
	} > $@

$(GENERATED_OBJ): $(GENERATED_SRC) | $(OBJ_DIR)
	@echo "Compiling embedded data: $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(LIBL): $(LIBL_OBJS)
	ar rcs $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/libl_%.o: $(LIBL_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

compile_commands.json:
	python3 generate_compile_commands.py

clang: compile_commands.json

clean:
	rm -rf $(OBJ_DIR)
	rm -f $(GENERATED_SRC)

fclean: clean
	rm -f $(NAME) $(LIBL)

re: fclean all

.PHONY: all clean fclean re clang
