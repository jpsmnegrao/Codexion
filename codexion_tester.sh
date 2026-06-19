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
run_test "5 2000 100 50 50 5 0 fifo" "complete" "Zero cooldown fifo"
run_test "5 2000 100 50 50 5 0 edf" "complete" "Zero cooldown edf"

echo -e "\n${YELLOW}--- BURNOUT BOUNDARIES ---${RESET}"
run_test "1 800 200 100 100 5 50 fifo" "burnout" "Single coder fifo" "10"
run_test "1 800 200 100 100 5 50 edf" "burnout" "Single coder edf" "10"
run_test "2 400 200 100 100 5 50 fifo" "burnout" "2 coders exact boundary"
run_test "2 600 200 100 100 5 50 fifo" "complete" "2 coders above boundary" "10"
run_test "5 500 200 100 100 5 50 fifo" "burnout" "5 coders tight fifo"
run_test "5 500 200 100 100 5 50 edf" "burnout" "5 coders tight edf"

echo -e "\n${YELLOW}--- COOLDOWN AND CONTENTION ---${RESET}"
run_test "4 2000 100 50 50 4 200 fifo" "complete" "Cooldown reuse fifo" "20"
run_test "4 2000 100 50 50 4 200 edf" "complete" "Cooldown reuse edf" "20"
run_test "6 3000 120 60 60 6 300 fifo" "complete" "High contention fifo" "20"
run_test "6 3000 120 60 60 6 300 edf" "complete" "High contention edf" "25"

echo -e "\n${YELLOW}--- FAIRNESS CHECKS ---${RESET}"
run_test "3 2000 200 100 100 20 50 fifo" "complete" "FIFO starvation check" "20"
run_test "3 2000 200 100 100 20 50 edf" "complete" "EDF starvation check" "20"

echo -e "\n${YELLOW}--- STRESS TESTS ---${RESET}"
run_test "20 6000 20 10 10 5 5 fifo" "complete" "20 coders fifo" "30"
run_test "20 6000 20 10 10 5 5 edf" "complete" "20 coders edf" "30"
run_test "50 12000 10 5 5 3 5 fifo" "complete" "50 coders fifo" "30"
run_test "50 12000 10 5 5 3 5 edf" "complete" "50 coders edf" "30"
run_test "100 20000 10 5 5 2 5 fifo" "complete" "100 coders fifo" "45"
run_test "100 20000 10 5 5 2 5 edf" "complete" "100 coders edf" "45"

echo -e "\n${YELLOW}--- HELGRIND (RACE DETECTION) ---${RESET}"
run_test "4 2000 100 50 50 4 200 fifo" "helgrind" "4 coders fifo"
run_test "4 2000 100 50 50 4 200 edf" "helgrind" "4 coders edf"
run_test "1 800 200 100 100 5 50 fifo" "helgrind" "1 coder fifo"
run_test "8 5000 80 40 40 8 50 edf" "helgrind" "8 coders edf"