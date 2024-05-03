/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/03 19:31:50 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/03 23:23:12 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// void	get_next_token_content(char *line, t_minishell *ms)
// {
// 	size_t	i;

// 	i = 0;
// 	while (line[i])
// 	{

// 	}
// }

void	init_token(char *line, t_minishell *ms)
{
	t_token	*token;

	ms->head = token;
	while
	{
		// get_next_token_content(line, ms);
		token = token_new();
	}
}

int	main(void)
{
	t_minishell	ms;

	ft_bzero(&ms, sizeof(ms));
	while (1)
	{
		ms.line = readline("minishell: ");
		if (!ms.line)
			continue ;
		init_tokens(ms.line, &ms);
		free(ms.line);
	}
	// rl_redisplay();
}
