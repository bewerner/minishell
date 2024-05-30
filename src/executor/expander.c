/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 21:16:48 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/30 00:31:02 by bwerner          ###   ########.fr       */
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

char	*ft_replace_substr(char *s, size_t start, size_t len, char *replacement)
{
	char	*rt;
	size_t	rp_len;
	size_t	s_len;
	size_t	rt_len;

	// printf("replacement is: %s\n", replacement);
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
	// printf("new string is: %s\n", rt);
	return (rt);
}

char	*get_expanded_content(t_token *token, char *str, size_t key_pos, size_t *new_pos, t_minishell *ms)
{
	size_t	i;
	size_t	key_len;
	char	*value;

	i = key_pos;
	if(ft_isalpha(str[i]) || str[i] == '_' || str[i] == '?')
		i++;
	else
		return (NULL);
	while (str[i - 1] != '?' && str[i] && (ft_isalnum(str[i]) || str[i] == '_'))
		i++;
	key_len = i - key_pos;
	if (str[i - 1] != '?')
		value = get_env_value(ms->head_env, str + key_pos, key_len);
	else
		value = ft_itoa(ms->exit_code);
	if (!value)
		value = "";
	// if (!value)
	// 	return (NULL);
	*new_pos = key_pos - 1 + ft_strlen(value);
	if (token->remove)
		token->remove = ft_replace_substr(token->remove, key_pos - 1, key_len + 1, value);
	return (ft_replace_substr(str, key_pos - 1, key_len + 1, value));
}

// char	*get_expanded_content(t_token *token, char *str, size_t key_pos, size_t *new_pos, t_minishell *ms)
// {
// 	size_t	i;
// 	size_t	key_len;
// 	char	*value;

// 	i = key_pos;
// 	if(ft_isalpha(str[i]) || str[i] == '_')
// 		i++;
// 	else
// 		return (NULL);
// 	while (str[i] && (ft_isalnum(str[i]) || str[i] == '_'))
// 		i++;
// 	key_len = i - key_pos;
// 	value = get_env_value(ms->head_env, str + key_pos, key_len);
// 	if (!value)
// 		return (NULL);
// 	*new_pos = key_pos - 1 + ft_strlen(value);
// 	if (token->remove)
// 		token->remove = ft_replace_substr(token->remove, key_pos - 1, key_len + 1, value);
// 	return (ft_replace_substr(str, key_pos - 1, key_len + 1, value));
// }

void	expand_variables(t_token *token, t_minishell *ms)
{
	size_t	i;
	char	*expanded_content;
	size_t	new_pos;

	i = 0;
	while (token->content[i])
	{
		if (token->content[i] == '$' && !in_single_quotes(token->content, i))
		{
			expanded_content = get_expanded_content(token, token->content, i + 1, &new_pos, ms);
			if (expanded_content)
			{
				if (!token->original_content)
					token->original_content = token->content;
				else
					free(token->content);
				token->content = expanded_content;
				i = new_pos;
				continue ;
			}
			if (errno)
			{
				ms_error("expander", NULL, 1, ms);
				return ;
			}
		}
		i++;
	}
}

t_token	*split_token(t_token *token, size_t split_pos)
{
	char 	*content;
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
	t_token *old_next;

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
		get_previous_token(&ms->head_token, token)->next = rt;
	free(token->content);
	free(token->remove);
	free(token->original_content);
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
		if(token->remove[i] == '1')
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
	t_token *token;
	size_t	i;

	token = leaf->head_token;
	i = 0;
	while (i < leaf->size && !ms->error)
	{
		if (!token->split && leaf->type != LEAF_HEREDOC)
		{
			expand_variables(token, ms);
			split_words(token, leaf);
		}
		if (token_content_is_empty(token->content))
		{
			token = remove_token_from_leaf(token, leaf, ms);
			continue ;
		}
		remove_quotes(token, ms);
		token = token->next;
		i++;
	}
	// debug_print_leafs(&ms->head_leaf);
	// debug_print_tokens(&ms->head_token, 1);
}

// void	expand_leaf(t_leaf *leaf, t_minishell *ms)
// {
// 	t_token *token;
// 	size_t	i;

// 	token = leaf->head_token;
// 	i = 0;
// 	while (i < leaf->size && !ms->error)
// 	{
// 		if (!token->split)
// 		{
// 			expand_variables(token, ms);
// 			if (token_content_is_empty(token->content))
// 			{
// 				if (leaf->size > 1)
// 				{

// 					token = token_delete(&ms->head_token, &leaf->head_token, token); // leaf->head_token needs to be updated!
// 					leaf->size--;
// 					continue ;
// 				}
// 			}
// 			else
// 				split_words(token, leaf);
// 		}
// 		remove_quotes(token, ms);
// 		token = token->next;
// 		i++;
// 	}
// 	debug_print_leafs(&ms->head_leaf);
// 	debug_print_tokens(&ms->head_token, 1);
// }
