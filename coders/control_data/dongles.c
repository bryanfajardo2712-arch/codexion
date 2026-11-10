/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bryfajar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/08 14:16:03 by bryfajar          #+#    #+#             */
/*   Updated: 2026/10/15 16:36:14 by bryfajar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

void	destroy_dongles(t_dongle *dongles, long count)
{
	while (0 < count)
	{
		--count;
		pthread_mutex_destroy(&dongles[count].lock);
	}
	free (dongles);
}

t_dongle	*create_dongle(t_config *config)
{
	t_dongle	*dongles;
	long		i;

	i = 0;
	dongles = malloc(sizeof(t_dongle) * config->number_of_coders);
	if (!dongles)
		return (NULL);
	while (i < config->number_of_coders)
	{
		dongles[i].is_taken = 0;
		dongles[i].free_ms = 0;
		dongles[i].queue_size = 0;
		if (pthread_mutex_init(&dongles[i].lock, NULL) != 0)
		{
			destroy_dongles(dongles, i);
			return (NULL);
		}
		i++;
	}
	return (dongles);
}
