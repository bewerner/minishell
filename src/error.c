/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/05 19:50:28 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/05 19:50:40 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ms_error(char *s1, char *s2, uint8_t exit_code, t_minishell *ms)
{
	ft_putstr_fd("minishell: ", 2);
	if (s1)
		ft_putstr_fd(s1, 2);
	if (s2)
		ft_putstr_fd(s2, 2);
	else if (errno)
		perror(NULL);
	if (!errno)
		ft_putchar_fd('\n', 2);
	errno = 0;
	ms->error = true;
	ms->exit_code = exit_code;
}
