#include "../include/Response.hpp"

void	Response::buildResponse() {

	std::stringstream buffer;
	buffer << _setup.getRequestedFile().rdbuf();
	_body = buffer.str();

	_builtResponse = buildFirstLine();
	_builtResponse += buildHeaders();
	_builtResponse += _body;
}

std::string	Response::buildHeaders() {
	_headers._timeStamp = "Date: " + buildTime() + "\r\n";
	_headers._contentType = buildContentType() + "\r\n";
}

std::string	Response::buildFirstLine() {

	std::string	firstLine = _setup.getRequest().getVersion() + " " + to_string(_setup.getCodeStatus()) + " " + _setup.getReasonPhrase() + "\r\n";
	return firstLine;
}

std::string	Response::buildContentType() {
	//to do
}

std::string	Response::buildTime(void) {

	time_t _timestamp;

	std::stringstream result;
	std::string weekDay, month, day, hours, year;
	std::string	timeZone = "GMT";

	// Get the current time.
	time(&_timestamp);
	std::stringstream buffTime;
	buffTime << ctime(&_timestamp);

	// Dispatch the ctime format accross the corresponding strings
	// ? ctime format = Sat Oct 19 10:31:54 2024
	buffTime >> weekDay >> month >> day >> hours >> year;

	// Rearrange time to match HTTP standarts.
	// ? ex : Wed, 11 Oct 2024 10:24:12 GMT
	result	<< weekDay << ", " << day << " " << month << " " << year << " " << hours << " " << timeZone;

	return result.str();
}