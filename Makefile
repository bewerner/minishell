NAME				=	minishell
LIBFT				=	libft/libft.a
SRC					=	builtins/exec_cd.c builtins/exec_echo.c builtins/exec_env.c builtins/exec_pwd.c builtins/exec_unset.c builtins/exec_exit.c builtins/exec_export.c \
						debug/print.c debug/print_tree.c \
						executor/exec_cmd.c executor/exec_heredoc.c executor/exec_redirect.c executor/exec_tree.c executor/exec_word.c executor/path_utils.c \
						expander/expand_leaf.c expander/expander_utils.c expander/split_words.c \
						lexer/get_next_token_content.c lexer/init_heredocs.c lexer/init_tokens.c lexer/update_line.c \
						list_utils/env_utils.c list_utils/input_utils.c list_utils/leaf_utils.c list_utils/token_utils.c \
						parser/connect_pipes.c parser/connect_rest.c parser/rearrange_tokens.c parser/init_leafs.c parser/init_tree.c \
						minishell.c add_env.c cleanup.c error.c fd_utils.c init_input.c init_env.c quote_check.c signals.c
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
	mkdir -p obj/expander
	mkdir -p obj/builtins
	mkdir -p obj/list_utils
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
