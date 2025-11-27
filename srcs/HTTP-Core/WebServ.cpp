#include "../../includes/HTTP-Core/WebServ.hpp"

/*------------------------------- CONSTRUCTOR --------------------------------*/

WebServ::WebServ(const std::vector<ServerConfig>& servers) : m_servers(servers), m_handler("./www")
{
	setupSocket();
}

WebServ::WebServ(const WebServ &copy) : 
m_servers(copy.m_servers),
m_socketToServer(copy.m_socketToServer)
{
	*this = copy;
}

/*------------------------------- DESTRUCTOR --------------------------------*/

WebServ::~WebServ()
{
	for (size_t i = 0; i < m_clients.size(); ++i)
		delete m_clients[i];
	for (size_t i = 0; i < m_sockets.size(); ++i)
		close(m_sockets[i]);
}

/*------------------------------- OVERLOAD OPERATOR --------------------------------*/

WebServ& WebServ::operator=(const WebServ& copy)
{
	if (this != &copy)
	{
		this->m_servers = copy.m_servers;
		this->m_socketToServer = copy.m_socketToServer;
	}
	return (*this);
}

/*------------------------------- FUNCTIONS --------------------------------*/

void	WebServ::setupSocket(void)
{
	std::set<std::string> tmp;
	for (size_t i = 0; i < this->m_servers.size(); i++)
	{
		if (tmp.find(this->m_servers[i].getListen()) == tmp.end())
		{
			tmp.insert(this->m_servers[i].getListen());
			int socketFD = this->createSocket(this->m_servers[i]);
			for (size_t k = 0; k < this->m_servers.size(); k++)
			{
				if (this->m_servers[i].getListen() == this->m_servers[k].getListen())
					this->m_socketToServer[socketFD].push_back(&this->m_servers[k]);
			}
		}
	}
}

int	WebServ::createSocket(ServerConfig &server)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
		throw std::runtime_error("Socket creation failed");
	std::string listenStr = server.getListen();
	std::string ip = listenStr.substr(0, listenStr.find(':'));
	size_t port = std::atoi(listenStr.substr(listenStr.find(':') + 1, listenStr.size()).c_str());
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
	if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		perror("bind");
		throw std::runtime_error("Bind failed on " + listenStr);
	}
	if (listen(sockfd, 128) == -1)
		throw std::runtime_error("Listen failed");
	// Set non-blocking
	fcntl(sockfd, F_SETFL, O_NONBLOCK);
	this->m_sockets.push_back(sockfd);
	std::cout << "Listening on " << listenStr << std::endl;
	return (sockfd);
}

void WebServ::run()
{
	std::cout << "Starting run loop" << std::endl;
	std::vector<pollfd> fds;
	for (size_t i = 0; i < m_sockets.size(); ++i)
	{
		pollfd pfd;
		pfd.fd = m_sockets[i];
		pfd.events = POLLIN;
		pfd.revents = 0;
		fds.push_back(pfd);
	}

	while (true)
	{
		try {
			// Add client fds
			size_t serverCount = m_sockets.size();
			for (size_t i = 0; i < m_clients.size(); ++i)
			{
				pollfd pfd;
				pfd.fd = m_clients[i]->getFd();
				pfd.events = POLLIN;
				if (m_clients[i]->isRequestComplete() && !m_clients[i]->isResponseSent())
					pfd.events |= POLLOUT;
				pfd.revents = 0;
				fds.push_back(pfd);
			}

			int ret = poll(&fds[0], fds.size(), -1);
			if (ret < 0)
			{
				perror("poll error");
				break;
			}
			if (ret == 0)
			{
				std::cout << "poll timeout" << std::endl;
				continue;
			}

			size_t fdIndex = 0;
			// Handle server sockets
			for (size_t i = 0; i < serverCount; ++i, ++fdIndex)
			{
				if (fds[fdIndex].revents & POLLIN)
				{
					acceptClient(m_sockets[i]);
				}
			}

			// Handle clients
			for (size_t i = 0; i < m_clients.size(); ++i, ++fdIndex)
			{
				Client* client = m_clients[i];
				if (fds[fdIndex].revents & POLLIN && !client->isRequestComplete())
				{
					client->receiveData();
				}
				if (client->isRequestComplete() && !client->isResponseSent() && (fds[fdIndex].revents & POLLOUT))
				{
					handleClient(client);
					client->sendResponse();
					removeClient(client);
					--i; // Adjust index
					--fdIndex;
				}
			}

			// Resize fds to server count
			fds.resize(serverCount);
		} catch (const std::exception& e) {
			std::cerr << "Exception in run: " << e.what() << std::endl;
			break;
		}
	}
}

