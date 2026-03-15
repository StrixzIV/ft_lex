#!/bin/bash

# Configuration
FT_LEX="./ft_lex"
LIBL="libl.a"
PARSER_C_DIR="tests/parser/c"
PARSER_PY_DIR="tests/parser/py"
DATA_DIR="tests/text"
GEN_C="lex.yy.c"
GEN_PY="lex.yy.py"
OUT_BIN="lexer_test"

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
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
    local flags="$3"
    local name=$(basename "$l_file" .l)
    
    local flag_disp=""
    if [ ! -z "$flags" ]; then
        flag_disp=" [$flags]"
    fi

    echo -n "Testing $name$flag_disp... "

    # 1. Generate C code
    $FT_LEX $flags -o "$GEN_C" "$l_file" > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo -e "${RED}FAILED (Generation)${NC}"
        return 1
    fi

    # 2. Compile lexer
    clang -Wall -Wextra -Werror -Isrc/libl "$GEN_C" "$LIBL" -o "$OUT_BIN" > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo -e "${RED}FAILED (Compilation)${NC}"
        rm "$GEN_C" 2>/dev/null
        return 1
    fi

    # 3. Run and check
    if [ -f "$txt_file" ]; then
        local expected="$txt_file.expected"
        local actual="$txt_file.actual"
        local diff_out="$txt_file.diff"

        # Dynamically generate expected output using system flex for C targets
        if [ ! -f "$expected" ]; then
            flex -o expected.c "$l_file" > /dev/null 2>&1
            clang -w expected.c -ll -o expected_bin > /dev/null 2>&1
            if [ $? -eq 0 ]; then
                ./expected_bin < "$txt_file" > "$expected" 2>/dev/null
            fi
            rm expected.c expected_bin 2>/dev/null
        fi

        ./"$OUT_BIN" < "$txt_file" > "$actual" 2>&1
        if [ $? -eq 0 ]; then
            if [ -f "$expected" ]; then
                diff -u "$expected" "$actual" > "$diff_out"
                if [ $? -eq 0 ]; then
                    echo -e "${GREEN}PASSED (Output Matches)${NC}"
                    rm "$actual" "$diff_out" 2>/dev/null
                else
                    echo -e "${RED}FAILED (Output Mismatch)${NC}"
                    # Print the first 10 lines of the diff to avoid flooding the terminal
                    head -n 15 "$diff_out"
                fi
            else
                echo -e "${YELLOW}PASSED (Runtime OK, No expected output)${NC}"
                rm "$actual" 2>/dev/null
            fi
        else
            echo -e "${RED}FAILED (Runtime Crash)${NC}"
        fi
    else
        ./"$OUT_BIN" < /dev/null > /dev/null 2>&1
        echo -e "${GREEN}PASSED (Build only)${NC}"
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
    clang -Wall -Wextra -Werror -Isrc/libl "$GEN_C" "$LIBL" -o "$OUT_BIN" > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo -e "${RED}FAILED (Compilation)${NC}"
        rm "$GEN_C" 2>/dev/null
        return 1
    fi

    # 3. Run and check
    if [ -f "$txt_file" ]; then
        local expected="$txt_file.expected"
        local actual="$txt_file.actual"
        local diff_out="$txt_file.diff"

        # Dynamically generate expected output using system flex for C targets
        if [ ! -f "$expected" ]; then
            flex -o expected.c "$@" > /dev/null 2>&1
            clang -w expected.c -ll -o expected_bin > /dev/null 2>&1
            if [ $? -eq 0 ]; then
                ./expected_bin < "$txt_file" > "$expected" 2>/dev/null
            fi
            rm expected.c expected_bin 2>/dev/null
        fi

        ./"$OUT_BIN" < "$txt_file" > "$actual" 2>&1
        if [ $? -eq 0 ]; then
            if [ -f "$expected" ]; then
                diff -u "$expected" "$actual" > "$diff_out"
                if [ $? -eq 0 ]; then
                    echo -e "${GREEN}PASSED (Output Matches)${NC}"
                    rm "$actual" "$diff_out" 2>/dev/null
                else
                    echo -e "${RED}FAILED (Output Mismatch)${NC}"
                    head -n 15 "$diff_out"
                fi
            else
                echo -e "${YELLOW}PASSED (Runtime OK, No expected output)${NC}"
                rm "$actual" 2>/dev/null
            fi
        else
            echo -e "${RED}FAILED (Runtime Crash)${NC}"
        fi
    else
        ./"$OUT_BIN" < /dev/null > /dev/null 2>&1
        echo -e "${GREEN}PASSED (Build only)${NC}"
    fi

    # Cleanup
    rm "$GEN_C" "$OUT_BIN" 2>/dev/null
}

run_error_test() {
    local l_file="$1"
    local name=$(basename "$l_file" .l)

    echo -n "Testing error handling ($name)... "

    # Generate C code - EXPECTED TO FAIL
    $FT_LEX -o "$GEN_C" "$l_file" > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo -e "${GREEN}PASSED (Caught expected error)${NC}"
        return 0
    else
        echo -e "${RED}FAILED (Error not caught)${NC}"
        rm "$GEN_C" 2>/dev/null
        return 1
    fi
}

