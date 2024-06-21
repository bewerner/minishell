/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_words.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 01:53:41 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/21 02:13:02 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_token	*split_token(t_token *token, size_t split_pos)
{
	char	*content;
	t_token	*new_token;
	size_t	substr_start;
	size_t	substr_len;

	substr_start = split_pos;
	while (ft_isspace(token->content[substr_start]))
		substr_start++;
	substr_len = ft_strlen(token->content) - substr_start;
	content = ft_substr(token->content, substr_start, substr_len);
	new_token = token_new(content);
	if (token->remove)
		new_token->remove = ft_substr(token->remove, substr_start, substr_len);
	new_token->split = true;
	token->content[split_pos] = '\0';
	if (token->remove)
		token->remove[split_pos] = '\0';
	return (new_token);
}

void	token_insert(t_token *token, t_token *new_token)
{
	t_token	*old_next;

	old_next = token->next;
	token->next = new_token;
	new_token->next = old_next;
	new_token->type = token->type;
	new_token->operator = token->operator;
}

bool	token_content_is_empty(char *content)
{
	size_t	i;

	i = 0;
	if (!content)
		return (true);
	while (content[i])
	{
		if (!ft_isspace(content[i]))
			return (false);
		i++;
	}
	return (true);
}

// deletes a token and returns a pointer to the token now in it's place
t_token	*remove_token_from_leaf(t_token *token, t_leaf *leaf, t_minishell *ms)
{
	t_token	*rt;

	rt = token->next;
	if (token == leaf->head_token)
		leaf->head_token = token->next;
	if (token == ms->head_token)
		ms->head_token = token->next;
	else
	{
		get_previous_token(&ms->head_token, token)->original_next_content
		= token->original_content;
		get_previous_token(&ms->head_token, token)->next = rt;
		token->original_content = NULL;
	}
	free(token->content);
	free(token->remove);
	free(token->original_content);
	free(token->original_next_content);
	free(token);
	leaf->size--;
	if (!leaf->size)
		leaf->head_token = NULL;
	if (!leaf->size)
		leaf->executed = true;
	return (rt);
}

void	split_words(t_token *token, t_leaf *leaf)
{
	size_t	i;
	t_token	*new_token;

	while (token->content[0] && ft_isspace(token->content[0]))
	{
		ft_memmove(token->content, token->content + 1, \
			ft_strlen(token->content));
		if (token->remove)
			ft_memmove(token->remove, token->remove + 1, \
				ft_strlen(token->remove));
	}
	i = 0;
	while (token->content[i])
	{
		if (ft_isspace(token->content[i]) && !in_quotes(token->content, i))
		{
			new_token = split_token(token, i);
			token_insert(token, new_token);
			token = token->next;
			leaf->size++;
			i = 0;
			continue ;
		}
		i++;
	}
}
