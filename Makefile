
CC			=	c++
CFLAGS		=	-Wall -Werror -Wextra -std=c++98 -g
NAME		=	webserv

SRC_PATH	=	srcs/
SRC			=	main.cpp \
				Master.cpp \
				Server.cpp \
				Response.cpp

OBJ_PATH	=	objs/
OBJS		=	$(addprefix $(OBJ_PATH), $(SRC:.cpp=.o))

INC_PATH	=	includes/

all:	$(NAME)

$(NAME):	$(OBJS)
	$(CC) $(CFLAGS) -I $(INC_PATH) -o $@ $(OBJS)

$(OBJ_PATH)%.o:	$(SRC_PATH)%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I $(INC_PATH) -c $< -o $@

clean:
	rm -rf $(OBJ_PATH)

fclean:
	rm -rf $(OBJ_PATH)
	rm -f $(NAME)

re: fclean all
