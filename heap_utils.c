/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpaim-so <jpaim-so@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/15 22:32:27 by jpaim-so          #+#    #+#             */
/*   Updated: 2026/06/15 23:46:49 by jpaim-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	has_priority(t_heap_entry a, t_heap_entry b)
{
	if (a.prio < b.prio || (a.prio == b.prio && a.tiebreaker < b.tiebreaker))
		return (1);
	return (0);
}

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

void	heap_sift_up(t_heap_container *heap, int i)
{
	t_heap_entry	temp;
	int				parent;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (heap->heap[i].prio < heap->heap[parent].prio
			|| (heap->heap[i].prio == heap->heap[parent].prio
				&& heap->heap[i].tiebreaker < heap->heap[parent].tiebreaker))
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
