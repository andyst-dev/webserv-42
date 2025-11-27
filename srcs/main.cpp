#include "../includes/utils.hpp"
#include "../includes/parsing/ConfigParser.hpp"
#include "../includes/HTTP-Core/WebServ.hpp"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
		return 1;
	}

	try
	{
		ConfigParser parser;
		parser.parseConfigFile(argv[1]);
		std::vector<ServerConfig> servers = parser.getServers();

		WebServ webserv(servers);
		webserv.run();
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
