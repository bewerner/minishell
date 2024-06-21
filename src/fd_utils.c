/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fd_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/20 23:38:48 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/21 16:56:19 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	restore_std_fd(t_minishell *ms)
{
	dup2(ms->fd_stdin_dup, STDIN_FILENO);
	dup2(ms->fd_stdout_dup, STDOUT_FILENO);
}

void	close_fd_parent_child(bool parent, bool child, t_minishell *ms)
{
	if (parent)
	{
		if (ms->close_in_parent[0] != -1)
			close(ms->close_in_parent[0]);
		if (ms->close_in_parent[1] != -1
			&& ms->close_in_parent[1] != ms->close_in_parent[0])
		{
			close(ms->close_in_parent[1]);
		}
		if (ms->close_in_parent[2] != -1
			&& ms->close_in_parent[2] != ms->close_in_parent[0]
			&& ms->close_in_parent[2] != ms->close_in_parent[1])
		{
			close(ms->close_in_parent[2]);
		}
		ms->close_in_parent[0] = -1;
		ms->close_in_parent[1] = -1;
		ms->close_in_parent[2] = -1;
	}
	if (child)
	{
		if (ms->close_in_child != -1)
			close(ms->close_in_child);
		ms->close_in_child = -1;
	}
}
