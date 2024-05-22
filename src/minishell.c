/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/03 19:31:50 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/22 23:23:39 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	g_signal = 0;

void	run_line(char *line, t_minishell *ms)
{
	init_tokens(line, ms);
	if (!ms->head_token)
		return ;
	rearrange_tokens(ms);
	init_leafs(ms);
	init_tree(ms);
	exec_tree(ms->first_leaf, ms);
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

// void	sigint_handler(int signum)
// {
// 	(void)signum;
// 	ioctl(0, TIOCSTI, "\4");
// }

int	main(int argc, char **argv, char **envp)
{
	t_minishell	ms;

	ft_bzero(&ms, sizeof(ms));
	ms.fd_stdin_dup = dup(STDIN_FILENO);
	ms.fd_stdout_dup = dup(STDOUT_FILENO);
	init_signals(&ms);
	ms.envp = envp; // TEMP
	init_env(envp, &ms);
	ms.debug = 0;
	if (argc != 1)
		read_arguments(argc, argv, &ms);
	while (argc == 1)
	{
		ms.error = false;
		ms.line = get_input(&ms);
		if (g_signal)
		{
			if (!ms.exit_code)
				ms.exit_code = 1;
			g_signal = 0;
		}
		if (!ms.line)
			continue ;
		add_history(ms.line);
		run_line(ms.line, &ms);
		cleanup(&ms);
	}
	rl_clear_history();
	return (ms.exit_code);
}
