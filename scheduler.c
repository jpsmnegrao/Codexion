/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpaim-so <jpaim-so@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/25 13:51:28 by jpaim-so          #+#    #+#             */
/*   Updated: 2026/05/28 21:40:43 by jpaim-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	next_in_line_fifo(t_sim *sim)
{
	int			i;
	long long	temp;
	int			next_coder;

	i = 0;
	temp = LLONG_MAX;
	next_coder = 0;
	while (i < sim->number_of_coders)
	{
		if (sim->coders[i].is_waiting == 1)
		{
			if (sim->coders[i].request_time < temp
				|| (sim->coders[i].request_time == temp
					&& sim->coders[i].id < next_coder))
			{
				temp = sim->coders[i].request_time;
				next_coder = sim->coders[i].id;
			}
		}
		i++;
	}
	return (next_coder);
}

int	next_in_line_edf(t_sim *sim)
{
	int			i;
	long long	temp;
	int			next_coder;
	long long	burnout_deadline;

	i = 0;
	temp = LLONG_MAX;
	next_coder = 0;
	while (i < sim->number_of_coders)
	{
		if (sim->coders[i].is_waiting == 1)
		{
			burnout_deadline = sim->coders[i].last_compile_start
				+ sim->time_to_burnout;
			if (burnout_deadline < temp
				|| (burnout_deadline == temp
					&& sim->coders[i].id < next_coder))
			{
				temp = burnout_deadline;
				next_coder = sim->coders[i].id;
			}
		}
		i++;
	}
	return (next_coder);
}

int	get_next_coder(t_sim *sim)
{
	if (sim->scheduler == 0)
		return (next_in_line_fifo(sim));
	return (next_in_line_edf(sim));
}

int	can_proceed(t_coder *coder)
{
	if (dongles_available(coder) && get_next_coder(coder->sim) == coder->id)
		return (1);
	return (0);
}
