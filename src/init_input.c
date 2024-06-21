/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_input.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 23:53:55 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/21 17:07:13 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	input_is_complete(t_input *input, t_token *token)
{
	(void)token;
	if (!input)
		return (false);
	if (input->complete)
		return (true);
	if (!token)
		return (true);
	if (token_last(token)->operator >= OP_PIPE)
		return (false);
	if (is_unclosed(token_last(token)->content))
		return (false);
	return (true);
}

char	*get_user_input(t_minishell *ms)
{
	char	*user_input;

	if (!ms->head_input)
		user_input = readline("minishell$ ");
	else
	{
		set_signal(SIGINT, sigint_handler_heredoc);
		user_input = readline("> ");
	}
	if (!user_input && errno)
		ms_error("readline", NULL, EXIT_FAILURE, ms);
	else if (!ms->head_input && !user_input)
	{
		printf("\033[Aminishell$ exit\n");
		terminate(ms->exit_code, ms);
	}
	else if (!user_input)
		ms->head_input->complete = true;
	else if (!ms->head_input && user_input && !user_input[0])
	{
		free(user_input);
		user_input = NULL;
	}
	set_signal(SIGINT, sigint_handler);
	return (user_input);
}

void	store_input(char *content, t_minishell *ms)
{
	t_input	*input;

	input = input_new(content);
	if (!input)
	{
		free(content);
		ms_error("store_input: input_new", NULL, EXIT_FAILURE, ms);
		return ;
	}
	input_add_back(&ms->head_input, input);
}

char	*non_interactive_readline(t_minishell *ms)
{
	char	*line;

	line = get_next_line(STDIN_FILENO);
	ms->line_count++;
	if (!line && errno)
		ms_error("readline", NULL, EXIT_FAILURE, ms);
	if (line && ft_strrchr(line, '\n'))
		*ft_strrchr(line, '\n') = '\0';
	if (ms->error || (!line && !ms->head_input))
		terminate(ms->exit_code, ms);
	if (!line && ms->head_input)
		ms->head_input->complete = true;
	if (line && line[0] == '\0' && !ms->head_input)
	{
		free(line);
		line = NULL;
	}
	return (line);
}

void	init_input(t_minishell *ms)
{
	char	*content;

	while (!g_signal && !ms->error && !ms->syntax_error
		&& !input_is_complete(ms->head_input, ms->head_token))
	{
		if (ms->interactive)
			content = get_user_input(ms);
		else
			content = non_interactive_readline(ms);
		if (!content)
			continue ;
		store_input(content, ms);
		init_tokens(ms);
		check_syntax(ms->head_token, ms);
		init_heredocs(ms->head_token, ms);
	}
	check_syntax(ms->head_token, ms);
	init_heredocs(ms->head_token, ms);
	if (ms->syntax_error == SYN_UNCLOSED)
		syntax_error(NULL, ms);
	if (ms->interactive && ms->line)
		add_history(ms->line);
	else if (!ms->interactive && ms->syntax_error && ms->syntax_error < SYN_EOF)
		put_syntax_error_line(ms->syntax_error_input, ms);
}
