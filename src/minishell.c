/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/03 19:31:50 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/17 01:49:05 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	check_syntax(t_token *token, t_minishell *ms);

int	g_signal = 0;
char **args;

// void	sigint_handler(int signum)
// {
// 	(void)signum;
// 	ioctl(0, TIOCSTI, "\4");
// }

void    tmp(char *arg, t_minishell *ms)
{
    // char    **args;
    size_t  i;

    args = ft_split(arg, ';');
    i = 0;
    while (args[i])
    {
        input_add_back(&ms->head_input, input_new(ft_strdup(args[i])));
        init_tokens(ms);
        check_syntax(ms->head_token, ms);
        check_syntax(ms->head_token, ms);
        if (ms->syntax_error)
        {
            put_syntax_error_line(ms->input_syntax_error, ms);
            terminate(ms->exit_code, ms);
        }
        if (ms->error)
            terminate(ms->exit_code, ms);
        rearrange_tokens(ms);
        init_leafs(ms);
        init_tree(ms);
        exec_tree(ms->first_leaf, ms);
        cleanup(ms);
        i++;
    }
    i = 0;
    // while(args[i])
    // {
    //     free(args[i]);
    //     i++;
    // }
    // free(args);
    terminate(ms->exit_code, ms);
}

void	init_fd(t_minishell *ms)
{
	ms->fd_stdin_dup = dup(STDIN_FILENO);
	ms->fd_stdout_dup = dup(STDOUT_FILENO);
	ms->close_in_child = -1;
	ms->close_in_parent[0] = -1;
	ms->close_in_parent[1] = -1;
}

int	main(int argc, char **argv, char **envp)
{
	t_minishell	ms;

	// (void)argv;
	// if (argc != 1)
	// {
	// ft_putendl_fd("No arguments allowed!", STDERR_FILENO);
	// 	exit(EXIT_FAILURE);
	// }
	ft_bzero(&ms, sizeof(ms));
	init_fd(&ms);
	ms.interactive = isatty(STDIN_FILENO);
	// ms.interactive = false;
	init_signals(&ms);
	init_env(envp, &ms);
	// ms.debug = 0;
	if (argc > 2)
		tmp(argv[2], &ms);
	(void)argv;
	while (1)
	{
		cleanup(&ms);
		init_input(&ms);
		if (!ms.head_input || ms.error)
			continue ;
		rearrange_tokens(&ms);
		init_leafs(&ms);
		init_tree(&ms);
		exec_tree(ms.first_leaf, &ms);
	}
}

		// if (g_signal)
		// {
		// 	if (!ms.exit_code)
		// 		ms.exit_code = EXIT_FAILURE;
		// 	g_signal = 0;
		// }
