
#include "Cgi.hpp"



/* ################################## CONSTRUCTORS ################################## */

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



/* ############################## OPERATOR'S OVERLOADS ############################## */

Cgi &Cgi::operator=(const Cgi &other)
{
	if (&other != this)
	{
		
	}
	return (*this);
}



/* ################################ MEMBER FUNCTIONS ################################ */

/*
 *	@brief Fills the request's content for CGI Scripts.
 *	@param filepath The CGI script's path.
 *	@param cgiPath The path of the binary needed to execute the script.
 *	@param content The content to fill.
 *	@param env The environnement to use.
 *	@return 0 in case of success, -1 in case of error. 
*/
int	Cgi::getContent(const std::string &filepath, const std::string &cgiPath, std::string &content, char **env)
{
	pid_t	pid;
	int		pipeFd[2];
	char	*args[3];

	int			bytesread;
	char		buffer[BUFFER_SIZE];

	if (cgiPath.empty())
	{
		args[0] = const_cast<char *>(filepath.c_str());
		args[1] = NULL;
	}
	else
	{
		args[0] = const_cast<char *>(cgiPath.c_str());
		args[1] = const_cast<char *>(filepath.c_str());
	}
	args[2] = NULL;

	if (pipe(pipeFd) < 0)
		throw (Logger::FunctionError("pipe", errno));
	if ((pid = fork()) < 0)
		throw (Logger::FunctionError("fork", errno));
	if (!pid)
	{
		dup2(pipeFd[1], STDOUT_FILENO);
		dup2(pipeFd[1], STDERR_FILENO);
		close(pipeFd[0]);
		close(pipeFd[1]);

		if (cgiPath.empty())
		{
			if (execve(filepath.c_str(), args, env) < 0)
			{
				Logger::error("Cannot execute file '" + filepath + "': " + strerror(errno));
				exit(EXIT_FAILURE);
			}
		}
		else
		{

			if (execve(cgiPath.c_str(), args, env) < 0)
			{
				Logger::error("Cannot run '" + cgiPath + ": " + strerror(errno));
				exit(EXIT_FAILURE);
			}
		}
	}
	else
	{
		close(pipeFd[1]);
		
		if ((bytesread = read(pipeFd[0], buffer, BUFFER_SIZE)) == 0)
		{
			close(pipeFd[0]);

			int pipeFd2[2];
			if (pipe(pipeFd2) < 0)
				throw (Logger::FunctionError("pipe", errno));
			
			pid_t	pid2 = fork();
			if (pid2 < 0)
					throw (Logger::FunctionError("fork", errno));
			if (pid2 == 0)
			{
				dup2(pipeFd2[1], STDOUT_FILENO);
				dup2(pipeFd2[1], STDERR_FILENO);
				close(pipeFd2[0]);
				close(pipeFd2[1]);

				if (execve("a.out", args, env) < 0)
				{
					Logger::error("Cannot run 'a.out': " + std::string(strerror(errno)) + ".");
					exit(EXIT_FAILURE);
				}
			}
			
			close(pipeFd2[1]);
			while ((bytesread = read(pipeFd2[0], buffer, BUFFER_SIZE)) > 0) {
				content.append(buffer, bytesread);
			}
			if (bytesread < 0)
				throw (Logger::FunctionError("read", -1));
			close(pipeFd2[0]);
			waitpid(pid2, NULL, 0);

			std::remove("a.out");
		}
		else if (bytesread < 0)
			throw (Logger::FunctionError("read", -1));
		else
		{
			do
			{
				if (bytesread < 0)
					throw (Logger::FunctionError("read", -1));
				if (bytesread > 0)
					content.append(buffer, bytesread);
				bytesread = read(pipeFd[0], buffer, BUFFER_SIZE);
			} while (bytesread != 0);
		}

		close(pipeFd[0]);

		int status;
		if (waitpid(pid, &status, 0) < 0)
			Logger::FunctionError("waitpid", errno);
		if (WIFEXITED(status) && WEXITSTATUS(status))
			return (-1);
	}
	return (0);	
}

/*
 *	@brief Dynamically generates cgi.html page.
 *	@return The page's content.
*/
std::string	Cgi::GenerateCgiPage()
{
	std::ostringstream	pageContent;

	pageContent << "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n"
				<< "\t<meta charset=\"UTF-8\">\n"
				<< "\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
				<< "\t<title>CGI Management</title>\n"
				<< "\t<link rel=\"stylesheet\" href=\"/style.css\">\n"
				<< "</head>\n<body>\n"
				<< "\t<header>\n"
				<< "\t\t<nav>\n"
				<< "\t\t\t<ul>\n"
				<< "\t\t\t\t<li><a href=\"/index.html\">Home</a></li>\n"
				<< "\t\t\t\t<li><a href=\"/post.html\">Upload a file</a></li>\n"
				<< "\t\t\t\t<li><a href=\"/cgi.html\">CGI Scripts</a></li>\n"
				<< "\t\t\t\t<li><a href=\"/nonexistent.html\">Error 404</a></li>\n"
				<< "\t\t\t</ul>\n"
				<< "\t\t</nav>\n"
				<< "\t</header>\n"
				<< "\t<main>\n"
				<< "\t\t<h1>CGI Scripts Management</h1>\n";

	pageContent << "\t\t<section>\n"
				<< "\t\t\t<h2>Upload a CGI Script</h2>\n"
				<< "\t\t\t<form action=\"/cgi-bin\" method=\"POST\" enctype=\"multipart/form-data\">\n"
				<< "\t\t\t\t<label for=\"cgi_file\">Select CGI Script to Upload:</label>\n"
				<< "\t\t\t\t<input type=\"file\" id=\"cgi_file\" name=\"cgi_file\">\n"
				<< "\t\t\t\t<br><br>\n"
				<< "\t\t\t\t<input type=\"submit\" value=\"Upload Script\">\n"
				<< "\t\t\t</form>\n"
				<< "\t\t</section>\n"
				<< "\t\t<br>\n";


	pageContent << "\t\t<section>\n"
				<< "\t\t\t<h2>Execute a CGI Script</h2>\n"
				<< "\t\t\t<form action=\"/cgi-bin/cgi_script\" method=\"GET\">\n"
				<< "\t\t\t\t<label for=\"script_select\">Select Script:</label>\n"
				<< "\t\t\t\t<select id=\"script_select\" name=\"script\">\n";

	DIR	*dir = opendir("www/cgi-bin");
	if (dir)
	{
		struct dirent	*entry;

		while ((entry = readdir(dir)))
			if (entry->d_type == DT_REG)
			{
				std::string	fileName = entry->d_name;
				pageContent << "\t\t\t\t\t<option value=\"" << fileName << "\">" << fileName << "</option>\n";
			}

		closedir(dir);
	}

	pageContent << "\t\t\t\t</select>\n"
				<< "\t\t\t\t<br><br>\n"
				<< "\t\t\t\t<input type=\"submit\" value=\"Execute Script\">\n"
				<< "\t\t\t</form>\n"
				<< "\t\t</section>\n"
				<< "\t</main>\n"
				<< "\t<footer>\n"
				<< "\t\t<p>&copy; 2024, by drenassi and nsalles.</p>\n"
				<< "\t</footer>\n"
				<< "</body>\n</html>";

	return pageContent.str();
}
