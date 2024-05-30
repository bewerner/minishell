/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_tokens.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 00:10:18 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/30 06:13:26 by bwerner          ###   ########.fr       */
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

	newtoken = (t_token *)ft_calloc(1, sizeof(t_token));
	if (newtoken == NULL)
		return (NULL);
	newtoken->content = content;
	return (newtoken);
}

t_char_type	get_char_type(char *str, size_t pos)
{
	if (ft_isspace(str[pos]))
		return (LEX_SPACE);
	else if (str[pos] == '|')
		return (LEX_PIPE);
	else if (str[pos] == '<')
		return (LEX_LESS);
	else if (str[pos] == '>')
		return (LEX_GREATER);
	else if (str[pos] == '&' && str[pos + 1] == '&')
		return (LEX_AND);
	else if (pos > 0 && str[pos] == '&' && str[pos - 1] == '&')
		return (LEX_AND);
	return (LEX_WORD);
}

bool	token_is_full(char *line, size_t start, size_t end)
{
	size_t	len;

	len = end - start;
	if (len == 2 && ft_strncmp(line + start, "<<", 2) == 0)
		return (true);
	else if (len == 2 && ft_strncmp(line + start, ">>", 2) == 0)
		return (true);
	else if (len == 2 && ft_strncmp(line + start, "||", 2) == 0)
		return (true);
	else if (len == 2 && ft_strncmp(line + start, "&&", 2) == 0)
		return (true);
	return (false);
}

char	*read_token(char *line, size_t start, size_t *i, t_char_type type)
{
	bool				in_single_quotes;
	bool				in_double_quotes;

	in_single_quotes = false;
	in_double_quotes = false;
	while (line[*i])
	{
		if (!in_single_quotes && !in_double_quotes \
		&& (type != get_char_type(line, *i) \
		|| token_is_full(line, start, *i)))
			break ;
		if (line[*i] == '\'' && !in_double_quotes)
			in_single_quotes = !in_single_quotes;
		if (line[*i] == '\"' && !in_single_quotes)
			in_double_quotes = !in_double_quotes;
		(*i)++;
	}
	return (ft_substr(line, start, *i - start));
}

char	*get_next_token_content(char *line, t_minishell *ms)
{
	static size_t		i;
	size_t				start;
	t_char_type			type;
	char				*content;

	while (ft_isspace(line[i]))
		i++;
	if (!line[i])
	{
		i = 0;
		return (NULL);
	}
	start = i;
	type = get_char_type(line, i);
	content = read_token(line, start, &i, type);
	if (!content)
		ms_error("lexer", NULL, 1, ms);
	return (content);
}

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

// void	join_unclosed_tokens(t_minishell *ms)
// {
// 	t_token *token;

// 	while (token)
// 	{
// 		if (token->remove && token->next && is_unclosed(token->content))
// 		{
// 			token_join(token, token->next)
// 		}
// 		token = token->next;
// 	}
// }

char	*get_joined_input_content(t_input *head, t_input *input, t_minishell *ms)
{
	char	*content;
	size_t	len;

	len = 0;
	while (input)
	{
		len += ft_strlen(input->content) + 1;
		input = input->next;
	}
	content = (char *)ft_calloc(len + 1, sizeof(char));
	if (!content)
		ms_error("get_joined_input_content", NULL, 1, ms);
	if (!content)
		return (NULL);
	input = head;
	while (input)
	{
		if (input != head && is_unclosed(content))
			ft_strlcat(content, "\n", len + 1);
		else if (input != head)
			ft_strlcat(content, " ", len + 1);
		ft_strlcat(content, input->content, len + 1);
		input = input->next;
	}
	return (content);
}

void	init_tokens(t_minishell *ms)
{
	t_token	*token;
	char	*content;

	free(ms->line);
	ms->line = get_joined_input_content(ms->head_input, ms->head_input, ms);
	while (ms->line)
	{
		content = get_next_token_content(ms->line, ms);
		if (!content)
			break ;
		token = token_new(content);
		if (!token || !mark_for_removal(token))
		{
			free(content);
			ms_error("lexer", NULL, 1, ms);
			break ;
		}
		token_add_back(&ms->head_token, token);
		set_token_type(token);
		token->operator = get_operator_type(token);
	}
	if (ms->debug)
		debug_print_tokens(&ms->head_token, 1);
}

// void	init_tokens(char *line, t_minishell *ms)
// {
// 	t_token	*token;
// 	char	*content;

// 	while (1)
// 	{
// 		content = get_next_token_content(line, ms);
// 		if (!content)
// 			break ;
// 		token = token_new(content);
// 		if (!token || !mark_for_removal(token))
// 		{
// 			free(content);
// 			ms_error("lexer", NULL, 1, ms);
// 			break ;
// 		}
// 		token_add_back(&ms->head_token, token);
// 		set_token_type(token);
// 		token->operator = get_operator_type(token);
// 	}
// 	// join_unclosed_tokens(ms);
// 	if (ms->debug)
// 		debug_print_tokens(&ms->head_token, 1);
// }
