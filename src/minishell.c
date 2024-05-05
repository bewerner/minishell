/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/03 19:31:50 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/05 19:53:28 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	run_line(char *line, t_minishell *ms)
{
	init_tokens(line, ms);
	cleanup(ms);
}

void	read_arguments(int argc, char **argv, t_minishell *ms)
{
	if (ft_strncmp(argv[1], "-c\0", 3))
	{
		ms_error(argv[1], ": invalid option\nonly -c is supported", 1, ms);
		exit (ms->exit_code);
	}
	if (argc == 2)
	{
		ms_error("-c: option requires an argument", NULL, 2, ms);
		exit (ms->exit_code);
	}
	ms->line = ft_strdup(argv[2]);
	run_line(ms->line, ms);
}

int	main(int argc, char **argv)
{
	t_minishell	ms;

	ft_bzero(&ms, sizeof(ms));
	if (argc != 1)
		read_arguments(argc, argv, &ms);
	while (argc == 1)
	{
		ms.error = false;
		ms.line = readline("minishell: ");
		if (!ms.line)
			continue ;
		run_line(ms.line, &ms);
	}
	return (ms.exit_code);
}
