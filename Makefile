TARGET = ft_ping

OBJ_PATH = obj
SRC_PATH = src
INC_PATH = inc

HEADERS = -I ./$(INC_PATH)

CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -g

REMOVE = rm -rf

SRC =	main.c \
		utils.c \
		arguments.c \
		icmp.c \
		interface.c

OBJ = $(addprefix $(OBJ_PATH)/, $(SRC:.c=.o))

all: $(TARGET)

$(TARGET): $(OBJ)
	@$(CC) $(OBJ) $(HEADERS) -o $@
	@echo "ft_ping is compiled!"

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c $(INC_PATH)/*.h
	@mkdir -p $(OBJ_PATH)
	@$(CC) $(CFLAGS) -c $< -o $@ $(HEADERS)

clean:
	@$(REMOVE) $(OBJ_PATH)
	@echo "ft_ping is cleaned!"

fclean:
	@$(REMOVE) $(OBJ_PATH)
	@$(REMOVE) $(TARGET)
	@echo "ft_ping is fcleaned!"

re: fclean all
