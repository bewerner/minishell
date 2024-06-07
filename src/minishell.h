/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/03 18:46:23 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/07 01:20:57 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft/libft.h"
# include <readline/readline.h>
# include <readline/history.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/types.h>
# include <sys/param.h>
# include <sys/stat.h>
# include <sys/ioctl.h>
# include <sys/param.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <signal.h>
# include <errno.h>
# include <dirent.h>
# include <string.h>
# include <termios.h>
# include <curses.h>
# include <stdbool.h>

extern int				g_signal;

typedef struct s_input	t_input;
typedef struct s_token	t_token;
typedef struct s_leaf	t_leaf;

typedef enum e_char_type
{
	LEX_WORD,
	LEX_SPACE,
	LEX_LESS,
	LEX_GREATER,
	LEX_PIPE,
	LEX_AND
}	t_char_type;

typedef enum e_token_type
{
	TKN_WORD,
	TKN_PIPE,
	TKN_OR,
	TKN_AND,
	TKN_IN,
	TKN_OUT,
	TKN_APPEND,
	TKN_HEREDOC
}	t_token_type;

typedef enum e_operator
{
	OP_NONE,
	OP_REDIRECT,
	OP_PIPE,
	OP_LOGICAL
}	t_operator;

struct s_token
{
	char				*content;
	char				*remove;
	char				*original_content;
	enum e_token_type	type;
	enum e_operator		operator;
	bool				split;
	t_input				*head_heredoc;
	t_token				*next;
};

typedef enum e_leaf_type
{
	LEAF_WORD,
	LEAF_PIPE,
	LEAF_OR,
	LEAF_AND,
	LEAF_IN,
	LEAF_OUT,
	LEAF_APPEND,
	LEAF_HEREDOC
}	t_leaf_type;

struct s_leaf
{
	t_token				*head_token;
	size_t				size;
	char				**content;
	enum e_leaf_type	type;
	enum e_operator		operator;
	t_leaf				*next;
	t_leaf				*prev;
	t_leaf				*parent;
	t_leaf				*left;
	t_leaf				*right;
	bool				executed;
	bool				fork;
	pid_t				child_pid;
};

typedef struct s_env	t_env;

struct s_env
{
	char				*content;
	char				*key;
	char				*value;
	t_env				*prev;
	t_env				*next;
};

struct s_input
{
	char				*content;
	bool				complete;
	bool				literal;
	t_input				*next;
};

typedef struct s_minishell
{
	bool				debug;
	t_input				*head_input;
	t_token				*head_token;
	t_leaf				*head_leaf;
	t_leaf				*root;
	t_leaf				*first_leaf;
	t_env				*head_env;
	char				**envp;
	char				*line;
	bool				line_is_complete;
	uint8_t				exit_code;
	bool				error;
	int					fd_stdin_dup;
	int					fd_stdout_dup;
	int					close_in_child;
	int					close_in_parent;
	bool				in_pipeline;
}	t_minishell;

// builtins/exec_cd.c
void		exec_cd(t_leaf *leaf, t_token *token, t_minishell *ms);

// builtins/exec_echo.c
void		exec_echo(t_leaf *leaf, t_token *token, t_minishell *ms);

// builtins/exec_env.c
void		exec_env(t_minishell *ms);

// builtins/exec_exit.c
void		exec_exit(t_leaf *leaf, t_token *token, t_minishell *ms);

// builtins/exec_export.c
void		export_key(char *key, t_minishell *ms);
void		remove_duplicate_env(t_env *env, t_env *tail, t_minishell *ms);
void		exec_export(t_leaf *leaf, t_token *token, t_minishell *ms);

// builtins/exec_pwd.c
void		exec_pwd(t_minishell *ms);

// builtins/exec_unset.c
void		exec_unset(t_leaf *leaf, t_token *token, t_minishell *ms);

// debug/print.c
void		debug_print_tokens(t_token **head, size_t option);
void		debug_print_leafs(t_leaf **head);

// debug/print_tree.c
void		debug_print_tree(t_leaf *root, t_leaf *head);

// executor/exec_redirect.c
void		exec_redirect(t_leaf *leaf, t_minishell *ms);

// executor/exec_tree.c
void		exec_tree(t_leaf *leaf, t_minishell *ms);

// executor/exec_path.c
void		exec_word(t_leaf *leaf, t_minishell *ms);

// executor/expander.c
void		remove_quotes(t_token *token, t_minishell *ms);
void		expand_leaf(t_leaf *leaf, t_minishell *ms);

// lexer/init_heredocs.c
void		init_heredocs(t_token *token, t_minishell *ms);

// lexer/init_tokens.c
t_char_type	get_char_type(char *str, size_t pos);
void		init_tokens(t_minishell *ms);

// list_utils/env_utils.c
t_env		*get_env(char *key, t_env *env);
void		remove_env(t_env **head, t_env *env);
t_env		*env_last(t_env *lst);
void		env_add_back(t_env **lst, t_env *new);
t_env		*env_new(char *content);

// list_utils/input_utils.c
t_input		*input_last(t_input *lst);
void		input_add_back(t_input **lst, t_input *new);
t_input		*input_new(char *content);

// list_utils/leaf_utils.c
t_leaf		*leaf_last(t_leaf *lst);
void		leaf_add_back(t_leaf **lst, t_leaf *new);
t_leaf		*leaf_new(t_token *head);

// list_utils/token_utils.c
t_token		*get_previous_token(t_token **head, t_token *current);
t_token		*token_last(t_token *lst);
void		token_add_back(t_token **lst, t_token *new);
t_token		*token_new(char *content);

// parser/init_leafs.c
void		init_leafs(t_minishell *ms);

// parser/init_tree.c
void		init_tree(t_minishell *ms);

// parser/rearrange_tokens.c
void		rearrange_tokens(t_minishell *ms);

// cleanup.c
void		free_tokens(t_token **head);
void		cleanup(t_minishell *ms);
void		terminate(uint8_t exit_code, t_minishell *ms);

// error.c
void		ms_error(char *s1, char *s2, uint8_t exit_code, t_minishell *ms);

// init_env.c
void		update_envp(t_env *env, t_minishell *ms);
void		sort_env(t_env	*head);
void		add_env(char *str, t_minishell *ms);
void		init_env(char **envp, t_minishell *ms);

// init_input.c
void		free_inputs(t_input **head);
void		init_input(t_minishell *ms);

// quote_check.c
bool		in_single_quotes(char *str, size_t pos);
bool		in_double_quotes(char *str, size_t pos);
bool		in_quotes(char *str, size_t pos);
bool		is_unclosed(char *str);
bool		is_removable_quote(char *str, size_t pos);

// signals.c
void		sigint_handler_heredoc(int signum);
void		sigquit_handler_exec(int signum);
void		sigint_handler_exec(int signum);
void		sigint_handler(int signum);
void		set_signal(int signum, void (*handler_function)(int));
void		init_signals(t_minishell *ms);

#endif
