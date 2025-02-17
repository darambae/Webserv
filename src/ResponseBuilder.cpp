#include "../include/ResponseBuilder.hpp"

std::string*	ResponseBuilder::buildResponse(std::string body) {

	if (!body.empty())
		_body = body;
	else {
		std::stringstream buffer;
		buffer << _setup.getRequestedFile().rdbuf();
		_body = buffer.str();
	}

	initMimeTypes();

	_builtResponse = buildFirstLine();
	_builtResponse += buildHeaders();
	_builtResponse += _body;

	return &_builtResponse;
}

std::string	ResponseBuilder::buildFirstLine() {

	std::string	firstLine = _setup.getRequest().getVersion() + " " + _setup.getCodeStatus() + " " + _setup.getReasonPhrase() + "\r\n";
	return firstLine;
}

std::string	ResponseBuilder::buildHeaders() {
	
	std::string	header;

	_headers._timeStamp = "Date: " + buildTime() + "\r\n";
	_headers._contentType = "Content-Type: " + buildContentType() + "\r\n";
	_headers._contentLength = "Content-Length: " + to_string(_body.size()) + "\r\n\r\n";

	return (header = _headers._timeStamp + _headers._contentType + _headers._contentLength);
}

std::string	ResponseBuilder::buildTime(void) {

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

std::string	ResponseBuilder::buildContentType() {
	std::string	requestedFilePath = _setup.getRequestedFilePath();
	std::string	contentType;

	size_t	pos = requestedFilePath.find_last_of('.');
	if (pos != std::string::npos) {
		std::string fileExtension = requestedFilePath.substr(pos + 1);
		std::map<std::string, std::string>::const_iterator	it = _mimeTypes.begin();
		for (; it != _mimeTypes.end(); ++it) {
			if (it->first == fileExtension) {
				contentType = it->second;
				LOG_INFO("Content-Type: " + contentType);
				break ;
			}
		}
	}
	return contentType;
}


void	ResponseBuilder::initMimeTypes() {

	// Text Based Types
	_mimeTypes.insert(std::make_pair("html", "text/html"));
	_mimeTypes.insert(std::make_pair("htm", "text/htm"));
	_mimeTypes.insert(std::make_pair("txt", "text/plain"));
	_mimeTypes.insert(std::make_pair("css", "text/css"));
	_mimeTypes.insert(std::make_pair("xml", "text/xml"));
	// Application Content Types
	_mimeTypes.insert(std::make_pair("js", "application/javascript"));
	_mimeTypes.insert(std::make_pair("json", "application/json"));
	_mimeTypes.insert(std::make_pair("pdf", "application/pdf"));
	_mimeTypes.insert(std::make_pair("zip", "application/zip"));
	// Image Content Types
	_mimeTypes.insert(std::make_pair("jpeg", "image/jpeg"));
	_mimeTypes.insert(std::make_pair("jpg", "image/jpg"));
	_mimeTypes.insert(std::make_pair("png", "image/png"));
	_mimeTypes.insert(std::make_pair("gif", "image/gif"));
	_mimeTypes.insert(std::make_pair("webp", "image/webp"));
	_mimeTypes.insert(std::make_pair("bmp", "image/bmp"));
	_mimeTypes.insert(std::make_pair("ico", "image/x-icon"));
	// Audio Content Types
	_mimeTypes.insert(std::make_pair("mp3", "audio/mp3"));
	_mimeTypes.insert(std::make_pair("mpeg", "audio/mpeg"));
	_mimeTypes.insert(std::make_pair("ogg", "audio/ogg"));
	_mimeTypes.insert(std::make_pair("wav", "audio/wav"));
	// Video Content Types
	_mimeTypes.insert(std::make_pair("mp4", "video/mp4"));
	_mimeTypes.insert(std::make_pair("webm", "video/webm"));
	_mimeTypes.insert(std::make_pair("ogv", "video/ogv"));

}