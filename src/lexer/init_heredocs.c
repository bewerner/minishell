/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_heredocs.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/31 00:41:01 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/18 01:15:28 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*get_heredoc_input(t_minishell *ms)
{
	char	*line;

	set_signal(SIGINT, sigint_handler_heredoc);
	if (ms->interactive)
		line = readline("> ");
	else
		line = non_interactive_readline(ms);
	if (!line && errno && errno != ENOTTY) // malloc error
		ms_error("readline", NULL, EXIT_FAILURE, ms);
	else if (!line) // ctrl + D (EOT)
		return (NULL);
	set_signal(SIGINT, sigint_handler);
	return (line);
}

void	read_heredoc(t_token *token, char *delimiter, bool literal, t_minishell *ms)
{
	t_input	*input;
	size_t	delimiter_len;

	delimiter_len = ft_strlen(delimiter);
	while (1)
	{
		input = input_new(NULL);
		if (!input)
		{
			ms_error("read_heredoc", NULL, EXIT_FAILURE, ms);
			return ;
		}
		input_add_back(&token->head_heredoc, input);
		input->literal = literal;
		input->content = get_heredoc_input(ms);
		if (!input->content
			|| !ft_strncmp(delimiter, input->content, delimiter_len + 1))
		{
			free(input->content);
			input->content = NULL;
			return ;
		}
	}
}

void	init_heredocs(t_token *token, t_minishell *ms)
{
	bool	literal;

	while (token && !ms->error && !token->syntax_error)
	{
		if (token->type == TKN_HEREDOC && !token->head_heredoc
			&& token->next && token->next->operator == OP_NONE)
		{
			literal = false;
			if (ft_strchr(token->next->content, '\'')
				|| ft_strchr(token->next->content, '\"'))
			{
				literal = true;
				token->next->original_length = ft_strlen(token->next->content);
				remove_quotes(token->next, ms);
			}
			read_heredoc(token, token->next->content, literal, ms);
		}
		token = token->next;
	}
}
