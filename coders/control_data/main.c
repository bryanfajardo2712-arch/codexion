/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bryfajar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 15:38:59 by bryfajar          #+#    #+#             */
/*   Updated: 2026/10/27 15:39:08 by bryfajar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../codexion.h"

int	main(int argc, char **argv)
{
	t_sim	sim;

	memset(&sim, 0, sizeof(sim));
	if (argc == 9)
	{
		if (parse(&sim.config, argv) != 0)
			return (1);
		if (data_init(&sim) != 0)
			return (1);
		simulation_start(&sim);
		clean_simulation(&sim);
	}
	else
	{
		fprintf(stderr, "Error, wrong numbers of arguments");
		return (1);
	}
	return (0);
}
