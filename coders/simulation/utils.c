/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bryfajar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/13 10:31:44 by bryfajar          #+#    #+#             */
/*   Updated: 2026/10/15 16:45:21 by bryfajar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../codexion.h"

long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void	print_note(t_shared *shared, int id, char *text)
{
	pthread_mutex_lock(&shared->log_lock);
	if (check_simulation_running(shared))
		printf("%ld %d %s\n", get_time_ms() - shared->start_time, id, text);
	pthread_mutex_unlock(&shared->log_lock);
}

int	can_take_dongle(t_dongle *dongle)
{
	return (dongle->is_taken == 0 && get_time_ms() >= dongle->free_ms);
}
