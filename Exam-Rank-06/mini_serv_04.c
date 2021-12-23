#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <fcntl.h>

typedef struct s_client
{
	int id;
	int fd;
	struct s_client *next;
}	t_client;

t_client *clients = NULL;
int sockfd = -1, g_id = 0;
fd_set current_socket, sock_read, sock_write;

char msg[42], str[42 * 4096], buff[42 * 4096 + 42], tmp[42 * 4096];

void	exit_fatal()
{
	write(2, "Fatal error\n", strlen("Fatal error\n"));
	if (sockfd != -1)
		close(sockfd);
	exit(1);
}

int get_id(int fd)
{
	t_client *tmp = clients;

	while (tmp)
	{
		if (tmp->fd == fd)
			return (tmp->id);
		tmp = tmp->next;
	}
	return (-1);
}

int get_max_fd()
{
	int max = sockfd;
	t_client *tmp = clients;

	while (tmp)
	{
		if (tmp->fd > max)
			max = tmp->fd;
		tmp = tmp->next;
	}
	return (max);
}

void send_all(int fd, char *str_to_send)
{
	t_client *tmp = clients;

	while (tmp)
	{
		if (tmp->fd != fd && FD_ISSET(tmp->fd, &sock_write))
		{
			if ((send(tmp->fd, str_to_send, strlen(str_to_send), 0) < 0))
				exit_fatal();
		}
		tmp = tmp->next;
	}
}

int add_client_to_list(int fd)
{
	t_client *tmp = clients;
	t_client *new = NULL;

	if (!(new = calloc(1, sizeof(t_client))))
		exit_fatal();

	new->id = g_id++;
	new->fd = fd;
	new->next = NULL;

	if (!clients)
		clients = new;
	else
	{
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new;
	}
	return (new->id);
}

int remove_client(int fd)
{
	t_client *tmp = clients;
	t_client *to_delete = NULL;
	int id = get_id(fd);

	if (tmp && tmp->fd == fd)
	{
		clients = tmp->next;
		free(tmp);
	}
	else
	{
		while (tmp && tmp->next && tmp->next->fd != fd)
			tmp = tmp->next;
		to_delete = tmp->next;
		tmp->next = tmp->next->next;
		free(to_delete);
	}
	return (id);
}

void add_client()
{
	struct sockaddr clientaddr;
	socklen_t len = sizeof(clientaddr);
	int client_fd;

	if ((client_fd = accept(sockfd, (struct sockaddr *) &clientaddr, &len)) < 0)
		exit_fatal();
	//fcntl(client_fd, F_SETFL, O_NONBLOCK);	// remove when connected
	sprintf(msg, "server: client %d just arrived\n", add_client_to_list(client_fd));
	send_all(client_fd, msg);
	FD_SET(client_fd, &current_socket);
}

void ex_msg(int fd)
{
	int i = 0;
	int j = 0;

	while (str[i])
	{
		tmp[j] = str[i];
		++j;
		if (str[i] == '\n')
		{
			sprintf(buff, "client %d: %s", get_id(fd), tmp);
			send_all(fd, buff);
			j = 0;
			bzero(&tmp, strlen(tmp));
			bzero(&buff, strlen(buff));
		}
		++i;
	}
	bzero(&str, strlen(str));
}

void loop_server()
{
	while (1)
	{
		sock_write = sock_read = current_socket;
		if ((select(get_max_fd() + 1, &sock_read, &sock_write, NULL, NULL)) < 0)
			continue;
		if (FD_ISSET(sockfd, &sock_read))
			add_client();

		for (int fd = 0; fd <= get_max_fd(); fd++) {
			if (FD_ISSET(fd, &sock_read))
			{
				int recv_ret = 1000;

				while (recv_ret == 1000 || str[strlen(str) - 1] != '\n')
				{
					recv_ret = recv(fd, str + strlen(str), 1000, 0);
					if (recv_ret <= 0)
						break;
				}

				if (recv_ret == 0)
				{
					bzero(&msg, sizeof(msg));
					sprintf(msg, "server: client %d just left\n", remove_client(fd));
					send_all(fd, msg);
					close(fd);
					FD_CLR(fd, &current_socket);
				}
				else
					ex_msg(fd);
			}
		}
	}
}

// INIT SERVER //

void init_values()
{
	FD_ZERO(&current_socket);
	FD_SET(sockfd, &current_socket);
}

void init_server(uint16_t port)
{
	struct sockaddr_in servaddr;

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(port);

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		exit_fatal();
	if (bind(sockfd, (const struct sockaddr *) &servaddr, sizeof(servaddr)) != 0)
		exit_fatal();
	if (listen(sockfd, 0) != 0)
		exit_fatal();
}

// MAIN //

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		write(2, "Wrong number of arguments\n", strlen("Wrong number of arguments\n"));
		exit(1);
	}
	init_server(atoi(argv[1]));
	init_values();
	loop_server();
}
