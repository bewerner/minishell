/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 02:06:04 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/21 02:19:28 by bwerner          ###   ########.fr       */
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
	while (val_len)
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
