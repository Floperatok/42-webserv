#pragma once

// DEFINES
# define BUFFER_SIZE 4096
# define MAX_CLIENT 1024
# define TIMEOUT	10000

// STANDARD LIBRARIES
# include <cstdlib>
# include <unistd.h>
# include <cctype>

// STRINGS
# include <string>
# include <iostream>
# include <fstream>
# include <sstream>
# include <cstring>

// ERRORS
# include <exception>
# include <errno.h>

// CONTAINERS
# include <algorithm>
# include <vector>
# include <stack>

// SERVER
# include <sys/poll.h>
# include <sys/socket.h>
# include <arpa/inet.h>

