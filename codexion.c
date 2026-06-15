/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpaim-so <jpaim-so@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 17:49:57 by jpaim-so          #+#    #+#             */
/*   Updated: 2026/06/15 23:09:24 by jpaim-so         ###   ########.fr       */
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
	if (run_sim(&sim) != 0)
		return (destroy_simulation(&sim), 1);
	destroy_simulation(&sim);
	return (0);
}
