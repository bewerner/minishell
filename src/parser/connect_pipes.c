/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   connect_pipes.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 02:50:07 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/21 16:52:58 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	connect_to_right_branch(t_leaf *branch, t_leaf *leaf)
{
	while (branch->right)
		branch = branch->right;
	branch->right = leaf;
	leaf->parent = branch;
}

void	connect_to_left_branch(t_leaf *branch, t_leaf *leaf)
{
	while (branch->left)
		branch = branch->left;
	branch->left = leaf;
	leaf->parent = branch;
}

bool	leaf_is_behind_branch(t_leaf *leaf, t_leaf *branch)
{
	leaf = leaf->prev;
	while (leaf)
	{
		if (leaf == branch)
			return (true);
		leaf = leaf->prev;
	}
	return (false);
}

void	connect_pipes(t_leaf *branch, t_leaf *leaf, t_leaf *head)
{
	while (branch->left)
		branch = branch->left;
	while (branch)
	{
		leaf = branch->next;
		if (!branch->left)
			leaf = head;
		while (leaf && leaf->operator != OP_LOGICAL)
		{
			if (leaf->operator == OP_PIPE && leaf != branch)
			{
				if (leaf_is_behind_branch(leaf, branch))
					connect_to_right_branch(branch, leaf);
				else
				{
					connect_to_left_branch(branch, leaf);
					branch = leaf;
				}
			}
			leaf = leaf->next;
			if (leaf == branch)
				leaf = leaf->next;
		}
		branch = branch->parent;
	}
}
