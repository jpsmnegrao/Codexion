/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpaim-so <jpaim-so@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 19:42:37 by jpaim-so          #+#    #+#             */
/*   Updated: 2026/05/28 20:21:02 by jpaim-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	dongles_available(t_coder *coder)
{
	long long	time;

	time = get_time_ms();
	if (coder->left->status != 0 || time < coder->left->release_time + 
		coder->sim->dongle_cooldown)
		return (0);
	if (coder->right->status != 0 || time < coder->right->release_time + 
		coder->sim->dongle_cooldown)
		return (0);
	return (1);
}

void	acquire_dongles(t_coder *coder)
{
	pthread_mutex_lock(&coder->sim->state_lock);
	while (!dongles_available(coder))
		pthread_cond_wait(&coder->sim->condition, &coder->sim->state_lock);
	coder->left->status = 1;
	coder->right->status = 1;
	pthread_mutex_unlock(&coder->sim->state_lock);
	log_state(coder, "has taken a dongle");
	log_state(coder, "has taken a dongle");
	
}

void release_dongles(t_coder *coder)
{
	pthread_mutex_lock(&coder->sim->state_lock);
	coder->left->status = 0;
	coder->left->release_time = get_time_ms();
	coder->right->status = 0;
	coder->right->release_time = get_time_ms();
	pthread_cond_broadcast(&coder->sim->condition);
	pthread_mutex_unlock(&coder->sim->state_lock);
}
