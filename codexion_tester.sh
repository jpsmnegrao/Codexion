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

    printf "Testing %-40s : [%s] " "($3)" "$1"

    if [ "$2" == "helgrind" ]; then
        TIMEOUT=${4:-30}
        timeout "$TIMEOUT" valgrind --tool=helgrind --quiet ./codexion $1 > "$TMP_OUT" 2>&1
        EXIT=$?
        RAW_OUT=$(cat "$TMP_OUT")
        rm -f "$TMP_OUT"

        ERRORS=$(echo "$RAW_OUT" | grep "ERROR SUMMARY" | grep -oE "[0-9]+ errors" | head -1 | grep -oE "[0-9]+")
        if [ -z "$ERRORS" ] || [ "$ERRORS" == "0" ]; then
            echo -e "${GREEN}[OK]${RESET}"
        else
            echo -e "${RED}[FAIL]${RESET} ($ERRORS errors)"
        fi
        return
    fi

    timeout "$TIMEOUT" bash -c "./codexion $1" > "$TMP_OUT" 2>&1
    EXIT=$?

    RAW_OUT=$(cat "$TMP_OUT")
    CLEAN_OUT=$(grep -v "^==" "$TMP_OUT")
    rm -f "$TMP_OUT"

    BURNOUT_COUNT=$(echo "$CLEAN_OUT" | grep -c "burned out")

    if [ "$2" == "error" ]; then
        if [ $EXIT -ne 0 ] && [ $BURNOUT_COUNT -eq 0 ]; then
            echo -e "${GREEN}[OK] Rejected${RESET}"
        else
            echo -e "${RED}[FAIL]${RESET} Should have rejected"
        fi
        return
    fi

    if [ "$2" == "complete" ]; then
        if [ $BURNOUT_COUNT -eq 0 ] && [ $EXIT -eq 0 ]; then
            echo -e "${GREEN}[OK] Completed${RESET}"
        else
            echo -e "${RED}[FAIL]${RESET} Expected clean completion"
        fi
        return
    fi

    if [ "$2" == "burnout" ]; then
        if [ $BURNOUT_COUNT -ge 1 ]; then
            echo -e "${GREEN}[OK] Burnout detected${RESET}"
        else
            echo -e "${RED}[FAIL]${RESET} Expected burnout"
        fi
        return
    fi

    if [ $EXIT -eq 124 ]; then
        echo -e "${RED}[HANG]${RESET} (timed out at ${TIMEOUT}s)"
    else
        echo -e "${RED}[FAIL]${RESET}"
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
run_test "5 800 200 100 100 -1 50 fifo" "error" "Negative compile count"
run_test "5 800 200 100 100 5 -50 fifo" "error" "Negative cooldown"


echo -e "\n${YELLOW}--- BASIC COMPLETION ---${RESET}"
run_test "3 3000 100 100 100 3 50 fifo" "complete" "Basic fifo completion"
run_test "3 3000 100 100 100 3 50 edf" "complete" "Basic edf completion"
run_test "2 3000 200 100 100 5 50 fifo" "complete" "Two coders completion"
run_test "2 3000 200 100 100 5 50 edf" "complete" "Two coders completion edf"


echo -e "\n${YELLOW}--- HELGRIND (RACE DETECTION) ---${RESET}"
run_test "4 10000 100 50 50 4 200 fifo" "helgrind" "Helgrind fifo" "60"
run_test "4 10000 100 50 50 4 200 edf" "helgrind" "Helgrind edf" "60"
run_test "3 2000 200 100 100 3 50 fifo" "helgrind" "Helgrind burnout path" "60"
run_test "8 20000 80 40 40 8 50 edf" "helgrind" "Helgrind contention" "90"

echo -e "\n${YELLOW}--- EVALUATOR EASY TIER (large scale, no burnout) ---${RESET}"
run_test "200 60000 60 60 60 5 60 fifo" "complete" "200 coders fifo" "180"
run_test "200 60000 60 60 60 5 60 edf" "complete" "200 coders edf" "180"

echo -e "\n${YELLOW}--- EVALUATOR LESS EASY TIER (burnout edges, no duplication) ---${RESET}"
run_test "1 1000 60 60 60 5 60 fifo" "burnout" "1 coder dongle duplication check" "10"
run_test "2 240 60 60 60 5 60 fifo" "burnout" "2 coders exact boundary" "10"
run_test "2 300 60 60 60 5 60 fifo" "complete" "2 coders above boundary" "10"

echo -e "\n${YELLOW}--- EVALUATOR MEDIUM TIER (cooldown, fifo vs edf, serialization) ---${RESET}"
run_test "4 5000 60 60 60 4 300 fifo" "complete" "Cooldown stress fifo" "30"
run_test "4 5000 60 60 60 4 300 edf" "complete" "Cooldown stress edf" "30"
run_test "5 10000 60 60 60 10 60 fifo" "complete" "Fairness fifo" "40"
run_test "5 10000 60 60 60 10 60 edf" "complete" "Fairness edf" "40"