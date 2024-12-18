/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dabae <dabae@student.42perpignan.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/03 17:08:53 by dabae             #+#    #+#             */
/*   Updated: 2023/10/11 15:36:25 by dabae            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "libft.h" 

void	*ft_calloc(size_t nmemb, size_t size)
{
	void	*arr;
	size_t	total;

	total = nmemb * size;
	if (nmemb == 0 || size == 0)
		return (malloc(0));
	else if (total / nmemb != size || total / size != nmemb)
		return (NULL);
	else
	{
		arr = (void *)malloc(total);
		if (!arr)
			return (NULL);
		ft_bzero(arr, total);
	}
	return (arr);
}
