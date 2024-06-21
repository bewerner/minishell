/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   update_line.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 02:35:14 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/21 02:36:54 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	free_unclosed_token(t_minishell *ms)
{
	t_token	*last;
	t_token	*prev;

	if (ms->error || !ms->head_token)
		return ;
	last = token_last(ms->head_token);
	prev = get_previous_token(&ms->head_token, last);
	if (last && is_unclosed(last->content))
	{
		if (!prev)
			free_tokens(&ms->head_token);
		else
		{
			prev->next = NULL;
			free(last->content);
			free(last->remove);
			free(last->original_content);
			free(last);
		}
	}
}

char	*get_input_content(t_input *head, t_input *input, t_minishell *ms)
{
	char	*content;
	size_t	len;

	if (ms->error)
		return (NULL);
	len = 0;
	while (input)
	{
		len += ft_strlen(input->content) + 1;
		input = input->next;
	}
	content = (char *)ft_calloc(len + 1, sizeof(char));
	if (!content)
		ms_error("get_input_content: ft_calloc", NULL, EXIT_FAILURE, ms);
	input = head;
	while (input && content)
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

char	*skip_tokenized_content(char *content, t_token *token, t_minishell *ms)
{
	char	*rt;

	if (!content || ms->error)
		return (NULL);
	rt = content;
	while (token)
	{
		while (ft_isspace(*rt))
			rt++;
		if (token->original_length)
			rt += token->original_length;
		else
			rt += ft_strlen(token->content);
		token = token->next;
	}
	return (rt);
}
