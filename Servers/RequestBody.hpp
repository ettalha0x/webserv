/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestBody.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aouchaad <aouchaad@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 16:15:48 by aouchaad          #+#    #+#             */
/*   Updated: 2024/02/10 19:44:38 by aouchaad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>

class Body {
	private:
		std::string content;
		std::string fileName;
		size_t		bodySize;
	public:
		std::string GetContent(void)const;
		std::string GetFileName(void)const;
		size_t GetBodySize(void)const;

		void parseBody(std::string type);
};