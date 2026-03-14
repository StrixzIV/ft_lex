#!/bin/bash

# Configuration
FT_LEX="./ft_lex"
LIBL="libl.a"
PARSER_DIR="tests/parser"
DATA_DIR="tests/text"
GEN_C="lex.yy.c"
OUT_BIN="lexer_test"

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Check if ft_lex and libl.a exist
check_deps() {
    if [ ! -f "$FT_LEX" ] || [ ! -f "$LIBL" ]; then
        echo -e "${RED}Error: $FT_LEX or $LIBL not found. Please run 'make' first.${NC}"
        return 1
    fi
    return 0
}

run_test() {
    local l_file="$1"
    local txt_file="$2"
    local name=$(basename "$l_file" .l)

    echo -n "Testing $name... "

    # 1. Generate C code
    $FT_LEX -o "$GEN_C" "$l_file" > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo -e "${RED}FAILED (Generation)${NC}"
        return 1
    fi

    # 2. Compile lexer
    clang -Wall -Wextra -Werror -Ilibl "$GEN_C" "$LIBL" -o "$OUT_BIN" > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo -e "${RED}FAILED (Compilation)${NC}"
        rm "$GEN_C" 2>/dev/null
        return 1
    fi

    # 3. Run and check
    if [ -f "$txt_file" ]; then
        ./"$OUT_BIN" < "$txt_file" > /dev/null 2>&1
        if [ $? -eq 0 ]; then
            echo -e "${GREEN}PASSED${NC}"
        else
            echo -e "${RED}FAILED (Runtime)${NC}"
        fi
    else
        echo -e "${GREEN}PASSED (No data)${NC}"
    fi

    # Cleanup
    rm "$GEN_C" "$OUT_BIN" 2>/dev/null
}

run_multi_test() {
    local name="$1"
    local txt_file="$2"
    shift 2
    local l_files=("$@")

    echo -n "Testing multi-file ($name)... "

    # 1. Generate C code
    $FT_LEX -o "$GEN_C" "${l_files[@]}" > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo -e "${RED}FAILED (Generation)${NC}"
        return 1
    fi

    # 2. Compile lexer
    clang -Wall -Wextra -Werror -Ilibl "$GEN_C" "$LIBL" -o "$OUT_BIN" > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo -e "${RED}FAILED (Compilation)${NC}"
        rm "$GEN_C" 2>/dev/null
        return 1
    fi

    # 3. Run and check
    if [ -f "$txt_file" ]; then
        ./"$OUT_BIN" < "$txt_file" > /dev/null 2>&1
        if [ $? -eq 0 ]; then
            echo -e "${GREEN}PASSED${NC}"
        else
            echo -e "${RED}FAILED (Runtime)${NC}"
        fi
    else
        echo -e "${GREEN}PASSED (No data)${NC}"
    fi

    # Cleanup
    rm "$GEN_C" "$OUT_BIN" 2>/dev/null
}

# Only run all tests if not being sourced
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    check_deps || exit 1
    run_test "$PARSER_DIR/minimal.l" "$DATA_DIR/minimal.txt"
    run_test "$PARSER_DIR/anchors.l" "$DATA_DIR/anchors.txt"
    run_test "$PARSER_DIR/charclass.l" "$DATA_DIR/charclass.txt"
    run_test "$PARSER_DIR/simple_test.l" ""
    run_test "$PARSER_DIR/valid.l" ""
    run_multi_test "combined" "$DATA_DIR/multi.txt" "$PARSER_DIR/multi_1.l" "$PARSER_DIR/multi_2.l"
    echo "Done."
fi
