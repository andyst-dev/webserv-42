#include "../../includes/HTTP-Core/Client.hpp"
#include <unistd.h>
#include <cstring>
#include <iostream>

Client::Client(int clientFd, int servFd, const std::string& clientIp, int clientPort)
	: fd(clientFd), serverFd(servFd), ip(clientIp), port(clientPort), requestComplete(false), responseSent(false)
{
}

Client::~Client()
{
	if (fd != -1)
		close(fd);
}

int Client::getFd() const { return fd; }
int Client::getServerFd() const { return serverFd; }
const std::string& Client::getIp() const { return ip; }
int Client::getPort() const { return port; }

bool Client::isRequestComplete() const { return requestComplete; }
bool Client::isResponseSent() const { return responseSent; }

void Client::receiveData()
{
	char buf[1024];
	ssize_t n = recv(fd, buf, sizeof(buf), 0);
	if (n > 0)
	{
		buffer.append(buf, n);
		if (parser.parse(buffer.c_str(), buffer.size(), request))
		{
			requestComplete = true;
		}
	}
	else if (n == 0)
	{
		// Client disconnected
		requestComplete = true;
		responseSent = true;
	}
	else
	{
		// Error
		requestComplete = true;
		responseSent = true;
	}
}

void Client::sendResponse()
{
	std::string data = response.serialize();
	size_t sent = 0;
	while (sent < data.size())
	{
		ssize_t n = send(fd, data.c_str() + sent, data.size() - sent, 0);
		if (n > 0)
			sent += n;
		else
			break;
	}
	responseSent = true;
}

const HttpRequest& Client::getRequest() const { return request; }
void Client::setResponse(const HttpResponse& resp) { response = resp; }