/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpaim-so <jpaim-so@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 15:48:57 by jpaim-so          #+#    #+#             */
/*   Updated: 2026/07/01 20:08:07 by jpaim-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* Compute the priority used to order a coder in the waiting queue.
   FIFO uses the request time, while EDF uses the burnout deadline. */
long long	prio_calculation(t_coder *coder)
{
	if (coder->sim->scheduler == 0)
		return (coder->request_time);
	return (coder->last_compile_start + coder->sim->time_to_burnout);
}

/* Checks whether a coder's dongles are physically free (not held by
   another coder). */
static int	is_contender_ready(t_coder *contender)
{
	if (contender->left == contender->right)
		return (0);
	if (contender->left->status != 0)
		return (0);
	if (contender->right->status != 0)
		return (0);
	return (1);
}

/* Check whether another coder waiting for the same dongle has higher priority
   and is ready to take it. Returns 1 if this coder must yield, 0 otherwise.
   Called with state_lock held. */
static int	loses_to_contender(t_coder *coder, t_dongle *dongle)
{
	t_heap_container	*q;
	t_heap_entry		*self;
	int					i;

	q = &coder->sim->wait_queue;
	self = NULL;
	i = -1;
	while (++i < q->size)
		if (q->heap[i].coder == coder)
			self = &q->heap[i];
	if (!self)
		return (0);
	i = -1;
	while (++i < q->size)
	{
		if (q->heap[i].coder != coder
			&& (q->heap[i].coder->left == dongle
				|| q->heap[i].coder->right == dongle)
			&& has_priority(q->heap[i], *self)
			&& is_contender_ready(q->heap[i].coder))
			return (1);
	}
	return (0);
}

/* Checks priority contention only — returns 1 if no higher-priority contender
   is waiting for either of this coder's dongles, 0 otherwise. */
int	can_proceed(t_coder *coder)
{
	if (loses_to_contender(coder, coder->left))
		return (0);
	if (loses_to_contender(coder, coder->right))
		return (0);
	return (1);
}

/* Attempt to secure both dongles for this coder. If the coder has priority and
   dongles are available, takes them, removes the coder from the wait queue,
   and returns 1. Returns 0 if the coder must keep waiting. Called with 
   state_lock held. */
int	try_secure_resources(t_coder *coder)
{
	if (can_proceed(coder) && dongles_available(coder))
	{
		take_dongles(coder);
		heap_remove(&coder->sim->wait_queue, coder);
		coder->in_queue = 0;
		return (1);
	}
	return (0);
}
