
#ifndef SERVER_HPP
# define SERVER_HPP

# include <sys/socket.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <errno.h>

# include <cstring>
# include <exception>
# include <string>
# include <sstream>
# include <iostream>

class Server
{
private:
	static const int	_BUFFSIZE = 4096;
	int					_port;
	int					_sockfd;
	struct sockaddr_in	_servaddr;
public:
	Server(void);
	Server(const Server &copy);
	~Server(void);

	Server &operator=(const Server &other);
	
	int			getPort(void) const;
	int			getSockfd(void) const;
	sockaddr_in	getServaddr(void) const;
	void		setPort(const int port);

	void	setup(void);
	void	listenSock(void);
	void	run(void);

	class CannotCreateSocket : public std::exception
	{
	private:
		std::string			_message;
	public:
		CannotCreateSocket(const int errnum);
		virtual ~CannotCreateSocket(void) throw();
		virtual const char	*what(void) const throw();
	};
	class CannotBindSocket : public std::exception
	{
	private:
		std::string			_message;
	public:
		CannotBindSocket(const unsigned int port, const int errnum);
		virtual ~CannotBindSocket(void) throw();
		const char *what(void) const throw();
	};
	class CannotListenSocket : public std::exception
	{
	private:
		std::string			_message;
	public:
		CannotListenSocket(const int errnum);
		virtual ~CannotListenSocket(void) throw();
		const char *what(void) const throw();
	};
	class ReadError : public std::exception
	{
	public:
		virtual const char *what(void) const throw();
	};
};

#endif
