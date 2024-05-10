/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/03 19:31:50 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/10 02:01:37 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	run_line(char *line, t_minishell *ms)
{
	init_tokens(line, ms);
	if (!ms->head_token)
		return ;
	rearrange_tokens(ms);
	init_leafs(ms);
	init_tree(ms);
}

// temp for debugging. remember to handle 'if (argc != 1)' in main!
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
	if (!ms->line)
		ms_error("read_arguments", NULL, 1, ms);
	run_line(ms->line, ms);
	cleanup(ms);
}

int	main(int argc, char **argv, char **envp)
{
	t_minishell	ms;

	ft_bzero(&ms, sizeof(ms));
	init_env(envp, &ms);
	ms.debug = 1;
	if (argc != 1)
		read_arguments(argc, argv, &ms);
	while (argc == 1)
	{
		ms.error = false;
		ms.line = get_input(&ms);
		if (!ms.line || !ms.line[0])
			continue ;
		run_line(ms.line, &ms);
		cleanup(&ms);
	}
	return (ms.exit_code);
}
