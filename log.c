/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpaim-so <jpaim-so@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 22:20:09 by jpaim-so          #+#    #+#             */
/*   Updated: 2026/05/27 22:33:22 by jpaim-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	log_state(t_coder *coder, const char *state)
{
	long long	current_time;

	current_time = get_time_ms() - coder->sim->sim_start_time;
	pthread_mutex_lock(&coder->sim->output_lock);
	printf("%lld %d %s\n", current_time, coder->id, state);
	pthread_mutex_unlock(&coder->sim->output_lock);
}
