/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dabae <dabae@student.42perpignan.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/04 16:58:54 by dabae             #+#    #+#             */
/*   Updated: 2023/10/12 11:34:30 by dabae            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "libft.h"

static	int	ft_count_digit(int n)
{
	int	count;

	count = 1;
	if (n < 0)
	{
		n *= -1;
		count++;
	}
	if (n > 9)
	{
		while (n > 9)
		{
			n /= 10;
			count++;
		}
	}
	return (count);
}

char	*ft_itoa(int n)
{
	long	i;
	char	*res;
	int		count;

	i = n;
	if (n == -2147483648)
		return (ft_strdup("-2147483648"));
	count = ft_count_digit(n);
	res = (char *)malloc(sizeof(char) * (count + 1));
	if (!res)
		return (NULL);
	if (n < 0)
		n *= -1;
	res[count] = '\0';
	while (--count >= 0)
	{
		res[count] = n % 10 + '0';
		n /= 10;
	}
	if (i < 0)
		res[0] = '-';
	return (res);
}
/*
#include <stdio.h>
int	main()
{
	long	n = -2147483648;
	printf("%s\n", ft_itoa(34235));
	printf("%s\n", ft_itoa(n));
	printf("%s\n", ft_itoa(0));
	return (0);
}
*/
