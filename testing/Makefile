
CC			=	c++
CFLAGS		=	-Wall -Werror -Wextra -std=c++98 -g
NAME		=	a.out

SRC_PATH	=	srcs/
SRC			=	main.cpp \

OBJ_PATH	=	objs/
OBJS		=	$(addprefix $(OBJ_PATH), $(SRC:.cpp=.o))

all:	$(NAME)

$(NAME):	$(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(OBJ_PATH)%.o:	$(SRC_PATH)%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_PATH)

fclean:
	rm -rf $(OBJ_PATH)
	rm -f $(NAME)

re: fclean all
