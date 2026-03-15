NAME        = ft_lex
LIBL        = libl.a
LIBL_LD		= libl

CC          = clang
CFLAGS      = -Wall -Wextra -Werror -Isrc/libl

CXX         = clang++
CXXFLAGS    = -Wall -Wextra -Werror -std=c++17 -Iinclude

SRC_DIR     	= src
OBJ_DIR    	 	= obj
TEMPLATE_DIR 	= template
LIBL_DIR    	= src/libl

TEMPLATE_C_FILE = template/template.c
TEMPLATE_PY_FILE = template/template.py
GENERATED_SRC = $(SRC_DIR)/data.template.cpp
GENERATED_OBJ = $(OBJ_DIR)/data.template.o

SRCS        = $(wildcard $(SRC_DIR)/*.cpp)
OBJS        = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS)) $(GENERATED_OBJ)

LIBL_SRCS   = $(wildcard $(LIBL_DIR)/*.c)
LIBL_OBJS   = $(patsubst $(LIBL_DIR)/%.c, $(OBJ_DIR)/libl_%.o, $(LIBL_SRCS))

all: clang $(NAME) $(LIBL) $(LIBL_LD)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(GENERATED_SRC): $(TEMPLATE_C_FILE) $(TEMPLATE_PY_FILE)
	@echo "🛠️ Generating C++ source from templates..."
	@{ \
		echo '#include <cstddef>'; \
		echo ""; \
		echo "// --- Embedded C Template (template/template.c) ---"; \
		xxd -i $(TEMPLATE_C_FILE) | sed 's/unsigned char template_template_c/extern const unsigned char TEMPLATE_C_START/g; s/unsigned int template_template_c_len/extern const std::size_t TEMPLATE_C_SIZE/g'; \
		echo ""; \
		echo "// --- Embedded Python Template (template/template.py) ---"; \
		xxd -i $(TEMPLATE_PY_FILE) | sed 's/unsigned char template_template_py/extern const unsigned char TEMPLATE_PY_START/g; s/unsigned int template_template_py_len/extern const std::size_t TEMPLATE_PY_SIZE/g'; \
	} > $@

$(GENERATED_OBJ): $(GENERATED_SRC) | $(OBJ_DIR)
	@echo "Compiling embedded data: $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(LIBL): $(LIBL_OBJS)
	ar rcs $@ $^

$(LIBL_LD): $(LIBL)
	@echo "Symlinking $(LIBL) to $(LIBL_LD)..."
	ln -sf $(LIBL) $(LIBL_LD)

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

test: all
	@chmod +x tests/test_runner.sh
	@./tests/test_runner.sh

test-%: all
	@chmod +x tests/test_runner.sh
	@if [ -f "tests/parser/c/$*.l" ]; then \
		FT_LEX="./ft_lex" LIBL="libl.a" \
		bash -c 'source tests/test_runner.sh && run_test "tests/parser/c/$*.l" "tests/text/$*.txt"'; \
	elif [ -f "tests/parser/py/$*.l" ]; then \
		FT_LEX="./ft_lex" LIBL="libl.a" \
		bash -c 'source tests/test_runner.sh && run_python_test "tests/parser/py/$*.l" "tests/text/$*.txt"'; \
	else \
		echo "Test case $* not found in tests/parser/c or tests/parser/py"; \
		exit 1; \
	fi

test-clean:
	rm -f lex.yy.c lexer_test lexer out.txt test_yyless.l

fclean: clean test-clean
	rm -f $(NAME) $(LIBL) $(LIBL_LD)

re: fclean all

.PHONY: all clean fclean re clang test test-% test-clean
