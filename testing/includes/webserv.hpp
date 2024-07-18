
#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <iostream>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <string.h>
# include <unistd.h>
# include <stdio.h>

# define SERVER_PORT 18000

void	client(char *arg);
void	server(void);

#endif