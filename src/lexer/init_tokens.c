/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_tokens.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 00:10:18 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/04 01:25:10 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_token	*token_last(t_token *lst)
{
	if (lst == NULL)
		return (NULL);
	while (lst->next != NULL)
	{
		lst = lst->next;
	}
	return (lst);
}

void	token_add_back(t_token **lst, t_token *new)
{
	t_token	*ptr;

	ptr = token_last(*lst);
	if (*lst == NULL)
		*lst = new;
	else
		ptr->next = new;
}

t_token	*token_new(char *content)
{
	t_token	*newtoken;

	newtoken = (t_token *)malloc(sizeof(t_token));
	if (newtoken == NULL)
		return (NULL);
	newtoken->content = content;
	newtoken->next = NULL;
	return (newtoken);
}

enum e_char_type	get_char_type(char c)
{
	if (ft_isspace(c))
		return (LEX_SPACE);
	if (c == '|' || c == '<' || c == '>')
		return (LEX_SPECIAL);
	return (LEX_WORD);
}

char	*get_next_token_content(char *line)
{
	static size_t		i;
	size_t				start;
	enum e_char_type	type;
	bool				in_single_quotes;
	bool				in_double_quotes;

	in_single_quotes = false;
	in_double_quotes = false;
	while (ft_isspace(line[i]))
		i++;
	start = i;
	type = get_char_type(line[i]);
	if (!line[i])
	{
		i = 0;
		return (NULL);
	}
	while (line[i])
	{
		if (!in_single_quotes && !in_double_quotes)
			if (type != get_char_type(line[i]))
				break ;
		if (line[i] == '\'' && !in_double_quotes)
			in_single_quotes = !in_single_quotes;
		if (line[i] == '\"' && !in_single_quotes)
			in_double_quotes = !in_double_quotes;
		i++;
	}
	return (ft_substr(line, start, i - start));
}

void	init_tokens(char *line, t_minishell *ms)
{
	t_token	*token;
	char	*content;

	token = ms->head;
	while (1)
	{
		content = get_next_token_content(line);
		if (!content)
			break ;
		token = token_new(content);
		token_add_back(&ms->head, token);
	}
	token = ms->head;
	while (token)
	{
		printf("%s, ", token->content);
		token = token->next;
	}
	printf("\n");
}
