clang++ *.cpp -std=c++98 -Wall -Wextra -Werror -o cpp_01
./cpp_01 | cat -e
./cpp_01 | cat -e > result_2.txt
diff result_2.txt result.txt
rm result_2.txt && rm cpp_01
