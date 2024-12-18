/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dabae <dabae@student.42perpignan.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/03 11:05:49 by dabae             #+#    #+#             */
/*   Updated: 2023/10/10 17:01:23 by dabae            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t size)
{
	size_t	len_dst;
	size_t	len_src;
	size_t	i;
	size_t	left;

	len_dst = 0;
	len_src = ft_strlen(src);
	while (dst[len_dst] && len_dst < size)
		len_dst++;
	if (size > len_dst)
		left = size - len_dst - 1;
	else
		left = 0;
	i = 0;
	while (src[i] && i < left)
	{
		dst[len_dst + i] = src[i];
		i++;
	}
	if (len_dst < size)
		dst[len_dst + i] = '\0';
	return (len_dst + len_src);
}
