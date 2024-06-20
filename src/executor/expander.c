/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 21:16:48 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/20 22:45:46 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*get_env_value(t_env *env, char *key, size_t key_len)
{
	if (!key_len)
		key_len = ft_strlen(key);
	while (env)
	{
		if (!ft_strncmp(env->key, key, key_len))
			if (env->key[key_len] == '\0')
				return (env->value);
		env = env->next;
	}
	return (NULL);
}

char	*replace_substr(char *s, size_t start, size_t len, char *replacement)
{
	char	*rt;
	size_t	rp_len;
	size_t	s_len;
	size_t	rt_len;

	if (!replacement)
		replacement = "";
	rp_len = ft_strlen(replacement);
	s_len = ft_strlen(s);
	rt_len = s_len - len + rp_len;
	rt = (char *)ft_calloc(rt_len + 1, sizeof(char));
	if (!rt)
		return (NULL);
	ft_strlcpy(rt, s, start + 1);
	ft_strlcat(rt, replacement, start + rp_len + 1);
	ft_strlcat(rt, s + start + len, rt_len + 1);
	return (rt);
}

void	update_remove(t_token *token, size_t i, size_t key_len, char *value)
{
	char	*tmp;
	size_t	val_len;

	if (!token->remove)
		return ;
	val_len = 0;
	if (value)
		val_len = ft_strlen(value);
	tmp = replace_substr(token->remove, i, key_len + 1, value);
	free(token->remove);
	token->remove = tmp;
	while(val_len)
	{
		token->remove[i] = '0';
		i++;
		val_len--;
	}
}

void	set_token_contents(t_token *token, char *new_content)
{
	if (!token->original_content)
		token->original_content = token->content;
	else
		free(token->content);
	token->content = new_content;
}

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
		ms_error("expand_question_mark: replace_substr", NULL, EXIT_FAILURE, ms);
		return (0);
	}
	set_token_contents(token, new_content);
	return (val_len);
}

size_t	get_key_len(t_token *token, char *str, size_t key_pos)
{
	size_t	i;

	i = key_pos;
	if (ft_isalpha(str[i]) || str[i] == '_')
		i++;
	else
	{
		if ((str[i] == '\'' || str[i] == '\"') && !in_quotes(str, i - 1))
			token->remove[i - 1] = '1';
		return (0);
	}
	while (str[i] && (ft_isalnum(str[i]) || str[i] == '_'))
		i++;
	return (i - key_pos);
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

void	set_prev_token_values(t_token *token, t_token *rt, t_minishell *ms)
{
	t_token	*prev_token;

	prev_token = get_previous_token(&ms->head_token, token);
	prev_token->original_next_content = token->original_content;
	prev_token->next = rt;
	token->original_content = NULL;
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
		set_prev_token_values(token, rt, ms);
	free(token->content);
	free(token->remove);
	free(token->original_content);
	free(token->original_next_content);
	free(token);
	leaf->size--;
	if (!leaf->size)
	{
		leaf->head_token = NULL;
		leaf->executed = true;
	}
	return (rt);
}

// t_token	*token_delete(t_token **ms_head, t_token **leaf_head, t_token *token)
// {
// 	t_token	*prev;

// 	if (token == *ms_head)
// 	{
// 		*ms_head = token->next;
// 		prev = token->next;
// 	}
// 	else
// 	{
// 		prev = *ms_head;
// 		while (prev->next != token)
// 			prev = prev->next;
// 		prev->next = token->next;
// 	}
// 	free(token->content);
// 	free(token->remove);
// 	free(token->original_content);
// 	free(token);
// 	return (prev);
// }

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
