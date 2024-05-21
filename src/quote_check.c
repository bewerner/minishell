/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote_check.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 20:26:32 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/21 21:15:28 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	in_single_quotes(char *str, size_t pos)
{
	bool	in_double_quotes;
	bool	in_single_quotes;
	size_t	i;

	in_double_quotes = false;
	in_single_quotes = false;
	i = 0;
	while (str[i] && i <= pos)
	{
		if (str[i] == '\"' && !in_single_quotes)
			in_double_quotes = !in_double_quotes;
		else if (str[i] == '\'' && !in_double_quotes)
			in_single_quotes = !in_single_quotes;
		i++;
	}
	return (in_single_quotes);
}

bool	in_double_quotes(char *str, size_t pos)
{
	bool	in_double_quotes;
	bool	in_single_quotes;
	size_t	i;

	in_double_quotes = false;
	in_single_quotes = false;
	i = 0;
	while (str[i] && i <= pos)
	{
		if (str[i] == '\"' && !in_single_quotes)
			in_double_quotes = !in_double_quotes;
		else if (str[i] == '\'' && !in_double_quotes)
			in_single_quotes = !in_single_quotes;
		i++;
	}
	return (in_double_quotes);
}

bool	in_quotes(char *str, size_t pos)
{
	bool	in_double_quotes;
	bool	in_single_quotes;
	size_t	i;

	in_double_quotes = false;
	in_single_quotes = false;
	i = 0;
	while (str[i] && i <= pos)
	{
		if (str[i] == '\"' && !in_single_quotes)
			in_double_quotes = !in_double_quotes;
		else if (str[i] == '\'' && !in_double_quotes)
			in_single_quotes = !in_single_quotes;
		i++;
	}
	return (in_single_quotes + in_double_quotes);
}

bool	is_removable_quote(char *str, size_t pos)
{
	bool	in_double_quotes;
	bool	in_single_quotes;
	size_t	i;

	if (str[pos] != '\'' && str[pos] != '\"')
		return (false);
	in_double_quotes = false;
	in_single_quotes = false;
	i = 0;
	while (str[i] && i < pos)
	{
		if (str[i] == '\"' && !in_single_quotes)
			in_double_quotes = !in_double_quotes;
		else if (str[i] == '\'' && !in_double_quotes)
			in_single_quotes = !in_single_quotes;
		i++;
	}
	if (str[i] && str[i] == '\"' && !in_single_quotes)
		return (true);
	else if (str[i] && str[i] == '\'' && !in_double_quotes)
		return (true);
	return (false);
}
