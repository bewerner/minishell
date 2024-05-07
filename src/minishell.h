/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/03 18:46:23 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/07 21:02:30 by bwerner          ###   ########.fr       */
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
# include <sys/stat.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <signal.h>
# include <errno.h>
# include <dirent.h>
# include <string.h>
# include <termios.h>
# include <curses.h>
# include <stdbool.h>

typedef struct s_token	t_token;
typedef struct s_leaf	t_leaf;

enum e_char_type
{
	LEX_WORD,
	LEX_SPACE,
	LEX_LESS,
	LEX_GREATER,
	LEX_PIPE,
	LEX_AND
};

enum e_token_type
{
	TKN_WORD,
	TKN_PIPE,
	TKN_OR,
	TKN_AND,
	TKN_IN,
	TKN_OUT,
	TKN_APPEND,
	TKN_HEREDOC,
	TKN_HERESTRING
};

enum e_operator
{
	OP_NONE,
	OP_REDIRECT,
	OP_CONTROL
};

struct s_token
{
	char				*content;
	enum e_token_type	type;
	enum e_operator		operator;
	bool				is_literal;
	t_token				*next;
};

enum e_leaf_type
{
	LEAF_WORD,
	LEAF_PIPE,
	LEAF_OR,
	LEAF_AND,
	LEAF_IN,
	LEAF_OUT,
	LEAF_APPEND,
	LEAF_HEREDOC,
	LEAF_HERESTRING
};

struct s_leaf
{
	t_token				*head_token;
	size_t				size;
	char				**content;
	enum e_leaf_type	type;
	enum e_operator		operator;
	t_leaf				*next;
	t_leaf				*left;
	t_leaf				*right;
};

typedef struct s_minishell
{
	bool				debug;
	t_token				*head_token;
	t_leaf				*head_leaf;
	char				*line;
	uint8_t				exit_code;
	bool				error;
}						t_minishell;

// lexer/init_tokens.c
void		debug_print_tokens(t_token **head); //TEMP
void		init_tokens(char *line, t_minishell *ms);

// parser/init_leafs.c
void		init_leafs(t_minishell *ms);

// parser/init_tree.c
void		init_tree(t_minishell *ms);

// parser/rearrange_tokens.c
void		rearrange_tokens(t_minishell *ms);

// cleanup.c
void		cleanup(t_minishell *ms);

// error.c
void		ms_error(char *s1, char *s2, uint8_t exit_code, t_minishell *ms);

#endif
