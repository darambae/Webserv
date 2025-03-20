#include "../include/ResponseBuilder.hpp"

std::string*	ResponseBuilder::buildResponse(std::string body) {
	LOG_INFO("Building response");
	std::ifstream& file = _response.getRequestedFile();
	if (file.is_open()) {
		std::stringstream buffer;
		buffer << file.rdbuf();
		_body = buffer.str();
	} else {
		_body = body;
	}

	initMimeTypes();

	_builtResponse = buildFirstLine();
	_builtResponse += buildHeaders();
//	LOG_INFO("Response :\n" + std::string(YELLOW) + _builtResponse);
	if (!_body.empty())
		_builtResponse += _body;
	LOG_DEBUG("Response :\n" + std::string(YELLOW) + _builtResponse);

	return &_builtResponse;
}

std::string	ResponseBuilder::buildFirstLine() {

	if (_response.getCodeStatus() == 200) {
		_response.setResponseStatus(200);
	}
	std::string	firstLine = _response.getRequest().getVersion() + " " + to_string(_response.getCodeStatus()) + " " + _response.getReasonPhrase() + "\r\n";
	return firstLine;
}

std::string	ResponseBuilder::buildHeaders() {
	
	std::string	header;

	_headers._timeStamp = "Date: " + buildTime() + "\r\n";
	header += _headers._timeStamp;
	if(!_response.getRedirectionResponseHeader().empty()) {
		header += "location: " + _response.getRedirectionResponseHeader() + "\r\n";
	}
	if (!_body.empty()) {
		_headers._contentType = "Content-Type: " + buildContentType() + "\r\n";
		header += _headers._contentType;
	}
	_headers._contentLength = "Content-Length: " + to_string(_body.size()) + "\r\n";
	header += _headers._contentLength;
	_headers._connection = "Connection: keep-alive\r\n";
	header += _headers._connection;

	if (_response.getCgiManager() != NULL) {
		std::map<std::string, std::vector<std::string> >	cgiHeaders = _response.getCgiManager()->getCgiHeaders();
		std::map<std::string, std::vector<std::string> >::iterator it = cgiHeaders.begin();
		for (; it != cgiHeaders.end(); ++it) {
			for (size_t i = 0; i < it->second.size(); i++) {
				header += it->first + ": " + it->second[i] + "\r\n";
			}
		}
	}
	header += "\r\n";

	return (header);
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
	std::string	requestedFilePath = _response.getRequestedFilePath();
	std::string	contentType;

	if (requestedFilePath.find(".php") != std::string::npos || requestedFilePath.find(".py") != std::string::npos) {
		contentType = "text/html";
		return contentType;
	}
	size_t	pos = requestedFilePath.find_last_of('.');
	if (pos != std::string::npos) {
		std::string fileExtension = requestedFilePath.substr(pos + 1);
		std::map<std::string, std::string>::const_iterator	it = _mimeTypes.begin();
		for (; it != _mimeTypes.end(); ++it) {
			if (it->first == fileExtension) {
				contentType = it->second;
				break ;
			}
		}
	} else 
		contentType = "text/html";
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
	_mimeTypes.insert(std::make_pair("py", "application/python"));
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