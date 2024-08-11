
#include "Cgi.hpp"


/* ########## Constructor ########## */

Cgi::Cgi(void)
{
}

Cgi::Cgi(const Cgi &copy)
{
	if (&copy != this)
		*this = copy;
}

Cgi::~Cgi(void)
{
}


/* ########## Operator overload ########## */

Cgi &Cgi::operator=(const Cgi &other)
{
	if (&other != this)
	{
		
	}
	return (*this);
}


/* ########## Setter/Getter ########## */


/* ########## Member function ########## */

int	Cgi::getResponse(std::string &filepath, std::string &cgiPath, std::string &response)
{
	pid_t	pid;
	int		pipeFd[2];
	char	*args[3];

	int			bytesread;
	char		buffer[BUFFER_SIZE];

	if (cgiPath.empty())
	{
		args[0] = &filepath[0];
		args[1] = NULL;
	}
	else
	{
		args[0] = &cgiPath[0];
		args[1] = &filepath[0];
	}
	args[2] = NULL;
	if (pipe(pipeFd) < 0)
		throw (Logger::FunctionError("pipe", errno));
	if ((pid = fork()) < 0)
		throw (Logger::FunctionError("fork", errno));
	if (!pid)
	{
		dup2(pipeFd[1], STDOUT_FILENO);
		close(pipeFd[0]);
		close(pipeFd[1]);
		if (cgiPath.empty())
		{
			if (execve(filepath.c_str(), args, NULL) < 0)
			{
				std::ostringstream oss;
				oss << "Cannot execute file \"" << filepath << "\" : " << strerror(errno);
				Logger::error(oss.str().c_str());
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			if (execve(cgiPath.c_str(), args, NULL) < 0)
			{
				std::ostringstream oss;
				oss << "Cannot run \"" << args[0] << " " << args[1] << "\" : " << strerror(errno);
				Logger::error(oss.str().c_str());
				exit(EXIT_FAILURE);
			}
		}
	}
	else
	{
		close(pipeFd[1]);

		do
		{
			bytesread = read(pipeFd[0], buffer, BUFFER_SIZE);
			if (bytesread < 0)
				throw (Logger::FunctionError("read", -1));
			if (bytesread > 0)
				response.append(buffer, bytesread);
		} while (bytesread != 0);
		close(pipeFd[0]);
		int status;
		if (waitpid(pid, &status, 0) < 0)
			Logger::FunctionError("waitpid", errno);
		if (WIFEXITED(status) && WEXITSTATUS(status))
			return (-1);
	}
	return (0);	
}


/* ########## Exception ########## */


/* ########## Non-member function ########## */

