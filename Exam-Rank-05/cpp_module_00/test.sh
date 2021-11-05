clang++ *.cpp -std=c++98 -Wall -Wextra -Werror -o cpp_00
./cpp_00 | cat -e
./cpp_00 | cat -e > result_2.txt
diff result_2.txt result.txt
rm result_2.txt && rm cpp_00
