/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parseo.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bryfajar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/05 15:42:42 by bryfajar          #+#    #+#             */
/*   Updated: 2026/10/15 10:51:53 by bryfajar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

static int	check_min(long value, long min, char *name)
{
	if (value >= min)
		return (0);
	fprintf(stderr, "Error :%s must be a leats %ld\n", name, min);
	return (1);
}

static int	check_limits(t_config *config)
{
	return (check_min(config->number_of_coders, 1, "number_of_coders")
		|| check_min(config->time_to_burnout, 1, "time_to_bornout")
		|| check_min(config->time_to_compile, 1, "time_to_compile")
		|| check_min(config->number_of_compiles_required, 1,
			"number_of_compiles_required")
		|| check_min(config->dongle_cooldown, 0, "dongle_cooldown"));
}

static int	ft_atol(char *number, long *out)
{
	long	n;
	int		digit;

	if (!number || !*number)
		return (1);
	n = 0;
	while (*number >= '0' && *number <= '9')
	{
		digit = *number - '0';
		if (n > (LONG_MAX - digit) / 10)
			return (1);
		n = n * 10 + digit;
		number++;
	}
	if (*number)
		return (1);
	*out = n;
	return (0);
}

static int	parser_scheduler(char *str, t_config *config)
{
	if (strcmp(str, "fifo") == 0)
		config->scheduler = FIFO;
	else if (strcmp(str, "edf") == 0)
		config->scheduler = EDF;
	else
		return (1);
	return (0);
}

int	parse(t_config *config, char **argv)
{
	if (ft_atol(argv[1], &config->number_of_coders) != 0
		|| ft_atol(argv[2], &config->time_to_burnout) != 0
		|| ft_atol(argv[3], &config->time_to_compile) != 0
		|| ft_atol(argv[4], &config->time_to_debug) != 0
		|| ft_atol(argv[5], &config->time_to_refactor) != 0
		|| ft_atol(argv[6], &config->number_of_compiles_required) != 0
		|| ft_atol(argv[7], &config->dongle_cooldown) != 0
		|| parser_scheduler(argv[8], config) != 0)
	{
		fprintf(stderr, "Error: invalid arguments.\n"
			"Usage: ./codexion number_of_coders time_to_burnout "
			"time_to_compile time_to_debug time_to_refactor "
			"number_of_compiles_required dongle_cooldown scheduler\n"
			"Numbers: digits only, no signs or decimals. "
			"Scheduler: fifo or edf.\n");
		return (1);
	}
	if (check_limits(config) != 0)
	{
		fprintf(stderr, "Wrong of parameter\n");
		return (1);
	}
	return (0);
}
