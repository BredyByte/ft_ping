NAME = ft_ping

OBJ_PATH = obj
SRC_PATH = src
INC_PATH = inc

HEADERS = -I ./$(INC_PATH)

CC = gcc
CFLAGS = -Wall -Wextra -Werror

REMOVE = rm -rf

SRC =	\
		main.c \
		simple_ping.c \
		output.c \

OBJ = $(addprefix $(OBJ_PATH)/, $(SRC:.c=.o))

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(OBJ) $(HEADERS) -o $@
	@echo "ft_ping is compiled!"

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	@mkdir -p $(OBJ_PATH)
	@$(CC) $(CFLAGS) -c $< -o $@ $(HEADERS)

clean:
	@$(REMOVE) $(OBJ_PATH)
	@echo "ft_ping is cleaned!"

fclean:
	@$(REMOVE) $(OBJ_PATH)
	@$(REMOVE) $(NAME)
	@echo "ft_ping is fcleaned!"

re: fclean all
