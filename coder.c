/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpaim-so <jpaim-so@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 21:50:02 by jpaim-so          #+#    #+#             */
/*   Updated: 2026/06/22 22:05:13 by jpaim-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	is_stopped(t_coder *coder)
{
	int	stopped;

	pthread_mutex_lock(&coder->sim->state_lock);
	stopped = coder->sim->stop;
	pthread_mutex_unlock(&coder->sim->state_lock);
	return (stopped);
}

static void	compile(t_coder *coder)
{
	if (acquire_dongles(coder))
		return ;
	pthread_mutex_lock(&coder->sim->state_lock);
	coder->last_compile_start = get_time_ms();
	coder->times_compiled += 1;
	pthread_mutex_unlock(&coder->sim->state_lock);
	log_state(coder, "is compiling");
	usleep(coder->sim->time_to_compile * 1000);
	release_dongles(coder);
}

static void	debug(t_coder *coder)
{
	log_state(coder, "is debugging");
	usleep(coder->sim->time_to_debug * 1000);
}

static void	refactor(t_coder *coder)
{
	log_state(coder, "is refactoring");
	usleep(coder->sim->time_to_refactor * 1000);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (!is_stopped(coder))
	{
		compile(coder);
		if (is_stopped(coder) == 0)
			debug(coder);
		if (is_stopped(coder) == 0)
			refactor(coder);
	}
	return (NULL);
}
