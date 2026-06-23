/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpaim-so <jpaim-so@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/24 22:09:37 by jpaim-so          #+#    #+#             */
/*   Updated: 2026/06/23 17:12:58 by jpaim-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* Return the current time in milliseconds. */
long long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((long long)tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

/* Release all resources owned by the simulation. */
void	destroy_simulation(t_sim *sim)
{
	int	i;

	i = 0;
	pthread_mutex_destroy(&sim->output_lock);
	pthread_mutex_destroy(&sim->state_lock);
	while (i < sim->number_of_coders)
	{
		pthread_mutex_destroy(&sim->dongles[i].lock);
		i++;
	}
	heap_free(&sim->wait_queue);
	free(sim->dongles);
	free(sim->coders);
}

/* Initialize the mutexes used for output serialization and shared state. */
static int	init_mutexes(t_sim *sim)
{
	if (pthread_mutex_init(&sim->output_lock, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&sim->state_lock, NULL) != 0)
		return (pthread_mutex_destroy(&sim->output_lock),
			pthread_mutex_destroy(&sim->state_lock),
			free(sim->dongles), free(sim->coders), 1);
	return (0);
}

/* Initialize each coder and its associated left and right dongles. */
static void	init_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].times_compiled = 0;
		sim->coders[i].sim = sim;
		sim->coders[i].left = &sim->dongles[i];
		sim->coders[i].right = &sim->dongles[(i + 1) % sim->number_of_coders];
		sim->coders[i].last_compile_start = sim->sim_start_time;
		sim->coders[i].request_time = 0;
		sim->dongles[i].status = 0;
		sim->dongles[i].release_time = 0;
		sim->coders[i].request_initialized = 0;
		i++;
	}
	i = 0;
	while (i < sim->number_of_coders)
	{
		pthread_mutex_init(&sim->dongles[i].lock, NULL);
		i++;
	}
}

/* Allocate and initialize the simulation state, mutexes, heap, and coder
   links. */
int	setup_simulation(t_sim *sim)
{
	sim->dongles = malloc(sizeof(t_dongle) * sim->number_of_coders);
	if (!sim->dongles)
		return (1);
	sim->coders = malloc(sizeof(t_coder) * sim->number_of_coders);
	if (!sim->coders)
		return (free(sim->dongles), 1);
	if (!heap_init(&sim->wait_queue, sim->number_of_coders, sim->scheduler))
		return (free(sim->dongles), free(sim->coders), 1);
	sim->stop = 0;
	if (init_mutexes(sim) != 0)
	{
		heap_free(&sim->wait_queue);
		free(sim->dongles);
		free(sim->coders);
		return (1);
	}
	sim->sim_start_time = get_time_ms();
	sim->first_request_time = sim->sim_start_time;
	init_coders(sim);
	return (0);
}
