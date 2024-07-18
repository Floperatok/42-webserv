
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
	static const unsigned int	_BUFFSIZE = 10;
	const unsigned int			_serverport;
	int							_sockfd;
	struct sockaddr_in			_servaddr;
public:
	Server(const unsigned int serverport);
	Server(const Server &copy);
	Server &operator=(const Server &other);
	~Server(void);

	void	setup(void);
	void	start(void);

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
