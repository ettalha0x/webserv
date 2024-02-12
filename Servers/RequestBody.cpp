/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestBody.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aouchaad <aouchaad@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 19:44:51 by aouchaad          #+#    #+#             */
/*   Updated: 2024/02/10 19:48:32 by aouchaad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestBody.hpp"

std::string Body::GetContent(void) const {
	return this->content;
}
std::string Body::GetFileName(void) const {
	return this->fileName;
}
size_t Body::GetBodySize(void) const {
	return this->bodySize;
}

void Body::parseBody(std::string type){
	
}