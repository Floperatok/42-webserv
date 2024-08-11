
CC			=	c++
CFLAGS		=	-Wall -Werror -Wextra -std=c++98 -g
NAME		=	webserv

SRCS			=	main.cpp \
				Utils/Utils.cpp \
				Logs/Logger.cpp \
				Master/Master.cpp \
				Server/Server.cpp \
				Server/Location.cpp \
				Response/Response.cpp \
				Parser/CheckConfig.cpp \
				Parser/Parser.cpp \
				Cgi/Cgi.cpp

OBJ_PATH	=	objs/
OBJS		=	$(addprefix $(OBJ_PATH), $(SRCS:.cpp=.o))

UPLOADED_FILES = www/uploads/*

INCLUDES	=	-I Utils/ -I Logs/ -I Master/ -I Server/ -I Response/ -I Parser/ -I Cgi/

all:	$(NAME)

$(NAME):	$(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $(OBJS)

$(OBJ_PATH)%.o:	%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_PATH)
	rm -rf $(UPLOADED_FILES)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re