run_python_test() {
    local l_file="$1"
    local txt_file="$2"
    local name=$(basename "$l_file" .l)

    echo -n "Testing Python target ($name) (Polyglotism Bonus)... "

    # 1. Generate Python code
    $FT_LEX -l python -o "$GEN_PY" "$l_file" > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo -e "${RED}FAILED (Generation)${NC}"
        return 1
    fi

    # 2. Run Python lexer
    if [ -f "$txt_file" ]; then
        local expected="$txt_file.expected"
        local actual="$txt_file.actual"
        local diff_out="$txt_file.diff"

        python3 "$GEN_PY" "$txt_file" > "$actual" 2>&1
        if [ $? -eq 0 ]; then
            if [ -f "$expected" ]; then
                diff -u "$expected" "$actual" > "$diff_out"
                if [ $? -eq 0 ]; then
                    echo -e "${GREEN}PASSED (Output Matches)${NC}"
                    rm "$actual" "$diff_out" 2>/dev/null
                else
                    echo -e "${RED}FAILED (Output Mismatch)${NC}"
                    head -n 15 "$diff_out"
                fi
            else
                echo -e "${YELLOW}PASSED (Runtime OK, No expected output)${NC}"
                rm "$actual" 2>/dev/null
            fi
        else
            echo -e "${RED}FAILED (Runtime Crash)${NC}"
        fi
    else
        python3 "$GEN_PY" < /dev/null > /dev/null 2>&1
        echo -e "${GREEN}PASSED (Build only)${NC}"
    fi

    # Cleanup
    rm "$GEN_PY" 2>/dev/null
}

# Only run all tests if not being sourced
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    check_deps || exit 1
    
    echo -e "${YELLOW}--- C Passing Tests ---${NC}"
    for f in "$PARSER_C_DIR"/*.l; do
        if [[ $(basename "$f") != error_* && $(basename "$f") != missing_* && $(basename "$f") != multi_* ]]; then
            # Match data file if exists, otherwise empty
            base=$(basename "$f" .l)
            data=""
            if [ -f "$DATA_DIR/$base.txt" ]; then data="$DATA_DIR/$base.txt"; fi
            run_test "$f" "$data"
        fi
    done
    
    echo -e "\n${YELLOW}--- C Multi-file Tests ---${NC}"
    run_multi_test "combined" "$DATA_DIR/multi.txt" "$PARSER_C_DIR/multi_1.l" "$PARSER_C_DIR/multi_2.l"
    
    echo -e "\n${YELLOW}--- Python Target Tests ---${NC}"
    for f in "$PARSER_PY_DIR"/*.l; do
        base=$(basename "$f" .l)
        data=""
        if [ -f "$DATA_DIR/$base.txt" ]; then data="$DATA_DIR/$base.txt"; fi
        run_python_test "$f" "$data"
    done

    # Run tests for each compression flag
    for flag in "-Ce" "-Cm" "-f" "-F"; do
        echo -e "\n${YELLOW}--- C Passing Tests [Flag: $flag] ---${NC}"
        for f in "$PARSER_C_DIR"/*.l; do
            if [[ $(basename "$f") != error_* && $(basename "$f") != missing_* && $(basename "$f") != multi_* ]]; then
                base=$(basename "$f" .l)
                data=""
                if [ -f "$DATA_DIR/$base.txt" ]; then data="$DATA_DIR/$base.txt"; fi
                run_test "$f" "$data" "$flag"
            fi
        done
    done

    echo -e "\n${YELLOW}--- Compression Size Bonus Test ---${NC}"
    echo -n "Testing Compression Ratio (> 2x size reduction)... "
    # Compile uncompressed
    $FT_LEX -o "lex.yy.c.uncompressed" "$PARSER_C_DIR/compression_stress.l" > /dev/null 2>&1
    size_uncompressed=$(wc -c < "lex.yy.c.uncompressed")
    
    # Compile compressed (-Cm is maximum compression)
    $FT_LEX -Cm -o "lex.yy.c.compressed" "$PARSER_C_DIR/compression_stress.l" > /dev/null 2>&1
    size_compressed=$(wc -c < "lex.yy.c.compressed")

    if [ "$size_compressed" -gt 0 ] && [ "$size_uncompressed" -gt $(($size_compressed * 2)) ]; then
        compaction=$(awk "BEGIN {printf \"%.4fx\", $size_uncompressed / $size_compressed}")
        echo -e "${GREEN}PASSED ($size_uncompressed bytes -> $size_compressed bytes, Compaction: $compaction)${NC}"
    else
        compaction="N/A"
        if [ "$size_compressed" -gt 0 ]; then
            compaction=$(awk "BEGIN {printf \"%.4fx\", $size_uncompressed / $size_compressed}")
        fi
        echo -e "${RED}FAILED ($size_uncompressed bytes -> $size_compressed bytes, Compaction: $compaction, not >2x reduction)${NC}"
    fi
    rm "lex.yy.c.uncompressed" "lex.yy.c.compressed" 2>/dev/null


    echo -e "\n${YELLOW}--- Error Handling Tests ---${NC}"
    run_error_test "$PARSER_C_DIR/error_test.l"
    run_error_test "$PARSER_C_DIR/error_test_delim.l"
    run_error_test "$PARSER_C_DIR/error_test_fragment.l"
    run_error_test "$PARSER_C_DIR/error_test_sc.l"
    run_error_test "$PARSER_C_DIR/missing_delim.l"

    echo -e "\n${YELLOW}--- Done ---${NC}"
fi
