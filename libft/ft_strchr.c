/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dabae <dabae@student.42perpignan.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/03 13:45:45 by dabae             #+#    #+#             */
/*   Updated: 2023/10/12 12:04:07 by dabae            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "libft.h"

char	*ft_strchr(const char *s, int c)
{
	while (*s && ((unsigned char)*s != (unsigned char)c))
		s++;
	if ((unsigned char)*s != (unsigned char)c)
		return (NULL);
	else
		return ((char *)s);
}
