/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bryfajar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 14:08:45 by bryfajar          #+#    #+#             */
/*   Updated: 2026/11/03 14:08:49 by bryfajar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

static int	req_less(t_request a, t_request b)
{
	if (a.key != b.key)
		return (a.key < b.key);
	return (a.coder_id < b.coder_id);
}

static void	swap_req(t_request *a, t_request *b)
{
	t_request	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

void	heap_push(t_dongle *dongle, t_request req)
{
	int	i;
	int	parent;

	i = dongle->queue_size;
	dongle->queue[i] = req;
	dongle->queue_size++;
	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (!req_less(dongle->queue[i], dongle->queue[parent]))
			break ;
		swap_req(&dongle->queue[i], &dongle->queue[parent]);
		i = parent;
	}
}

t_request	heap_peek(t_dongle *dongle)
{
	return (dongle->queue[0]);
}

void	heap_pop(t_dongle *dongle)
{
	int	i;
	int	child;

	dongle->queue_size--;
	dongle->queue[0] = dongle->queue[dongle->queue_size];
	i = 0;
	while (2 * i + 1 < dongle->queue_size)
	{
		child = 2 * i + 1;
		if (child + 1 < dongle->queue_size
			&& req_less(dongle->queue[child + 1], dongle->queue[child]))
			child++;
		if (!req_less(dongle->queue[child], dongle->queue[i]))
			break ;
		swap_req(&dongle->queue[i], &dongle->queue[child]);
		i = child;
	}
}
