/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_env.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 00:06:53 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/05 00:33:29 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	exec_env(t_minishell *ms)
{
	size_t	i;

	i = 0;
	while (ms->envp[i])
	{
		printf("%s\n", ms->envp[i]);
		i++;
	}
	ms->exit_code = 0;
}
