/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/05 19:50:28 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/23 00:40:35 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ms_error(char *s1, char *s2, uint8_t exit_code, t_minishell *ms)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	if (s1 && s2)
	{
		ft_putstr_fd(s1, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putstr_fd(s2, STDERR_FILENO);
		ft_putchar_fd('\n', STDERR_FILENO);
	}
	else if (errno)
	{
		perror(s1);
		// if (errno == EACCES)
		// 	exit_code = 126;
	}
	else
	{
		ft_putstr_fd(s1, STDERR_FILENO);
		ft_putchar_fd('\n', STDERR_FILENO);
	}
	errno = 0;
	ms->error = true;
	ms->exit_code = exit_code;
}
