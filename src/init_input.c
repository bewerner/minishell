/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_input.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 23:53:55 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/15 23:37:32 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_inputs(t_input **head)
{
	t_input	*ptr;
	t_input	*next;

	ptr = *head;
	while (ptr)
	{
		next = ptr->next;
		free(ptr->content);
		free(ptr);
		ptr = next;
	}
	*head = NULL;
}

bool	check_syntax(t_token *token, t_minishell *ms)
{
	// printf("ms error: %d, head complete: %d\n", ms->error, ms->head_input->complete);
	// printf("check_syntax\n");
	if (token && token->operator >= OP_PIPE)
	{
		syntax_error(token, NULL, ms);
		return (false);
	}
	if (ms->head_input && ms->head_input->complete
		&& is_unclosed(token_last(ms->head_token)->content))
	{
		syntax_error(token_last(ms->head_token), NULL, ms);
		return (false);
	}
	while (token)
	{
		if (token->operator)
		{
			if (token->next && token->next->operator >= token->operator)
			{
				syntax_error(token->next, NULL, ms);
				return (false);
			}
			else if (!token->next && ms->head_input->complete)
			{
				syntax_error(NULL, NULL, ms);
				return (false);
			}
			else if (!token->next && token->operator == OP_REDIRECT)
			{
				syntax_error(NULL, "newline", ms);
				return (false);
			}
		}
		token = token->next;
	}
	if (ms->syntax_error)
		return (false);
	return (true);
}

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
	if (!user_input && errno) // malloc error
		ms_error("readline", NULL, EXIT_FAILURE, ms);
	else if (!ms->head_input && !user_input) // ctrl + D (EOT)
	{
		printf("\033[Aminishell$ exit\n");
		terminate(ms->exit_code, ms);
	}
	else if (!user_input) // ctrl + D (EOT)
		ms->head_input->complete = true;
	else if (!ms->head_input && user_input && !user_input[0]) // enter (empty input)
	{
		free(user_input);
		user_input = NULL;
	}
	set_signal(SIGINT, sigint_handler);
	return (user_input);
}

void	free_unclosed_token(t_token **head)
{
	t_token	*last;
	t_token	*prev;

	last = token_last(*head);
	prev = get_previous_token(head, last);
	if (last && is_unclosed(last->content))
	{
		if (!prev)
			free_tokens(head);
		else
		{
			prev->next = NULL;
			free(last->content);
			free(last->remove);
			free(last->original_content);
			free(last);
		}
	}
}

void	init_user_input(t_minishell *ms)
{
	t_input	*input;
	char	*content;

	while (!g_signal && !ms->error && !ms->syntax_error
		&& !input_is_complete(ms->head_input, ms->head_token))
	{
		content = get_user_input(ms);
		if (!content)
		{
			if (ms->head_token)
				check_syntax(ms->head_token, ms);
			continue ;
		}
		input = input_new(content);
		if (!input)
		{
			free(content);
			ms_error("init_user_input", NULL, EXIT_FAILURE, ms);
			return ;
		}
		input_add_back(&ms->head_input, input);
		free_unclosed_token(&ms->head_token);
		init_tokens(ms);
		check_syntax(ms->head_token, ms);
		if (ms->head_token && (ms->syntax_error || !is_unclosed(token_last(ms->head_token)->content)))
			init_heredocs(ms->head_token, ms);
	}
	init_heredocs(ms->head_token, ms);
	check_syntax(ms->head_token, ms);
	if (ms->line)
		add_history(ms->line);
	if (ms->syntax_error)
		ms->error = true;
}

char	*non_interactive_readline(t_minishell *ms)
{
	char	*line;

	line = get_next_line(STDIN_FILENO);
	ms->line_count++;
	if (line && ft_strrchr(line, '\n'))
		*ft_strrchr(line, '\n') = '\0';
	return (line);
}

void	init_noninteractive_input(t_minishell *ms)
{
	t_input	*input;
	char	*content;

	while (!g_signal && !ms->error && !ms->syntax_error
		&& !input_is_complete(ms->head_input, ms->head_token))
	{
		content = non_interactive_readline(ms);
		if (!content)
		{
			if (!ms->head_input)
				terminate(ms->exit_code, ms);
			if (ms->head_token)
				check_syntax(ms->head_token, ms);
			ms->head_input->complete = true;
			continue ;
		}
		input = input_new(content);
		if (!input)
		{
			free(content);
			ms_error("init_noninteractive_input", NULL, EXIT_FAILURE, ms);
			return ;
		}
		input_add_back(&ms->head_input, input);
		free_unclosed_token(&ms->head_token);
		init_tokens(ms);
		check_syntax(ms->head_token, ms);
		if (ms->head_token && (ms->syntax_error || !is_unclosed(token_last(ms->head_token)->content)))
			init_heredocs(ms->head_token, ms);
	}
	init_heredocs(ms->head_token, ms);
	check_syntax(ms->head_token, ms);
	if (ms->line)
		add_history(ms->line);
	if (ms->syntax_error)
	{
		put_syntax_error_line(ms->input_syntax_error, ms);
		terminate(ms->exit_code, ms);
	}
}

void	init_input(t_minishell *ms)
{
	if (ms->interactive)
		init_user_input(ms);
	else
		init_noninteractive_input(ms);
}
