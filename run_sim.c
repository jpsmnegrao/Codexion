/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_sim.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpaim-so <jpaim-so@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/25 13:51:24 by jpaim-so          #+#    #+#             */
/*   Updated: 2026/05/28 23:38:46 by jpaim-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	run_sim(t_sim *sim)
{
	int			i;
	pthread_t	monitor;

	i = 0;
	if (pthread_create(&monitor, NULL, monitor_routine, sim) != 0)
		return (1);
	while (i < sim->number_of_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL, coder_routine,
				&sim->coders[i]) != 0)
		{
			sim->stop = 1;
			break ;
		}
		i++;
	}
	while (i-- > 0)
		pthread_join(sim->coders[i].thread, NULL);
	pthread_join(monitor, NULL);
	if (sim->stop == 1)
		return (1);
	return (0);
}
