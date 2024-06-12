/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_heredocs.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/31 00:41:01 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/12 20:03:34 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char *get_heredoc_input(t_minishell *ms)
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

// void	read_heredoc(t_token *token, char *delimiter, t_minishell *ms)
// {
// 	char	*content;
// 	t_input	*input;
// 	size_t	delimiter_len;

// 	delimiter_len = ft_strlen(delimiter);
// 	while (1)
// 	{
// 		content = get_heredoc_input(ms);
// 		if (!content || !ft_strncmp(delimiter, content, delimiter_len + 1))
// 		{
// 			free(content);
// 			token->heredoc_initiated = true;
// 			return ;
// 		}
// 		input = input_new(content);
// 		if (!input)
// 		{
// 			free(content);
// 			ms_error("read_heredoc", NULL, EXIT_FAILURE, ms);
// 			return ;
// 		}
// 		input_add_back(&token->head_heredoc, input);
// 	}
// }

void	init_heredocs(t_token *token, t_minishell *ms)
{
	bool	literal;

	while (token && !ms->error && !token->syntax_error)
	{
		if (token->type == TKN_HEREDOC && !token->head_heredoc
			&& token->next)
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
