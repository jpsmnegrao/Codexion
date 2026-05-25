/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpaim-so <jpaim-so@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 17:49:57 by jpaim-so          #+#    #+#             */
/*   Updated: 2026/05/25 12:58:54 by jpaim-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char *argv[])
{
	t_sim	sim;

	if (parse_input(&sim, argc, argv) != 0)
		return (1);
	if (setup_simulation(&sim) != 0)
		return (1);
	printf("coders=%lld burnout=%lld compile=%lld debug=%lld refactor=%lld "
		"required=%lld cooldown=%lld scheduler=%d\n",
		sim.number_of_coders, sim.time_to_burnout, sim.time_to_compile,
		sim.time_to_debug, sim.time_to_refactor,
		sim.number_of_compiles_required, sim.dongle_cooldown, sim.scheduler);
	destroy_simulation(&sim, sim.number_of_coders);
	return (0);
}
