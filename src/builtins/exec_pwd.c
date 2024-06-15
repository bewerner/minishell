/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pwd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 00:29:43 by sgeiger           #+#    #+#             */
/*   Updated: 2024/06/15 21:38:02 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	exec_pwd(t_minishell *ms)
{
	ft_putendl_fd(ms->cwd, STDOUT_FILENO);
}
