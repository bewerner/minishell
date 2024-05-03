/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/03 19:31:50 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/04 00:11:07 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
