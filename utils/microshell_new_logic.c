###########################
#	Logique 	PARSING	  #
###########################

[1] - Main: stoquer envp et max_args (argc - 1)
[2] - Loop Parsing:
		- Tant que i = 1 < max_args
			- récupérer la distance jusqu'au prochain ;
				- Distance:
					- Simple boucle, tant que tab[i]
					- Si !strcmp(tab[i], ";")
						- return i
			- Initiaiser un tableau tab de commandes
			 	- tant que j < dist
				- tab[j] = argv[i + j]
			- i += distance
			- Si tab[0] == cd
				- effectuer CD
			- Sinon
				- Executer COMMANDS (avec tab)
			- free tab
			- tab = NULL

###########################
#	Logique 	  CD	  #
###########################

[1] - Check la taille du tableau cmd
	- si taille != 2
		- error: cd bad arguments
	- sinon
[2] - Si chdir (cmd[1]) < 0
		- error: cd cannot change directory to: cmd[1] \n
	- return 0

###########################
#	  FUNCTIONS UTILS	  #
###########################

- ft_tab_len(char **tab)
	- taille du tableau

- get_distance(char **cmd, char *str) // cmd est généralement envoyé avec l'adresse (&argv[i])
	- i = 0
	- si tab existe
		- tant que cmd [i]
		- si !strcmp(cmd[i], str)
			return i
	- return i

- error(char *str)
	- afficher dans STDERR_FILENO, str
	- return 1

- find_next_pipe(char **cmd)
	- i = 0
	- tant que cmd[i]
		- si !strcmp(cmd[i], "|")
			- return (&cmd[i + 1])
	return NULL

- exit_free(char **free_ptr)
	- free free_ptr
	- error "fartal"
	exit(1)

###########################
#	  Logique EXEC CMD	  #
###########################

[1] - Fork
		- check l'état du fork, -1 ou 0

[2] - == -1
		- exit_free
[3] - == 0
		- execve(cmd[0], cmd, renvp)
		- si return execve < 0
			- error : cannot execute cmd[0] \n
			- free to_free
[4] - Waitpid(0, NULL, 0)

###########################
#	Logique   Execute	  #
###########################

[1] - Si find_next_pipe != NULL
		- EXEC CMD
		- return 0
	- sinon
		- EXEC PIPE

###########################
#	Logique     Pipes	  #
###########################

[1] - initialiser fd_in = dup(STDIN_FILENO) // Premier entrée de pipe (c'est le fd qui transmettra au autres pipes les infos)
		- si fd_in < 0
			exit_free
	- nb_childs = 0;

	- char **tmp = cmd // copy de cmd dans tmp pour ne pas modifier le pointeur de cmd
	- tant que tmp
		- fd_pipes[2] // creation d'un tuyau
		- Si pipe(fd_pipes) < 0
			- exit_free
		- fork
			- si pid == -1
				- exit_free
			- si pid == 0
				- si dup2(fd_in, STDIN_FILENO) < 0 // STDIN_FILENO devient fd_in
					- exit_free
				- find_next_pipe && dup2(fd_pipe[1], STDOUT_FILENO) < 0 // STDOUT_FILENO devient pipe[1]
					- exit_free

				- close (fd_in), close(fd_pipe[0]), close(fd_pipe[1]) // on close tout dans le child !

				- tmp[get_distance(tmp, "|")] = NULL; // on met le premier pipe trouvé à NULL, pour dire qu'il est utilisé
				exec_cmd(tmp, cmd) // puis on execute la commande
				free cmd
				exit(0);

			- sinon si pid != 0
				- dup2(fd_pipe[0], fd_in) < 0 // fd_in devient ensuite fd_pipe[0] donc, fd_in devient la sortie du précédent pipe
					- exit_free
				- close(fd_pipe[0]) close(fd_pipe[1]) // on ferme tout sauf le fd_in qui transmettra les infos au prochains pipes
				- ++nb_childs
				- tmp = find_next_pipe(tmp) // tmp pointe sur la prochaine commande apres le prochain pipe

	- close (fd_in) // on ferme le fd_in
	- tant que nb_childs > 0 // on attent que tout les pipes aient étés éxécutés
		- waitpid (0, NULL, 0)
		--nb_childs
	- return 0
