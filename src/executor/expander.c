/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 21:16:48 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/14 03:13:39 by bwerner          ###   ########.fr       */
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

	printf("replacement is: %s\n", replacement);
	rp_len = ft_strlen(replacement);
	s_len = ft_strlen(s);
	rt_len = s_len - len + rp_len;
	rt = (char *)ft_calloc(rt_len + 1, sizeof(char));
	ft_strlcpy(rt, s, start + 1);
	ft_strlcat(rt, replacement, start + rp_len + 1);
	ft_strlcat(rt, s + start + len, rt_len + 1);
	// free(s);
	printf("new string is: %s\n", rt);
	return (rt);
}

char	*expand_variable(char *str, size_t key_pos, t_minishell *ms)
{
	size_t	i;
	size_t	key_len;
	char	*value;



	i = key_pos;
	if(ft_isalpha(str[i]) || str[i] == '_')
		i++;
	else
		return (str);
	while (str[i] && (ft_isalnum(str[i]) || str[i] == '_'))
		i++;
	key_len = i - key_pos;
	value = get_env_value(ms->head_env, str + key_pos, key_len);
	return (ft_replace_substr(str, key_pos - 1, key_len + 1, value));
}

char	*expand_variables(char *str, t_minishell *ms)
{
	size_t	i;
	bool	in_double_quotes;
	bool	in_single_quotes;

	i = 0;
	in_double_quotes = false;
	in_single_quotes = false;
	while (str[i])
	{
		if (str[i] == '\"' && !in_single_quotes)
			in_double_quotes = !in_double_quotes;
		else if (str[i] == '\'' && !in_double_quotes)
			in_single_quotes = !in_single_quotes;
		else if (str[i] == '$' && !in_single_quotes)
		{
			str = expand_variable(str, i + 1, ms);
		}
		i++;
	}
	return (str);
	// split_words();
}

void	expand_token(t_token *token, t_minishell *ms)
{
	token->content = expand_variables(token->content, ms);
	// remove_quotes()
}

void	expand_leaf(t_leaf *leaf, t_minishell *ms)
{
	t_token *token;
	size_t	i;

	token = leaf->head_token;
	i = 0;
	while (i < leaf->size)
	{
		expand_token(token, ms);
		token = token->next;
		i++;
	}
}
