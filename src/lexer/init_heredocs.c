/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_heredocs.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/31 00:41:01 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/31 04:19:54 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char *get_heredoc_input(t_minishell *ms)
{
	char	*user_input;

	set_signal(SIGINT, sigint_handler_heredoc);
	user_input = readline("> ");
	if (!user_input && errno) // malloc error
		ms_error("readline", NULL, 1, ms);
	else if (!user_input) // ctrl + D (EOT)
		return (NULL);
	set_signal(SIGINT, sigint_handler);
	return (user_input);
}

void	read_heredoc(t_token *token, char *delimiter, t_minishell *ms)
{
	char	*content;
	t_input	*input;
	size_t	delimiter_len;

	delimiter_len = ft_strlen(delimiter);
	while (1)
	{
		content = get_heredoc_input(ms);
		if (!content || !ft_strncmp(delimiter, content, delimiter_len + 1))
		{
			free(content);
			token->heredoc = true;
			return ;
		}
		input = input_new(content);
		if (!input)
		{
			free(content);
			ms_error("read_heredoc", NULL, 1, ms);
			return ;
		}
		input_add_back(&token->head_heredoc, input);
	}
}

void	init_heredocs(t_token *token, t_minishell *ms)
{
	while (token && !ms->error)
	{
		if (token->type == TKN_HEREDOC && !token->heredoc && token->next)
		{
			remove_quotes(token->next, ms);
			read_heredoc(token, token->next->content, ms);
		}
		token = token->next;
	}
}
