#!/bin/sh
# MICROSHELL EXAM-04 42 TESTER
# Made by vvaucoul

# COLORS

RESET="\033[0m"
BLACK="\033[30m"
RED="\033[31m"
GREEN="\033[32m"
YELLOW="\033[33m"
BLUE="\033[34m"
MAGENTA="\033[35m"
CYAN="\033[36m"
WHITE="\033[37m"

BOLDBLACK="\033[1m\033[30m"
BOLDRED="\033[1m\033[31m"
BOLDGREEN="\033[1m\033[32m"
BOLDYELLOW="\033[1m\033[33m"
BOLDBLUE="\033[1m\033[34m"
BOLDMAGENTA="\033[1m\033[35m"
BOLDCYAN="\033[1m\033[36m"
BOLDWHITE="\033[1m\033[37m"

# CONFIG

DISPLAY_FULL_OUTPUT=0

clear

if gcc *.c -Wall -Wextra -Werror -o microshell -I. ; then
		printf " $BOLDGREEN%s$RESET" "✓ [COMPILATION SUCCESS]"
else
		printf " $BOLDRED%s$RESET\n" "✗ [COMPILATION ERROR]"
		exit
fi


# START CMD to bash

function exec_test()
{
	sleep 0.02
	TEST1=$(./microshell $@)
	sleep 0.02
	TEST2=$(echo $@ "; exit" | bash --posix 2>&-)
	sleep 0.02

	if [ "$TEST1" == "$TEST2" ] ; then
		printf " $BOLDGREEN%s$RESET" "✓ "
	else
		printf " $BOLDRED%s$RESET" "✗ "
	fi
	printf "$BOLDBLUE \"$@\" $RESET"
	if [ $DISPLAY_FULL_OUTPUT == 1 ] ; then
		printf "$BOLDCYAN\n     MICROSHELL = [$TEST1]\n     BASH = [$TEST2]\n$RESET"
	if [ "$TEST1" != "$TEST2" ]; then
		echo
		echo
		printf $BOLDRED"Your output : \n%.20s\n$BOLDRED$TEST1\n%.20s$RESET\n" "-----------------------------------------" "-----------------------------------------"
		printf $BOLDGREEN"Expected output : \n%.20s\n$BOLDGREEN$TEST2\n%.20s$RESET\n" "-----------------------------------------" "-----------------------------------------"
	fi
	fi
	echo
	sleep 0.1
}

function exec_special()
{
	sleep 0.02
	TEST1=$1
	sleep 0.02
	TEST2=$2
	sleep 0.02

	if [ "$TEST1" == "$TEST2" ] ; then
		printf " $BOLDGREEN%s$RESET" "✓ "
	else
		printf " $BOLDRED%s$RESET" "✗ "
	fi
	printf "$BOLDBLUE \"$1\" $RESET"
}

# BASICS

printf "\n\n$BOLDMAGENTA%s$RESET\n\n" "[BASICS TESTS]"
sleep 1

# BASICS TESTS

exec_test /bin/echo test
exec_test /bin/echo -n test
exec_test /bin/ls
exec_test /bin/ls -l
exec_test /bin/ls -l -R

# PIPES TESTS

printf "\n\n$BOLDMAGENTA%s$RESET\n\n" "[PIPES TESTS]"
sleep 1

exec_test '/bin/ls | /usr/bin/grep microshell'
exec_test '/bin/ls | /usr/bin/grep microshell | /usr/bin/grep ll.c'
exec_test '/bin/ls | /usr/bin/grep microshell | /usr/bin/grep ll.c | /usr/bin/wc -l'
exec_test '/bin/echo 42 | /bin/cat -e | /bin/cat -e | /bin/cat -e'
exec_test '/bin/echo 42 | /bin/cat -e | /bin/cat -e | /bin/cat -e | /bin/cat -e | /bin/cat -e | /bin/cat -e | /bin/cat -e | /bin/cat -e | /bin/cat -e | /bin/cat -e | /bin/cat -e | /bin/cat -e | /bin/cat -e | /bin/cat -e | /bin/cat -e | /bin/cat -e | /bin/cat -e | /bin/cat -e | /bin/cat -e | /bin/cat -e | /bin/cat -e | /bin/cat -e | /bin/cat -e | /bin/cat -e | /bin/cat -e | /bin/cat -e | /bin/cat -e | /bin/cat -e | /bin/cat -e | /bin/cat -e | /bin/cat -e | /bin/cat -e | /bin/cat -e'
exec_test '/bin/ls | /bin/ls | /bin/ls | /bin/ls | /bin/ls | /bin/ls | /bin/ls | /bin/ls | /bin/ls | /bin/ls | /bin/ls | /bin/ls | /bin/ls | /bin/ls | /bin/ls | /bin/ls | /bin/ls | /bin/ls'
exec_test '/bin/echo test | /usr/bin/grep test | /bin/cat -e | usr/bin/cut -c3 | /usr/bin/wc -c'

# ; TESTS

printf "\n\n$BOLDMAGENTA%s$RESET\n\n" "[; TESTS]"
sleep 1

exec_test '/bin/ls ; /bin/ls'
exec_test '; ;'

# SPECIAL TESTS

A=$(./microshell /bin/ls "|" /usr/bin/grep microshell ";" /bin/echo i love my microshell)
B=$(ls | grep microshell ; echo i love my microshell)

exec_special $A $B

# cd TESTS

printf "\n\n$BOLDMAGENTA%s$RESET\n\n" "[CD TESTS]"
sleep 1

./microshell 'cd .'
./microshell "cd" "../." ";" "mkdir" "-p" "vv_test"

OUTPUT_VV=$(ls ../. | grep vv_test | wc -c)

echo $OUTPUT_VV

if [ $(ls ../. | grep vv_test | wc -c) != "0" ] ; then
	printf "$BOLDGREEN CD : Success$RESET\n"
else
	printf "$BOLDRED CD : Error, failed to check dir$RESET\n"
fi
#rm -rf ../vv_test

# SELF TESTS

# '; /bin/ls ; /bin/ls ;'
# '/bin/ls ; ;'
# '; ; /bin/ls ; ;'


rm microshell
