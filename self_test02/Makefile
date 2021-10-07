NAME = microshell

OBJDIR =	.obj

SRCS = 	microshell.c

OBJS =	$(addprefix $(OBJDIR)/, $(SRCS:.c=.o))
DPDCS =	$(OBJS:.o=.d)

CFLAGS = -Wall -Wextra -Werror -g -fsanitize=address

opti:
	@make -j all

all :
	@make $(NAME)

-include $(DPDCS)

$(NAME) : $(OBJS)
	@printf "[$(NAME)] "
	gcc $(CFLAGS) -o $(NAME) ${OBJS}
	@echo Compiled $(NAME) successfully !

$(OBJDIR)/%.o : %.c | $(OBJDIR)
	gcc $(CFLAGS) -c $< -o $@

$(OBJDIR) :
	@mkdir -p .obj

echo :
	@/bin/echo " please work normally | please... ; i beg you"
	@./microshell /bin/echo " please work normally | please... ; i beg you"
	@/bin/echo " please work normally |" " please... ; i beg you"
	@./microshell /bin/echo " please work normally |" " please... ; i beg you"
	@"/bin/echo" please work "normally |  please... ; i beg you"
	@./microshell "/bin/echo" please work "normally |  please... ; i beg you"

echo_mac :
	@/bin/echo " please work normally | please... ; i beg you"
	@./microshell /bin/echo " please work normally | please... ; i beg you"
	@/bin/echo " please work normally |" " please... ; i beg you"
	@./microshell /bin/echo " please work normally |" " please... ; i beg you"
	@"/bin/echo" please work "normally |  please... ; i beg you"
	@./microshell "/bin/echo" please work "normally |  please... ; i beg you"

cd :
	@echo "-------EXPECTED---------"
	@cd test ; /bin/pwd ; cd lvl2 ; /bin/cat "lol"
	@echo "--------OUTPUT---------"
	@./microshell cd test ";" /bin/pwd ";" cd lvl2 ";" /bin/cat "lol"
	@echo "-------EXPECTED---------"
	@cd test ; /bin/pwd ; cd lvl2 ; /bin/cat lol ; cd pouet ; ls
	@echo "--------OUTPUT---------"
	@./microshell cd test ";" /bin/pwd ";" cd lvl2 ";" /bin/cat lol ";" cd pouet ";" /bin/ls

cd_mac :
	@echo "-------EXPECTED---------"
	@cd test ; /bin/pwd ; cd lvl2 ; /bin/cat "lol"
	@echo "--------OUTPUT---------"
	@./microshell cd test ";" /bin/pwd ";" cd lvl2 ";" /bin/cat "lol"
	@echo "-------EXPECTED---------"
	@cd test ; /bin/pwd ; cd lvl2 ; /bin/cat lol ; cd pouet ; ls
	@echo "--------OUTPUT---------"
	@./microshell cd test ";" /bin/pwd ";" cd lvl2 ";" /bin/cat lol ";" cd pouet ";" /bin/ls

pipe :
	@echo "1)-------EXPECTED---------"
	@pwd | grep microshell ; echo i love my microshell
	@echo "1)--------OUTPUT---------"
	@./microshell /bin/pwd "|" /bin/grep microshell ";" /bin/echo i love my microshell
	@echo "2)-------EXPECTED---------"
	@pwd | grep nuke ; echo i love my microshell
	@echo "2)--------OUTPUT---------"
	@./microshell /bin/pwd "|" /bin/grep nuke ";" /bin/echo i love my microshell
	@echo "3)-------EXPECTED---------"
	@echo i love my microshell ; ls | grep microshell
	@echo "3)--------OUTPUT---------"
	@./microshell  /bin/echo i love my microshell ";" /bin/ls "|" /bin/grep microshell
	@echo "4)-------EXPECTED---------"
	@echo i love my microshell ; ls | grep microshell ;
	@echo "4)--------OUTPUT---------"
	@./microshell  /bin/echo i love my microshell ";" /bin/ls "|" /bin/grep microshell ";"
	@echo "5)-------EXPECTED---------"
	@cd test ; pwd ; cd .. ; ls | grep microshell | wc
	@echo "5)--------OUTPUT---------"
	@./microshell cd test ";" /bin/pwd ";" cd .. ";" /bin/ls "|" /bin/grep microshell "|" /usr/bin/wc
	@echo "6)-------EXPECTED---------"
	@cd test ; ls | grep microshell | wc; pwd ; cd .. ; pwd
	@echo "6)--------OUTPUT---------"
	@./microshell cd test ";" /bin/ls "|" /bin/grep microshell "|" /usr/bin/wc ";" /bin/pwd ";" cd .. ";" /bin/pwd

pipe_mac :
	@echo "1)-------EXPECTED---------"
	@pwd | grep microshell ; echo i love my microshell
	@echo "1)--------OUTPUT---------"
	@./microshell /bin/pwd "|" /bin/grep microshell ";" /bin/echo i love my microshell
	@echo "2)-------EXPECTED---------"
	@pwd | grep nuke ; echo i love my microshell
	@echo "2)--------OUTPUT---------"
	@./microshell /bin/pwd "|" /bin/grep nuke ";" /bin/echo i love my microshell
	@echo "3)-------EXPECTED---------"
	@echo i love my microshell ; ls | grep microshell
	@echo "3)--------OUTPUT---------"
	@./microshell  /bin/echo i love my microshell ";" /bin/ls "|" /bin/grep microshell
	@echo "4)-------EXPECTED---------"
	@echo i love my microshell ; ls | grep microshell ;
	@echo "4)--------OUTPUT---------"
	@./microshell  /bin/echo i love my microshell ";" /bin/ls "|" /bin/grep microshell ";"
	@echo "5)-------EXPECTED---------"
	@cd test ; pwd ; cd .. ; ls | grep microshell | wc
	@echo "5)--------OUTPUT---------"
	@./microshell cd test ";" /bin/pwd ";" cd .. ";" /bin/ls "|" /bin/grep microshell "|" /bin/wc
	@echo "6)-------EXPECTED---------"
	@cd test ; ls | grep microshell | wc; pwd ; cd .. ; pwd
	@echo "6)--------OUTPUT---------"
	@./microshell cd test ";" /bin/ls "|" /bin/grep microshell "|" /bin/wc ";" /bin/pwd ";" cd .. ";" /bin/pwd


clean :
	@rm -rf $(OBJDIR)

fclean : clean
	@rm -rf $(NAME)

re :
	@make fclean
	@make opti
