/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpaim-so <jpaim-so@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/15 22:32:27 by jpaim-so          #+#    #+#             */
/*   Updated: 2026/06/23 17:27:18 by jpaim-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* Compare two heap entries and decide which one has higher priority.
   Lower priority value wins; ties are broken by the tiebreaker field. */
int	has_priority(t_heap_entry a, t_heap_entry b)
{
	if (a.prio < b.prio)
		return (1);
	if (a.prio > b.prio)
		return (0);
	return (a.tiebreaker < b.tiebreaker);
}

/* Return the index of the smallest child in the min-heap, or i if none is 
   smaller. */
int	smallest_child(t_heap_container *heap, int i)
{
	int	left_child;
	int	right_child;
	int	smallest;

	left_child = i * 2 + 1;
	right_child = i * 2 + 2;
	smallest = i;
	if (left_child < heap->size
		&& has_priority(heap->heap[left_child], heap->heap[smallest]))
		smallest = left_child;
	if (right_child < heap->size
		&& has_priority(heap->heap[right_child], heap->heap[smallest]))
		smallest = right_child;
	return (smallest);
}

/* Move the entry at index i upward until the heap ordering is restored. */
void	heap_sift_up(t_heap_container *heap, int i)
{
	t_heap_entry	temp;
	int				parent;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (has_priority(heap->heap[i], heap->heap[parent]))
		{
			temp = heap->heap[i];
			heap->heap[i] = heap->heap[parent];
			heap->heap[parent] = temp;
			i = parent;
		}
		else
			break ;
	}
}

/* Check whether another coder waiting for the same dongle has higher 
   priority. */
static int	loses_to_contender(t_coder *coder, t_dongle *dongle)
{
	t_heap_container	*q;
	t_heap_entry		*self;
	int					i;

	q = &coder->sim->wait_queue;
	self = NULL;
	i = 0;
	while (i < q->size)
	{
		if (q->heap[i].coder == coder)
			self = &q->heap[i];
		i++;
	}
	if (!self)
		return (0);
	i = 0;
	while (i < q->size)
	{
		if (q->heap[i].coder != coder && (q->heap[i].coder->left == dongle
				|| q->heap[i].coder->right == dongle) && has_priority(
				q->heap[i], *self) && dongles_available(q->heap[i].coder))
			return (1);
		i++;
	}
	return (0);
}

/* Decide whether the coder may proceed and take dongles now. */
int	can_proceed(t_coder *coder)
{
	if (!dongles_available(coder))
		return (0);
	if (loses_to_contender(coder, coder->left))
		return (0);
	if (loses_to_contender(coder, coder->right))
		return (0);
	return (1);
}
