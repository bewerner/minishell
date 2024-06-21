/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_leaf.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 21:16:48 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/21 02:10:20 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

size_t	expand_question_mark(t_token *token, size_t i, t_minishell *ms)
{
	char	*value;
	char	*new_content;
	size_t	val_len;

	value = ft_itoa(ms->exit_code);
	if (!value)
	{
		ms_error("expand_question_mark: ft_itoa", NULL, EXIT_FAILURE, ms);
		return (0);
	}
	val_len = ft_strlen(value);
	update_remove(token, i, val_len, value);
	new_content = replace_substr(token->content, i, val_len + 1, value);
	free(value);
	if (!new_content)
	{
		ms_error("expand_question_mark: replace_substr",
			NULL, EXIT_FAILURE, ms);
		return (0);
	}
	set_token_contents(token, new_content);
	return (val_len);
}

size_t	expand_var(t_token *token, size_t i, size_t key_len, t_minishell *ms)
{
	char	*new_content;
	char	*value;
	size_t	val_len;

	if (key_len == 0)
		return (0);
	val_len = 0;
	value = get_env_value(ms->head_env, token->content + i + 1, key_len);
	if (value)
		val_len = ft_strlen(value);
	update_remove(token, i, key_len, value);
	new_content = replace_substr(token->content, i, key_len + 1, value);
	if (!new_content)
	{
		ms_error("expand_var: replace_substr", NULL, EXIT_FAILURE, ms);
		return (0);
	}
	set_token_contents(token, new_content);
	return (val_len);
}

void	expand_token(t_token *token, t_minishell *ms)
{
	size_t	i;
	size_t	key_len;
	size_t	val_len;

	i = 0;
	while (!ms->error && token->content[i])
	{
		val_len = 0;
		key_len = 0;
		if (token->content[i] == '$' && !in_single_quotes(token->content, i))
		{
			if (token->content[i + 1] == '?')
				val_len = expand_question_mark(token, i, ms);
			else
			{
				key_len = get_key_len(token, token->content, i + 1);
				val_len = expand_var(token, i, key_len, ms);
			}
			i += val_len;
		}
		if (val_len == 0 && key_len == 0)
			i++;
	}
}

void	remove_quotes(t_token *token, t_minishell *ms)
{
	size_t	i;
	size_t	len;

	if (ms->error || !token->remove)
		return ;
	i = 0;
	len = ft_strlen(token->content);
	while (token->content[i])
	{
		if (token->remove[i] == '1')
		{
			ft_memmove(token->content + i, token->content + i + 1, len - i);
			ft_memmove(token->remove + i, token->remove + i + 1, len - i);
		}
		else
			i++;
	}
}

void	expand_leaf(t_leaf *leaf, t_minishell *ms)
{
	t_token	*token;
	size_t	i;

	token = leaf->head_token;
	i = 0;
	while (i < leaf->size && !ms->error)
	{
		if (!token->split && leaf->type != LEAF_HEREDOC)
		{
			expand_token(token, ms);
			split_words(token, leaf);
		}
		if (leaf->type != LEAF_HEREDOC
			&& token_content_is_empty(token->content))
		{
			token = remove_token_from_leaf(token, leaf, ms);
			continue ;
		}
		remove_quotes(token, ms);
		token = token->next;
		i++;
	}
}
