/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pwd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sgeiger <sgeiger@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 00:29:43 by sgeiger           #+#    #+#             */
/*   Updated: 2024/06/06 01:30:46 by sgeiger          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	exec_pwd(t_minishell *ms)
{
	char	cwd[PATH_MAX];

	if (getcwd(cwd, sizeof(cwd)) == NULL)
		ms_error("exec_pwd", NULL, 1, ms);
	else
		printf("%s\n", cwd);
	ms->exit_code = 0;
}
