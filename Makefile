NAME				=	minishell
LIBFT				=	libft/libft.a
SRC					=	cleanup.c lexer/init_tokens.c minishell.c
OBJ					=	$(patsubst %.c, obj/%.o, $(SRC))
CFLAGS				=	-Wall -Wextra -Werror
LDFLAGS				=	-lreadline
CC					=	cc
COL_GREEN			= 	\033[32m
COL_RED				=	\033[31m
COL_YELLOW			= 	\033[38;2;214;189;28m
COL_PINK			= 	\033[95m
COL_DEFAULT			= 	\033[0m

# .SILENT:

all: $(LIBFT) $(NAME)

$(NAME): $(OBJ)
	echo "$(COL_YELLOW)Building $(NAME)...$(COL_DEFAULT)"
	$(CC) $(OBJ) $(LIBFT) -o $(NAME) $(LDFLAGS)
	echo "$(COL_GREEN)Successfully built $(NAME).$(COL_DEFAULT)"

$(LIBFT):
	echo "$(COL_YELLOW)Building $(LIBFT)...$(COL_DEFAULT)"
	$(MAKE) -C libft
	echo "$(COL_GREEN)Successfully built $(LIBFT).$(COL_DEFAULT)"

obj/%.o: src/%.c
	mkdir -p obj
	mkdir -p obj/lexer
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(MAKE) -C libft clean > /dev/null
	rm -rf obj
	echo "$(COL_GREEN)Object files have been removed.$(COL_DEFAULT)"

fclean: clean
	$(MAKE) -C libft fclean > /dev/null
	echo "$(COL_GREEN)$(LIBFT) has been removed.$(COL_DEFAULT)"
	rm -f $(NAME)
	echo "$(COL_GREEN)$(NAME) has been removed.$(COL_DEFAULT)"

t: all
	./$(NAME)

re: fclean all

.PHONY: all clean fclean re t
