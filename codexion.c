/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpaim-so <jpaim-so@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 17:49:57 by jpaim-so          #+#    #+#             */
/*   Updated: 2026/06/19 17:29:34 by jpaim-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	run_sim(t_sim *sim)
{
	int			i;
	int			create_failed;
	pthread_t	monitor;

	i = 0;
	create_failed = 0;
	if (pthread_create(&monitor, NULL, monitor_routine, sim) != 0)
		return (1);
	while (i < sim->number_of_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL, coder_routine,
				&sim->coders[i]) != 0)
		{
			sim->stop = 1;
			create_failed = 1;
			break ;
		}
		i++;
	}
	while (i-- > 0)
		pthread_join(sim->coders[i].thread, NULL);
	pthread_join(monitor, NULL);
	return (create_failed);
}

int	main(int argc, char *argv[])
{
	t_sim	sim;

	if (parse_input(&sim, argc, argv) != 0)
		return (1);
	if (setup_simulation(&sim) != 0)
		return (1);
	if (run_sim(&sim) != 0)
		return (destroy_simulation(&sim), 1);
	destroy_simulation(&sim);
	return (0);
}
