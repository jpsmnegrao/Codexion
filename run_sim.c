/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_sim.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpaim-so <jpaim-so@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/25 13:51:24 by jpaim-so          #+#    #+#             */
/*   Updated: 2026/05/27 21:55:22 by jpaim-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	run_sim(t_sim *sim)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
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
	while (j < i)
	{
		if (pthread_join(sim->coders[j].thread, NULL) != 0)
			return (1);
		j++;
	}
	if (sim->stop == 1)
		return (1);
	return (0);
}
