/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bryfajar                                   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 00:00:00 by bryfajar          #+#    #+#             */
/*   Updated: 2026/06/06 00:00:00 by bryfajar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

void	clean_simulation(t_sim *sim)
{
	long	i;

	if (!sim)
		return ;
	i = 0;
	while (i < sim->config.number_of_coders)
	{
		pthread_mutex_destroy(&sim->dongles[i].lock);
		pthread_mutex_destroy(&sim->coder[i].data_lock);
		i++;
	}
	pthread_mutex_destroy(&sim->shared.log_lock);
	pthread_mutex_destroy(&sim->shared.signal_lock);
	if (sim->dongles)
		free(sim->dongles);
	if (sim->coder)
		free(sim->coder);
}
