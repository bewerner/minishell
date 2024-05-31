NAME				=	minishell
LIBFT				=	libft/libft.a
SRC					=	minishell.c cleanup.c error.c init_input.c init_env.c quote_check.c signals.c \
						lexer/init_heredocs.c lexer/init_tokens.c \
						parser/rearrange_tokens.c parser/init_leafs.c parser/init_tree.c \
						debug/print.c debug/print_tree.c \
						executor/exec_redirect.c executor/exec_tree.c executor/exec_word.c executor/expander.c
OBJ					=	$(patsubst %.c, obj/%.o, $(SRC))
CFLAGS				=	-Wall -Wextra -Werror
FSANITIZE			=	-g -fsanitize=address
DEBUG				=	-g
LDFLAGS				=	-lreadline
CC					=	cc
COL_GREEN			= 	\033[32m
COL_RED				=	\033[31m
COL_YELLOW			= 	\033[38;2;214;189;28m
COL_PINK			= 	\033[95m
COL_DEFAULT			= 	\033[0m

.SILENT:

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
	mkdir -p obj/parser
	mkdir -p obj/debug
	mkdir -p obj/executor
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

re: fclean all

t: all
	./$(NAME)

f: LDFLAGS += $(FSANITIZE)
f: CFLAGS += $(FSANITIZE)
f: re

d: LDFLAGS += $(DEBUG)
d: CFLAGS += $(DEBUG)
d: re

.PHONY: all clean fclean re t f
