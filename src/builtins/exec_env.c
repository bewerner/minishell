/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_env.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 00:06:53 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/11 19:42:03 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	exec_env(t_minishell *ms)
{
	size_t	i;

	i = 0;
	while (ms->envp[i])
	{
		// printf("%s\n", ms->envp[i]);
		ft_putendl_fd(ms->envp[i], STDOUT_FILENO);
		i++;
	}
	ms->exit_code = 0;
}
