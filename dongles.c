/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpaim-so <jpaim-so@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 19:42:37 by jpaim-so          #+#    #+#             */
/*   Updated: 2026/06/23 17:37:41 by jpaim-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* Compute the priority used to order a coder in the waiting queue.
   FIFO uses the request time, while EDF uses the burnout deadline. */
static long long	prio_calculation(t_coder *coder)
{
	if (coder->sim->scheduler == 0)
		return (coder->request_time);
	return (coder->last_compile_start + coder->sim->time_to_burnout);
}

/* Mark both dongles as taken and remove the coder from the waiting queue. */
static int	take_dongles(t_coder *coder)
{
	pthread_mutex_lock(&coder->left->lock);
	pthread_mutex_lock(&coder->right->lock);
	coder->left->status = 1;
	coder->right->status = 1;
	return (1);
}

/* Check whether both target dongles are free and past their cooldown
   period. */
int	dongles_available(t_coder *coder)
{
	long long	time;
	int			available;

	time = get_time_ms();
	if (coder->left == coder->right)
		return (0);
	available = 1;
	pthread_mutex_lock(&coder->left->lock);
	if (coder->left->status != 0 || time < coder->left->release_time
		+ coder->sim->dongle_cooldown)
		available = 0;
	pthread_mutex_unlock(&coder->left->lock);
	pthread_mutex_lock(&coder->right->lock);
	if (coder->right->status != 0 || time < coder->right->release_time
		+ coder->sim->dongle_cooldown)
		available = 0;
	pthread_mutex_unlock(&coder->right->lock);
	return (available);
}

/* Register a dongle request, wait until the coder can proceed, and reserve
   them. */
int	acquire_dongles(t_coder *coder)
{
	pthread_mutex_lock(&coder->sim->state_lock);
	set_request_time(coder);
	heap_insert(&coder->sim->wait_queue, coder, prio_calculation(coder),
		coder->id);
	while (!can_proceed(coder) && !coder->sim->stop)
	{
		pthread_mutex_unlock(&coder->sim->state_lock);
		usleep(500);
		pthread_mutex_lock(&coder->sim->state_lock);
	}
	if (coder->sim->stop)
	{
		heap_remove(&coder->sim->wait_queue, coder);
		pthread_mutex_unlock(&coder->sim->state_lock);
		return (1);
	}
	heap_remove(&coder->sim->wait_queue, coder);
	pthread_mutex_unlock(&coder->sim->state_lock);
	if (!take_dongles(coder))
		return (1);
	log_state(coder, "has taken a dongle");
	log_state(coder, "has taken a dongle");
	return (0);
}

/* Release both dongles and start their cooldown timer. */
void	release_dongles(t_coder *coder)
{
	long long	now;

	now = get_time_ms();
	coder->left->status = 0;
	coder->right->status = 0;
	coder->left->release_time = now;
	coder->right->release_time = now;
	pthread_mutex_unlock(&coder->right->lock);
	pthread_mutex_unlock(&coder->left->lock);
}
