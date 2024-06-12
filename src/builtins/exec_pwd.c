/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pwd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 00:29:43 by sgeiger           #+#    #+#             */
/*   Updated: 2024/06/12 14:34:12 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	exec_pwd(t_minishell *ms)
{
	char	cwd[PATH_MAX];

	if (getcwd(cwd, sizeof(cwd)) == NULL)
		ms_error("exec_pwd", NULL, EXIT_FAILURE, ms);
	else
		ft_putendl_fd(cwd, STDOUT_FILENO);
	ms->exit_code = EXIT_SUCCESS;
}
