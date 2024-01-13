/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestChecker.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aouchaad <aouchaad@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 15:29:17 by aouchaad          #+#    #+#             */
/*   Updated: 2024/01/13 16:07:00 by aouchaad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*this checker cheks if the request is completed*/
#include <iostream>

std::string extructBoundary(std::string requestData, size_t pos) {
	size_t startPos = pos + 9; // 9 is the length of "boundary="
	size_t endPos = requestData.find("\n", startPos);
	size_t boundaryLength = endPos - startPos;
	std::string boundary = requestData.substr(startPos, boundaryLength);
	boundary = (boundary + "--");
	return boundary;
}

bool requestChecker(std::string requestData) {
	size_t pos = 0;
	if ((pos = requestData.find("\r\n\r\n")) != requestData.npos) {
		if ((pos = requestData.find("boundary")) != requestData.npos) {
			std::string ENDboundary = extructBoundary(requestData, pos);
			if ((pos = requestData.find(ENDboundary)) != requestData.npos)
				return true;
			return false;
		}
		return true;
	}
	
	return false;
}