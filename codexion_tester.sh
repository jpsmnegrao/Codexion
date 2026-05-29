#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
CYAN='\033[0;36m'
YELLOW='\033[0;33m'
RESET='\033[0m'

if [ ! -f "./codexion" ]; then
    echo -e "${RED}Error: codexion not found.${RESET}"
    exit 1
fi

run_test() {
    # $1 = args, $2 = expected (error|burnout|complete|helgrind), $3 = description, $4 = optional timeout
    TMP_OUT=".tmp_codexion_out"
    TIMEOUT=${4:-5}

    if [ "$2" == "helgrind" ]; then
        TIMEOUT=${4:-30}
        timeout $TIMEOUT valgrind --tool=helgrind --quiet ./codexion $1 > $TMP_OUT 2>&1
    else
        timeout $TIMEOUT valgrind --quiet --leak-check=full --show-leak-kinds=all \
            --error-exitcode=42 ./codexion $1 > $TMP_OUT 2>&1
    fi
    EXIT=$?

    RAW_OUT=$(cat $TMP_OUT)
    CLEAN_OUT=$(grep -v "^==" $TMP_OUT)
    rm -f $TMP_OUT

    BURNOUT=$(echo "$CLEAN_OUT" | grep -c "burned out")

    printf "Testing %-40s : [%s] " "($3)" "$1"

    if [ "$2" == "helgrind" ]; then
		ERRORS=$(echo "$RAW_OUT" | grep "ERROR SUMMARY" | grep -oE "[0-9]+ errors" | head -1 | grep -oE "[0-9]+")
		if [ -z "$ERRORS" ] || [ "$ERRORS" == "0" ]; then
            echo -e "${GREEN}[OK]${RESET}"
        else
            echo -e "${RED}[FAIL]${RESET} ($ERRORS errors)"
        fi
    elif [ $EXIT -eq 42 ]; then
        echo -e "${RED}[LEAK]${RESET}"
    elif [ $EXIT -eq 124 ]; then
        echo -e "${RED}[HANG]${RESET} (timed out at ${TIMEOUT}s)"
    elif [ "$2" == "error" ]; then
        if [ $EXIT -ne 0 ] && [ $BURNOUT -eq 0 ]; then
            echo -e "${GREEN}[OK] Rejected${RESET}"
        else
            echo -e "${RED}[FAIL]${RESET} Should have rejected"
        fi
    elif [ "$2" == "burnout" ]; then
        if [ $BURNOUT -ge 1 ]; then
            echo -e "${GREEN}[OK] Burnout detected${RESET}"
        else
            echo -e "${RED}[FAIL]${RESET} Expected burnout"
        fi
    elif [ "$2" == "complete" ]; then
        if [ $BURNOUT -eq 0 ] && [ $EXIT -eq 0 ]; then
            echo -e "${GREEN}[OK] Completed${RESET}"
        else
            echo -e "${RED}[FAIL]${RESET} Expected clean completion"
        fi
    fi
}

echo -e "\n${YELLOW}--- INPUT VALIDATION ---${RESET}"
run_test "" "error" "No arguments"
run_test "5 800 200 100 100 5 50" "error" "Too few arguments"
run_test "5 800 200 100 100 5 50 fifo extra" "error" "Too many arguments"
run_test "0 800 200 100 100 5 50 fifo" "error" "Zero coders"
run_test "-5 800 200 100 100 5 50 fifo" "error" "Negative coders"
run_test "5 -800 200 100 100 5 50 fifo" "error" "Negative burnout"
run_test "5 abc 200 100 100 5 50 fifo" "error" "Non-integer"
run_test "5 800 200 100 100 5 50 random" "error" "Bad scheduler"
run_test "5 800 200 100 100 5 50 FIFO" "error" "Wrong case scheduler"
run_test "99999999999 800 200 100 100 5 50 fifo" "error" "Overflow"

echo -e "\n${YELLOW}--- BURNOUT CASES ---${RESET}"
run_test "1 800 200 100 100 5 50 fifo" "burnout" "Single coder fifo"
run_test "1 800 200 100 100 5 50 edf" "burnout" "Single coder edf"
run_test "5 100 100 100 100 10 50 fifo" "burnout" "Tight burnout fifo"
run_test "5 100 100 100 100 10 50 edf" "burnout" "Tight burnout edf"
run_test "4 410 200 100 100 5 50 fifo" "burnout" "Borderline burnout"

echo -e "\n${YELLOW}--- CLEAN COMPLETION ---${RESET}"
run_test "2 1000 200 100 100 3 50 fifo" "complete" "Small fifo"
run_test "2 1000 200 100 100 3 50 edf" "complete" "Small edf"
run_test "5 1500 200 100 100 5 50 fifo" "complete" "5 coders fifo" "10"
run_test "5 1500 200 100 100 5 50 edf" "complete" "5 coders edf" "10"

echo -e "\n${YELLOW}--- STRESS TESTS ---${RESET}"
run_test "50 5000 10 5 5 3 5 fifo" "complete" "50 coders fifo" "10"
run_test "50 5000 10 5 5 3 5 edf" "complete" "50 coders edf" "10"
run_test "100 10000 10 5 5 2 5 fifo" "complete" "100 coders fifo" "15"
run_test "200 20000 10 5 5 2 5 edf" "complete" "200 coders edf" "30"

echo -e "\n${YELLOW}--- HELGRIND (race detection) ---${RESET}"
run_test "5 800 200 100 100 5 50 fifo" "helgrind" "5 coders fifo"
run_test "5 800 200 100 100 5 50 edf" "helgrind" "5 coders edf"
run_test "2 1000 200 100 100 3 50 fifo" "helgrind" "2 coders fifo"
run_test "1 800 200 100 100 5 50 fifo" "helgrind" "1 coder fifo"

echo -e "\n${YELLOW}--- EDGE CASES ---${RESET}"
# Lower bound: 1 coder (must burn out, can't get 2 dongles)
run_test "1 800 200 100 100 5 50 fifo" "burnout" "1 coder lower bound"

# Lower bound: total task time equals burnout (compile+debug+refactor = burnout)
run_test "5 400 200 100 100 5 50 fifo" "burnout" "Total time = burnout"
run_test "5 400 200 100 100 5 50 edf" "burnout" "Total time = burnout edf"

# Large coder counts
run_test "100 10000 10 5 5 5 5 fifo" "complete" "100 coders many compiles" "30"
run_test "100 10000 10 5 5 5 5 edf" "complete" "100 coders many compiles edf" "30"

# Many compilations per coder
run_test "5 2000 50 25 25 50 25 fifo" "complete" "5 coders 50 compiles" "30"
run_test "5 2000 50 25 25 50 25 edf" "complete" "5 coders 50 compiles edf" "30"

# Long run - accumulating time errors
run_test "10 5000 20 10 10 100 10 fifo" "complete" "10 coders 100 compiles" "60"
run_test "10 5000 20 10 10 100 10 edf" "complete" "10 coders 100 compiles edf" "60"

# Pushing limits: large coders + many compiles
run_test "50 30000 10 5 5 20 5 fifo" "complete" "50 coders 20 compiles" "60"
run_test "50 30000 10 5 5 20 5 edf" "complete" "50 coders 20 compiles edf" "60"