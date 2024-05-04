/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/03 18:46:23 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/04 23:31:20 by bwerner          ###   ########.fr       */
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

struct s_token
{
	char				*content;
	enum e_token_type	type;
	bool				is_literal;
	t_token				*next;
};

typedef struct s_minishell
{
	t_token				*head;
	char				*line;
}						t_minishell;

void		init_tokens(char *line, t_minishell *ms);
void		cleanup(t_minishell *ms);

#endif
