/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/03 18:46:23 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/04 01:32:04 by bwerner          ###   ########.fr       */
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
	LEX_SPECIAL
};

enum e_token_type
{
	TKN_COMMAND,
	TKN_BUILTIN,
	TKN_FILE,
	TKN_PIPE,
	TKN_REDIRECT,
	TKN_VARIABLE
};

struct s_token
{
	size_t				index;
	char				*content;
	enum e_token_type	type;
	t_token				*next;
};

typedef struct s_minishell
{
	t_token				*head;
	char				*line;
}						t_minishell;

void		init_tokens(char *line, t_minishell *ms);

#endif
