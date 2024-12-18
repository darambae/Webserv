/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dabae <dabae@student.42perpignan.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/04 14:14:45 by dabae             #+#    #+#             */
/*   Updated: 2024/03/26 13:32:05 by dabae            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static	int	ft_count_arr(char const *s, char c)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	while (s[i] && s)
	{
		if (s[i] != c)
		{
			count++;
			while (s[i] != c && s[i])
				i++;
		}
		else
			i++;
	}
	return (count);
}

static	int	ft_size_wd(char const *s, char c, int i)
{
	int	size;

	size = 0;
	while (s[i] != c && s[i])
	{
		size++;
		i++;
	}
	return (size);
}

char	**ft_split(char const *s, char c)
{
	char	**arrs;
	int		i;
	int		z;
	int		size;

	arrs = (char **)malloc(sizeof(char *) * (ft_count_arr(s, c) + 1));
	if (!arrs)
		return (NULL);
	i = 0;
	z = -1;
	while (++z < ft_count_arr(s, c))
	{
		while (s[i] == c)
			i++;
		size = ft_size_wd(s, c, i);
		arrs[z] = ft_substr(s, i, size);
		if (!arrs[z])
			return (ft_free_tab(arrs));
		i += size;
	}
	arrs[z] = NULL;
	return (arrs);
}
