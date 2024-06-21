/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_tokens.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 00:10:18 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/21 02:53:22 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	set_token_type(t_token *token)
{
	size_t	len;

	len = ft_strlen(token->content);
	if (len == 2 && ft_strncmp(token->content, "<<", 2) == 0)
		token->type = TKN_HEREDOC;
	else if (len == 1 && ft_strncmp(token->content, "<", 1) == 0)
		token->type = TKN_IN;
	else if (len == 1 && ft_strncmp(token->content, ">", 1) == 0)
		token->type = TKN_OUT;
	else if (len == 2 && ft_strncmp(token->content, ">>", 2) == 0)
		token->type = TKN_APPEND;
	else if (len == 2 && ft_strncmp(token->content, "||", 2) == 0)
		token->type = TKN_OR;
	else if (len == 2 && ft_strncmp(token->content, "&&", 2) == 0)
		token->type = TKN_AND;
	else if (len == 1 && ft_strncmp(token->content, "|", 1) == 0)
		token->type = TKN_PIPE;
	else
		token->type = TKN_WORD;
}

t_operator	get_operator_type(t_token *token)
{
	if (token->type == TKN_IN
		|| token->type == TKN_OUT
		|| token->type == TKN_APPEND
		|| token->type == TKN_HEREDOC)
	{
		return (OP_REDIRECT);
	}
	else if (token->type == TKN_AND || token->type == TKN_OR)
		return (OP_LOGICAL);
	else if (token->type == TKN_PIPE)
		return (OP_PIPE);
	return (OP_NONE);
}

bool	mark_for_removal(t_token *token)
{
	size_t	i;

	if (!ft_strchr(token->content, '\'') && !ft_strchr(token->content, '\"'))
		return (true);
	token->remove = ft_strdup(token->content);
	if (!token->remove)
		return (false);
	i = 0;
	while (token->content[i])
	{
		if (is_removable_quote(token->content, i))
			token->remove[i] = '1';
		else
			token->remove[i] = '0';
		i++;
	}
	return (true);
}

void	init_tokens(t_minishell *ms)
{
	t_token	*token;
	char	*content;
	char	*line;

	free_unclosed_token(ms);
	free(ms->line);
	ms->line = get_input_content(ms->head_input, ms->head_input, ms);
	line = skip_tokenized_content(ms->line, ms->head_token, ms);
	while (line && !ms->error)
	{
		content = get_next_token_content(line, ms);
		if (!content)
			break ;
		token = token_new(content);
		if (!token || !mark_for_removal(token))
		{
			free(content);
			ms_error("init_tokens: lexer", NULL, EXIT_FAILURE, ms);
			break ;
		}
		token_add_back(&ms->head_token, token);
		set_token_type(token);
		token->line = ms->line_count;
		token->operator = get_operator_type(token);
	}
}
