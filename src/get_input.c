/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_input.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 23:53:55 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/09 23:54:38 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

size_t	get_last_char_pos(char *str)
{
	size_t	i;

	i = 0;
	while (str[i])
		i++;
	i--;
	while (ft_isspace(str[i]))
		i--;
	return (i);
}

char	*ms_strjoin(char *s1, const char *s2, char *s3)
{
	char	*tmp;
	char	*rt;

	tmp = ft_strjoin(s1, s2);
	if (!tmp)
		return (NULL);
	rt = ft_strjoin(tmp, s3);
	free(tmp);
	if (!rt)
		return (NULL);
	return (rt);
}

char	*join_input(char *s1, char *s2, t_minishell *ms)
{
	char	*input;

	if (!s2 && errno)
	{
		free(s1);
		return (NULL);
	}
	if (!s2)
	{
		ms->line_is_complete = true;
		return (s1);
	}
	if (!s2[0])
	{
		free(s2);
		return (s1);
	}
	input = ms_strjoin(s1, " ", s2);
	free(s1);
	free(s2);
	return (input);
}

// bash would perform syntax check at end of while loop;
char	*get_input(t_minishell *ms)
{
	char		*input;
	size_t		last_char_pos;
	t_char_type	last_char;

	input = NULL;
	while (!input || last_char == LEX_PIPE || last_char == LEX_AND)
	{
		if (!input)
			input = readline("minishell: ");
		else
			input = join_input(input, readline("> "), ms);
		if (ms->line_is_complete)
			return (input);
		if (!input && errno)
			ms_error("readline", NULL, 1, ms);
		if (!input || !input[0])
		{
			free(input);
			return (NULL);
		}
		last_char_pos = get_last_char_pos(input);
		last_char = get_char_type(input, last_char_pos);
	}
	return (input);
}
