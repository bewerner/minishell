/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/03 19:31:50 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/05 01:54:55 by bwerner          ###   ########.fr       */
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
		// ms.line = ft_strdup("asdf asdf asdf asdf asdf && || >><<>><>>>> asdjhafkljsfh");
		if (!ms.line)
			continue ;
		init_tokens(ms.line, &ms);
		cleanup(&ms);
		exit(0);
	}
}
