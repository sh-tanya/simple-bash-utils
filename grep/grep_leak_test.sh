#!/bin/bash
SUCCESS=0
FAIL=0
COUNTER=0
DIFF=""

s21_command=(
    "./s21_grep"
)
sys_command=(
    "grep"
)

options=(
    "v"
    "c" 
    "l" 
    "n"
    "h"
    "o"
    "i"
    "s"
)

tests=(
    "t test_files/test_1_grep.txt FLAGS"
    "while grep.c FLAGS"
    "while grep.c grep.h Makefile FLAGS"
    "-e while -e #char grep.c grep.h Makefile FLAGS"
    "-e while -e #char grep.c FLAGS"
    "FLAGS no_file.txt"
    "-e sun -e sunset test_files/test_7_grep.txt"
    "-e sunset -e sun test_files/test_7_grep.txt"
)

manual=(
"-n for test_files/test_1_grep.txt test_files/test_2_grep.txt"
"-n for test_files/test_1_grep.txt"
"-n -e ^\} test_files/test_1_grep.txt"
"-ce ^int test_files/test_1_grep.txt test_files/test_2_grep.txt"
"-e ^int test_files/test_1_grep.txt"
"-nivh = test_files/test_1_grep.txt test_files/test_2_grep.txt"
"-e"
"-ie INT test_files/test_5_grep.txt"
"-echar test_files/test_1_grep.txt test_files/test_2_grep.txt"
"-ne = -e out test_files/test_5_grep.txt"
"-iv int test_files/test_5_grep.txt"
"-in int test_files/test_5_grep.txt"
"-c -l aboba test_files/test_1_grep.txt test_files/test_5_grep.txt"
"-v test_files/test_1_grep.txt -e ank"
"-noe ) test_files/test_5_grep.txt"
"-l for test_files/test_1_grep.txt test_files/test_2_grep.txt"
"-o -e int test_files/test_4_grep.txt"
"-e = -e out test_files/test_5_grep.txt"
"-noe it -e you -e the -e al -e was test_files/test_6_grep.txt"
"-e it -e you -e the -e al -e was test_files/test_6_grep.txt"
"-c -e . test_files/test_1_grep.txt -e '.'"
"-l for no_file.txt test_files/test_2_grep.txt"
"-e int -si no_file.txt s21_grep.c no_file2.txt s21_grep.h"
)

run_test() {
    param=$(echo "$@" | sed "s/FLAGS/$var/")
    valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose -q --log-file="${s21_command[@]}".log ./"${s21_command[@]}" $param > /dev/null
    leak=$(grep -ic -A10000 "LEAK SUMMARY:" "${s21_command[@]}".log || true)
    leak2=$(grep -ic -A10000 "ERROR SUMMARY: [^0]" "${s21_command[@]}".log || true)
    let "COUNTER++"
    if [ "$leak" -eq "0" ] && [ "$leak2" -eq "0" ]
    then
        let "SUCCESS++"
        echo "$COUNTER - Success $param"
    else
        let "FAIL++"
        echo "$COUNTER - Fail $param"
        exit 1
    fi
    rm -f "${s21_command[@]}".log "${sys_command[@]}".log
}

echo "========================"
echo "TESTS WITH NORMAL FLAGS"
echo "========================"
echo "%%%%%%%%%%%%%%%%%%%%%%%%"
echo "MANUAL TESTS"
echo "%%%%%%%%%%%%%%%%%%%%%%%%"
printf "\n"

for i in "${manual[@]}"
do
    var="-"
    run_test "$i"
done

printf "\n"
echo "%%%%%%%%%%%%%%%%%%%%%%%%"
echo "AUTOTESTS"
echo "%%%%%%%%%%%%%%%%%%%%%%%%"
printf "\n"
echo "________________________"
echo "1 PARAMETER"
echo "________________________"
printf "\n"

for var1 in "${options[@]}"
do
    for i in "${tests[@]}"
    do
        var="-$var1"
        run_test "$i"
    done
done

printf "\n"
echo "________________________"
echo "2 PARAMETERS"
echo "________________________"
printf "\n"

for var1 in "${options[@]}"
do
    for var2 in "${options[@]}"
    do
        if [ $var1 != $var2 ]
        then
            for i in "${tests[@]}"
            do
                var="-$var1 -$var2"
                run_test "$i"
            done
        fi
    done
done

printf "\n"
echo "________________________"
echo "3 PARAMETERS"
echo "________________________"
printf "\n"

for var1 in "${options[@]}"
do
    for var2 in "${options[@]}"
    do
        for var3 in "${options[@]}"
        do
            if [ $var1 != $var2 ] && [ $var2 != $var3 ] && [ $var1 != $var3 ]
            then
                for i in "${tests[@]}"
                do
                    var="-$var1 -$var2 -$var3"
                    run_test "$i"
                done
            fi
        done
    done
done

printf "\n"
echo "________________________"
echo "DOUBLE PARAMETER"
echo "________________________"
printf "\n"

for var1 in "${options[@]}"
do
    for var2 in "${options[@]}"
    do
        if [ $var1 != $var2 ]
        then
            for i in "${tests[@]}"
            do
                var="-$var1$var2"
                run_test "$i"
            done
        fi
    done
done

printf "\n"
echo "FAILED: $FAIL"
echo "SUCCESSFUL: $SUCCESS"
echo "ALL: $COUNTER"
printf "\n"