/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/01 13:59:03 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/07/01 16:57:05 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MICROSHELL_H
# define MICROSHELL_H

# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <string.h>

# define INPUT 0
# define OUTPUT 1

typedef struct	s_arg
{
	char		*args[4096];
	int			is_pipe;
}				t_arg;

/*
** UTILS
*/

size_t	ft_strlen(char *str);
char	*ft_strdup(char *str);
size_t	ft_tablen(char **tab);
void	ft_putstr_fd(char *str, int fd);
size_t	ft_tab_occur(char **tab, char *str);
size_t	args_len(t_arg *args);

int		exit_fatal(char *str);
int		return_error(char *str, char *suffix, int err);


#endif
