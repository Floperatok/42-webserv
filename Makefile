
CC			=	c++
CFLAGS		=	-Wall -Werror -Wextra -std=c++98 -g
NAME		=	webserv

SRCS_PATH		= srcs/

SRCS			=	main.cpp \
				Utils/Utils.cpp \
				Logs/Logger.cpp \
				Master/Master.cpp \
				Server/Server.cpp \
				Server/Location.cpp \
				Response/Response.cpp \
				Parser/CheckConfig.cpp \
				Parser/Parser.cpp \
				Cgi/Cgi.cpp \
				Cookie/Cookie.cpp \
				Client/Client.cpp

OBJ_PATH	=	objs/
OBJS		=	$(addprefix $(OBJ_PATH), $(SRCS:.cpp=.o))

UPLOADED_FILES = www/uploads/* www/cgi-bin/*

INCLUDES	=	-I $(SRCS_PATH)Utils/ \
				-I $(SRCS_PATH)Logs/ \
				-I $(SRCS_PATH)Master/ \
				-I $(SRCS_PATH)Server/ \
				-I $(SRCS_PATH)Response/ \
				-I $(SRCS_PATH)Parser/ \
				-I $(SRCS_PATH)Cgi/ \
				-I $(SRCS_PATH)Cookie/ \
				-I $(SRCS_PATH)Client/ \

all:	$(NAME)

$(NAME):	$(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $(OBJS)

$(OBJ_PATH)%.o:	$(SRCS_PATH)%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_PATH)
	rm -rf $(UPLOADED_FILES)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re