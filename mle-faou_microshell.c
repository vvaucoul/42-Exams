#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>

#include <stdio.h>

int error(char *str)
{
	while (*str)
		write(STDERR_FILENO, str++, 1);
	return (1);
}

int exit_free(char **free_ptr)
{
	free(free_ptr);
	error("error: fatal\n");
	exit(1);
}

// Return a **char pointing just after the first pipe the func will meet
char **find_next_pipe(char **cmd)
{
	if (!cmd)
		return (NULL);
	int i = 0;
	while (cmd[i])
	{
		if (!strcmp(cmd[i], "|"))
			return (&cmd[i + 1]);
		i++;
	}
	return (NULL);
}

// Return the size of **cmd until cmd[i] == str (useful for ";" and "|")
int get_size_until_str(char **cmd, char *str)
{
	if (!cmd)
		return (0);
	int i = 0;
	while (cmd[i])
	{
		if (!strcmp(cmd[i], str))
			return (i);
		i++;
	}
	return (i);
}

// Executes a command and free free_ptr in the forked process if an error occured
void exec_cmd(char **cmd, char **env, char **free_ptr)
{
	pid_t pid = fork();

	if (pid == -1)
		exit_free(free_ptr);
	if (!pid) // child
	{
		if (execve(cmd[0], cmd, env) < 0)
		{
			error("error: cannot execute ");
			error(cmd[0]);
			error("\n");
			free(free_ptr); // Freeing the char** free_ptr previously allocated
			exit(1);
		}
	}
	waitpid(0, NULL, 0);
}

int execute(char **cmd, char **env)
{
	/* CASE NO PIPES */
	if (!find_next_pipe(cmd))
	{
		exec_cmd(cmd, env, cmd);
		return (0);
	}

	/* CASE PIPES */
	int fd_in = dup(STDIN_FILENO);
	if (fd_in < 0)
		exit_free(cmd);

	int nb_wait = 0;
	char **tmp = cmd;
	while (tmp)
	{
		int fd_pipe[2];
		if (pipe(fd_pipe) < 0)
			exit_free(cmd);

		pid_t pid = fork();
		if (pid == -1)
			exit_free(cmd);

		// child is executing commands
		if (!pid)
		{
			// Do the pipes and then execute only the part of the command before next pipe.
			// cmd is char** cmd previously allocated. char** tmp isn't allocated,
			// it's just a ptr to **cmd so no need to free it.
			if (dup2(fd_in, STDIN_FILENO) < 0)
				exit_free(cmd);
			if (find_next_pipe(tmp) && dup2(fd_pipe[1], STDOUT_FILENO) < 0) // If there is still a pipe after this command
				exit_free(cmd);

			// Closing all fds to avoid leaking files descriptors
			close(fd_in);
			close(fd_pipe[0]);
			close(fd_pipe[1]);

			// Replaces first pipe met with NULL (modifying **cmd in the child, **cmd in
			// the parent is still the same!) then executing the command
			tmp[get_size_until_str(tmp, "|")] = NULL;
			exec_cmd(tmp, env, cmd);

			// Freeing char** cmd in the fork process (still exists in the parent!)
			free(cmd);
			exit(0);
		}
		// Parent is just saving fd_pipe[0] for next child execution and correctly closing pipes
		else
		{
			if (dup2(fd_pipe[0], fd_in) < 0)	// Really important to protect syscalls using fd,
				exit_free(cmd);					// tests with wrong fds will be done during grademe
			close(fd_pipe[0]);
			close(fd_pipe[1]);
			++nb_wait;
			tmp = find_next_pipe(tmp); // Goes to the next command to be executed, just after first pipe met
		}
	}

	//closing last dup2 that happen in the last parent loop tour
	close(fd_in);

	//waiting for each command launched to bed executed
	while (nb_wait-- >= 0)
		waitpid(0, NULL, 0);
	return (0);
}

int main(int argc, char **argv, char **env)
{
	int i = 0;
	while (++i < argc)
	{
		// size of new **char until next ";". We start from i position
		int size = get_size_until_str(&argv[i], ";");
		if (size == 0) // case ";" ";" with nothing between them
			continue;

		// cmd = command until next ";".
		char **cmd;
		if (!(cmd = malloc(sizeof(*cmd) * (size + 1))))
			exit_free(NULL);

		int j = 0;
		while (j < size)
		{
			cmd[j] = argv[i + j];
			j++;
		}
		cmd[j] = NULL;

		// adds the number of elements copied, argv[i] will be on the next ";"
		i += size;

		if (!strcmp(cmd[0], "cd"))
		{
			int size_cmd = -1;
			while (cmd[++size_cmd]);
			if (size_cmd != 2)
				error("error: cd: bad arguments\n");
			else if (chdir(cmd[1]) < 0)
			{
				error("error: cd: cannot change directory to");
				error(cmd[1]);
				error("\n");
			}
		}
		else
			execute(cmd, env);

		free(cmd);
		cmd = NULL;
	}
	return (0);
}