void WebServ::acceptClient(int serverFd)
{
	struct sockaddr_in clientAddr;
	socklen_t addrLen = sizeof(clientAddr);
	int clientFd = accept(serverFd, (struct sockaddr*)&clientAddr, &addrLen);
	if (clientFd < 0)
	{
		perror("accept");
		return;
	}
	fcntl(clientFd, F_SETFL, O_NONBLOCK);
	char ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &clientAddr.sin_addr, ip, INET_ADDRSTRLEN);
	int port = ntohs(clientAddr.sin_port);
	std::cout << "Accepted client " << ip << ":" << port << std::endl;
	Client* client = new Client(clientFd, serverFd, ip, port);
	m_clients.push_back(client);
}

void WebServ::handleClient(Client* client)
{
	const HttpRequest& req = client->getRequest();
	// Resolve server and location
	std::string host = req.headers.count("Host") ? req.headers.at("Host") : "";
	ServerConfig* server = resolveServer(host, client->getServerFd());
	if (!server)
		server = &m_servers[0];
	LocationConfig* location = resolveLocation(req.uri, *server);
	HttpResponse resp = m_handler.handleRequest(req, *server, location);
	client->setResponse(resp);
}

void WebServ::removeClient(Client* client)
{
	for (std::vector<Client*>::iterator it = m_clients.begin(); it != m_clients.end(); ++it)
	{
		if (*it == client)
		{
			delete client;
			m_clients.erase(it);
			break;
		}
	}
}

ServerConfig* WebServ::resolveServer(const std::string &host, int socket)
{
	try {
		std::vector<ServerConfig *> serverBlockPerSocket = this->getServerBySocket(socket);
		for (size_t i = 0; i < serverBlockPerSocket.size(); i++)
		{
			if (serverBlockPerSocket[i]->getServerName() == host)
				return (serverBlockPerSocket[i]);
		}
		return (serverBlockPerSocket[0]);
	}
	catch (const std::runtime_error &e)
	{
		throw std::runtime_error("No match found for socket");
	}
}

LocationConfig* WebServ::resolveLocation(const std::string &uri, ServerConfig server)
{
	std::vector<LocationConfig> locationsServers = server.getLocations();
	size_t occurence = 0;
	size_t tmp = 0;
	size_t k = 0;
	for (size_t i = 0; i < locationsServers.size(); i++)
	{
		size_t countFind = 0;
		size_t countFind2 = uri.find_first_of('/', countFind + 1);
		if (locationsServers[i].getPath() == uri)
			return (&locationsServers[i]);
		while (locationsServers[i].getPath().compare(countFind, countFind2, uri.substr(countFind, countFind2)) == 0)
		{
			tmp++;
			if (tmp > occurence)
			{
				k = i;
				occurence++;
			}
			countFind += uri.find_first_of('/', countFind + 1);
			countFind2 += uri.find_first_of('/', countFind + 1);
		}
	}
	if (occurence > 0)
		return (&locationsServers[k]);
	return (NULL);
}

std::string WebServ::buildPath(const std::string &uri, LocationConfig *location, ServerConfig *server)
{
	if (!location || location->getRoot().empty())
		return (server->getRoot() + uri);
	else if (!location->getRoot().empty())
		return (location->getRoot() + uri);
	return (server->getRoot() + uri);
}

/*------------------------------- GETTERS --------------------------------*/

std::vector<ServerConfig*>	WebServ::getServerBySocket(int socket)
{
	if (this->m_socketToServer.find(socket) != this->m_socketToServer.end())
		return (this->m_socketToServer[socket]);
	throw std::runtime_error("ServerConfig not found for socket");
}

std::string WebServ::getMime(const std::string &uri)
{
	std::string tmp = uri.substr(uri.find_first_of('.') + 1, uri.size());
	switch(tmp[0]) {
		case 'p':
			switch(tmp[1]) {
				case 'n': return ("image/png");
				case 'd': return ("application/pdf");
			}
			break;
		case 'j': 
			switch(tmp[1]) {
				case 'p': return ("image/jpeg");
				case 's': 
					switch (tmp[2]) {
						case 'o': return ("application/json");
						case '\0': return ("application/javascript");
				}
			}
			break;
		case 'g': return ("image/gif");
		case 's': return ("image/svg+xml");
		case 'b': return ("image/bmp");
		case 'w': return ("image/webp");
		case 'i': return ("image/x-icon");
		case 't': 
			switch(tmp[1]) {
				case 'i': return ("image/tiff");
				case 'x': return ("text/txt");
			}
			break;
		case 'h': return ("text/html");
		case 'c': return ("text/css");
		default: return ("application/octet-stream");
	}
	return ("application/octet-stream");
}

int	WebServ::getStatusError(const std::string &uri, const std::string &method, LocationConfig *location, ServerConfig *server)
{
	if (location && !location->isMethodAllowed(method))
		return (405);
	else if (!location && !server->isMethodAllowed(method))
		return (405);

	std::string finalPath = buildPath(uri, location, server);
	if (access(finalPath.c_str(), F_OK) != 0)
		return (404);   
	if (access(finalPath.c_str(), R_OK) != 0)
		return (403);
	return (200);
}
