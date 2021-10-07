/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/07 14:26:13 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/10/07 17:52:27 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>

#include <stdio.h>

size_t  nb_args = 0;
size_t  max_args = 0;
char    **renvp;

/*
**  UTILS
*/

size_t ft_tab_len(char **tab)
{
    size_t i = 0;

    while (tab[i])
        ++i;
    return (i);
}

size_t get_distance(char **cmd, char *str)
{
    size_t i = 0;
    if (!cmd)
        return (0);
    while (cmd[i])
    {
        if (!(strcmp(cmd[i], str)))
            return (i);
        ++i;
    }
    return (i);
}

int error(char *str)
{
    size_t i = 0;

	while (str[i])
	{
        write(STDERR_FILENO, &str[i], 1);
        ++i;
    }
	return (1);
}

char **find_next_pipe(char **cmd)
{
    if (!cmd)
        return (NULL);
    size_t i = 0;
    while (cmd[i])
    {
        if (!strcmp(cmd[i], "|"))
            return (&cmd[i + 1]);
        ++i;
    }
    return (NULL);
}

int exit_free(char **free_ptr)
{
	free(free_ptr);
	error("error: fatal\n");
	exit(1);
}

/*
**  CORE
*/

int builtin_cd(char **cmd)
{
    size_t size = 0;
    while (cmd[size])
        ++size;
    if (size != 2)
        error("error: cd: bad arguments\n");
    else if (chdir(cmd[1]) < 0)
    {
        error("error: cd: cannot change directory to");
        error(cmd[1]);
        error("\n");
    }
    return (0);
}

void exec_cmd(char **cmd, char **to_free)
{
    pid_t pid = fork();

    if (pid == -1)
        exit_free(to_free);
    else if (!pid)
    {
        if (execve(cmd[0], cmd, renvp) < 0)
        {
            error("error: cannot execute ");
			error(cmd[0]);
			error("\n");
			free(to_free);
			exit(1);
        }
    }
    waitpid(0, NULL, 0);
}

int execute(char **cmd)
{
    // no pipes
    if (!(find_next_pipe(cmd)))
    {
        exec_cmd(cmd, cmd);
        return (0);
    }

    // pipes

    int fd_in = dup(STDIN_FILENO);
    if (fd_in < 0)
        exit_free(cmd);

    size_t nb_childs = 0;
    char **tmp = cmd;

    while (tmp)
    {
        int fd_pipe[2];
        if (pipe(fd_pipe) < 0)
            exit_free(cmd);

        pid_t pid = fork();
        if (pid == -1)
            exit_free(cmd);
        else if (!pid)
        {
            if (dup2(fd_in, STDIN_FILENO) < 0)
                exit_free(cmd);
            if (find_next_pipe(tmp) && dup2(fd_pipe[1], STDOUT_FILENO) < 0)
                exit_free(cmd);

            close (fd_in);
            close(fd_pipe[0]);
            close(fd_pipe[1]);

            tmp[get_distance(tmp, "|")] = NULL;
            exec_cmd(tmp, cmd);
            free(cmd);
            exit(0);
        }
        else
        {
            if (dup2(fd_pipe[0], fd_in) < 0)
                exit_free(cmd);
            close(fd_pipe[0]);
            close(fd_pipe[1]);
            ++nb_childs;
            tmp = find_next_pipe(tmp);
        }
    }
    close (fd_in);
    while (nb_childs > 0)
    {
        waitpid(0, NULL, 0);
        --nb_childs;
    }
    return (0);
}

int parse_args(char **argv)
{
    size_t i = 1;

    while (i < max_args)
    {
        size_t dist = get_distance(&argv[i], ";");
        if (!dist)
            continue;
        char **current_cmds = NULL;

        if (!(current_cmds = malloc(sizeof(char *) * (dist + 1))))
            exit_free(NULL);

        size_t j = 0;
        while (j < dist)
        {
            current_cmds[j] = argv[i + j];
            ++j;
        }
        current_cmds[j] = NULL;

        i += dist;

        if (!strcmp(current_cmds[0], "cd"))
            builtin_cd(current_cmds);
        else
            execute(current_cmds);
        free(current_cmds);
        current_cmds = NULL;
    }
    return (0);
}

int main(int argc, char **argv, char **envp)
{
    renvp = envp;
    max_args = argc - 1;
    parse_args(argv);
    return (0);
}
