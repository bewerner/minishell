/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   connect_rest.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 02:49:56 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/21 02:51:02 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	build_left_branch(t_leaf *head, t_leaf *branch)
{
	t_leaf	*leaf;

	if (!branch->parent || branch->parent->right != branch)
		leaf = head;
	else
		leaf = branch->parent->next;
	while (leaf && leaf->operator < OP_PIPE)
	{
		connect_to_left_branch(branch, leaf);
		leaf = leaf->next;
	}
}

void	build_right_branch(t_leaf *branch)
{
	t_leaf	*leaf;

	leaf = branch->next;
	if (leaf && leaf->operator < OP_PIPE)
	{
		connect_to_right_branch(branch, leaf);
		branch = leaf;
		leaf = leaf->next;
	}
	while (leaf && leaf->operator < OP_PIPE)
	{
		connect_to_left_branch(branch, leaf);
		leaf = leaf->next;
	}
}

void	connect_rest(t_leaf *head, t_leaf *branch, t_leaf *root)
{
	t_leaf	*leaf;

	leaf = head->next;
	if (root->operator < OP_PIPE)
	{
		while (leaf)
		{
			connect_to_left_branch(branch, leaf);
			leaf = leaf->next;
		}
		return ;
	}
	while (branch)
	{
		if (branch->operator >= OP_PIPE)
		{
			if (!branch->left)
				build_left_branch(head, branch);
			if (!branch->right)
				build_right_branch(branch);
		}
		branch = branch->next;
	}
}
