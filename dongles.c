/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpaim-so <jpaim-so@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 19:42:37 by jpaim-so          #+#    #+#             */
/*   Updated: 2026/06/22 02:09:35 by jpaim-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static long long	prio_calculation(t_coder *coder)
{
	if (coder->sim->scheduler == 0)
		return (coder->request_time);
	return (coder->last_compile_start + coder->sim->time_to_burnout);
}

static int	take_dongles(t_coder *coder)
{
	heap_remove(&coder->sim->wait_queue, coder);
	coder->left->status = 1;
	coder->right->status = 1;
	return (1);
}

int	dongles_available(t_coder *coder)
{
	long long	time;

	time = get_time_ms();
	if (coder->left == coder->right)
		return (0);
	if (coder->left->status != 0 || time < coder->left->release_time
		+ coder->sim->dongle_cooldown)
		return (0);
	if (coder->right->status != 0 || time < coder->right->release_time
		+ coder->sim->dongle_cooldown)
		return (0);
	return (1);
}

int	acquire_dongles(t_coder *coder)
{
	pthread_mutex_lock(&coder->sim->state_lock);
	coder->request_time = get_time_ms();
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
	if (!take_dongles(coder))
	{
		pthread_mutex_unlock(&coder->sim->state_lock);
		return (1);
	}
	pthread_mutex_unlock(&coder->sim->state_lock);
	log_state(coder, "has taken a dongle");
	log_state(coder, "has taken a dongle");
	return (0);
}

void	release_dongles(t_coder *coder)
{
	long long	now;

	pthread_mutex_lock(&coder->sim->state_lock);
	now = get_time_ms();
	coder->left->status = 0;
	coder->left->release_time = now;
	coder->right->status = 0;
	coder->right->release_time = now;
	pthread_mutex_unlock(&coder->sim->state_lock);
}
