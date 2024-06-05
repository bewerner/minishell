/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_exit.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 23:00:20 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/06 01:26:15 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int64_t	ft_atoi64(const char *str)
{
	int64_t	n;
	int		i;
	int64_t	x;

	i = 0;
	n = 0;
	x = 1;
	while (ft_isspace(str[i]))
		i++;
	if (str[i] == '-')
		x = -1;
	if (ft_isdigit(str[i]) || str[i] == '-' || str[i] == '+')
	{
		while (ft_isdigit(str[i + 1]))
			i++;
		while (i >= 0 && str[i] != '-' && str[i] != '+' && !ft_isspace(str[i]))
		{
			n += x * (str[i] - '0');
			x *= 10;
			i--;
		}
	}
	return (n);
}

static bool	is_in_range(char *nbstr, char *min, char *max)
{
	size_t		i;
	size_t		j;
	size_t		len;

	i = 0;
	j = 0;
	if (nbstr[0] == '-')
		max = min + 1;
	if (nbstr[0] == '-' || nbstr[0] == '+')
		i = 1;
	while (nbstr[i] == '0')
		i++;
	len = ft_strlen(nbstr) - i;
	if (len < ft_strlen(max))
		return (true);
	if (len > ft_strlen(max))
		return (false);
	while (nbstr[i])
	{
		if (nbstr[i] > max[j])
			return (false);
		i++;
		j++;
	}
	return (true);
}

static bool	is_numeric(char *str)
{
	int	i;

	i = 0;
	if (str[0] == '-' || str[0] == '+')
		i = 1;
	if (!ft_isdigit(str[i]))
		return (false);
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (false);
		i++;
	}
	return (true);
}

static uint8_t	get_exit_code(char *str, t_minishell *ms)
{
	if (!is_numeric(str)
		|| !is_in_range(str, "-9223372036854775808", "9223372036854775807"))
	{
		ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
		ft_putstr_fd(str, STDERR_FILENO);
		ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
		ms->error = true;
		return (255);
	}
	return ((uint8_t)ft_atoi64(str));
}

void	exec_exit(t_leaf *leaf, t_token *token, t_minishell *ms)
{
	ms->exit_code = 0;
	if (!leaf->fork)
		printf("exit\n");
	if (leaf->size > 1)
		ms->exit_code = get_exit_code(token->content, ms);
	if (!ms->error && leaf->size > 2)
	{
		ft_putstr_fd("minishell: exit: too many arguments\n", STDERR_FILENO);
		ms->exit_code = 1;
	}
	terminate(ms->exit_code, ms);
}
