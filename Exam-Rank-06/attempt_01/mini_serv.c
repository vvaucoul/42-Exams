/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mini_serv.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/15 13:40:54 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/12/15 14:46:51 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

# include <stdio.h>

typedef struct s_client {
	int fd;
	int id;
}		t_client;

t_client clients[4096];
int		srv_fd = 0, max_id = 0;
fd_set	current_socket, sock_read, sock_write;
char	msg[42];
char	str[42 * 4096], tmp[42 * 4096], buf[42 * 4096 + 42];

// UTILS //

void 	exit_fatal()
{
	write(2, "Fatal error\n", strlen("Fatal error\n"));
	close(srv_fd);
	exit(1);
}

int get_max_fd()
{
	size_t i = 0;
	int max_fd = srv_fd;

	while (i < max_id)
	{
		if (clients[i].fd > max_fd)
			max_fd = clients[i].fd;
		++i;
	}
	return (max_fd);
}

void send_all(int fd, char *str)
{
	size_t i = 0;

	while (i < max_id)
	{
		if (clients[i].fd != fd && FD_ISSET(clients[i].fd, &sock_write))
		{
			if (send(clients[i].fd, str, strlen(str), 0) < 0)
				exit_fatal();
		}
		++i;
	}
}

int	 add_client_to_list(int fd)
{
	clients[max_id].fd = fd;
	clients[max_id].id = max_id + 1;
	++max_id;
	return (max_id);
}

void add_client()
{
	struct sockaddr_in clientaddr;
	socklen_t len = sizeof(clientaddr);
	int client_fd;

	if ((client_fd = accept(srv_fd, (struct sockaddr *) &clientaddr, &len)) < 0)
		exit_fatal();
	bzero(&msg, sizeof(msg));
	sprintf(msg, "server: client %d just arrived\n", add_client_to_list(client_fd));
	send_all(client_fd, msg);
	FD_SET(client_fd, &current_socket);
}

int remove_client(int fd)
{
	size_t i = 0;
	int id_removed = 0;

	while (i < max_id)
	{
		if (clients[i].fd == fd)
		{
			id_removed = clients[i].id;

			size_t j = i;
			while (j < max_id)
			{
				clients[j] = clients[j + 1];
				++j;
			}
			--max_id;
		}
		++i;
	}
	return (id_removed);
}

int get_id(int fd)
{
	size_t i = 0;

	while (i < max_id)
	{
		if (clients[i].fd == fd)
			return (clients[i].id);
		++i;
	}
	return (0);
}

void ex_msg(int fd)
{
	int i = 0;
	int j = 0;

	while (str[i])
	{
		tmp[j] = str[i];
		++j;
		if (str[i] != '\n')
		{
			sprintf(buf, "client %d: %s", get_id(fd), tmp);
		}
	}
}

// LOOP //

int loop_server()
{
	while (1)
	{
		sock_write = sock_read = current_socket;
		if (select( get_max_fd() + 1, &sock_read, &sock_write, NULL, NULL) < 0)
			continue;
		for (size_t fd = 0; fd <= get_max_fd(); fd++) {
			if (FD_ISSET(fd, &sock_read))
			{
				if (fd == srv_fd)
				{
					add_client();
					break;
				}
				else
				{
					if (recv(fd, str, sizeof(str), 0) <= 0)
					{
						bzero(&msg, sizeof(msg));
						sprintf(msg, "server: client %d just left\n", remove_client(fd));
						send_all(fd, msg);
						FD_CLR(fd, &current_socket);
						close(fd);
						break;
					}
					else
						ex_msg(fd);
				}
			}
		}
	}
}

// INIT //

int init_values()
{
	FD_ZERO(&current_socket);
	FD_SET(srv_fd, &current_socket);
	bzero(&tmp, sizeof(tmp));
	bzero(&buf, sizeof(buf));
	bzero(&str, sizeof(str));
}

int init_server(uint16_t port)
{
	struct sockaddr_in servaddr;

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(8081);

	if ((srv_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		exit_fatal();
	if (bind(srv_fd, (const struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
		exit_fatal();
	if (listen(srv_fd, 0) < 0)
		exit_fatal();
}

// MAIN //

int main(int argc, char const *argv[])
{
	if (argc != 2)
	{
		write(2, "Wrong number of arguments\n", strlen("Wrong number of arguments\n"));
		exit(1);
	}
	init_server(atoi(argv[1]));
	init_values();
	loop_server();
	return (0);
}
