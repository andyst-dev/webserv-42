#include "../../includes/HTTP-Core/RequestHandler.hpp"
#include <fstream>
#include <cstdio>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <limits.h>

RequestHandler::RequestHandler(const std::string& docRoot) : documentRoot(docRoot) {
	// Convertir en path absolu
	char cwd[PATH_MAX];
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		documentRoot = std::string(cwd) + "/" + docRoot;
	}
}

std::string RequestHandler::resolvePath(const std::string& path) {
	if (path.empty() || path[0] == '/') return path;
	char cwd[PATH_MAX];
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		return std::string(cwd) + "/" + path;
	}
	return path;
}

void RequestHandler::setDocumentRoot(const std::string& root) {
	documentRoot = root;
}

HttpResponse RequestHandler::handleRequest(const HttpRequest& request, const ServerConfig& server, const LocationConfig* location) {
	// Validation de base
	if (!request.isValid || !request.isComplete) {
		return HttpResponse::badRequest();
	}
	
	// Check body size
	if (request.body.size() > server.getClientMaxBodySize()) {
		return HttpResponse(413, "Request Entity Too Large");
	}
	
	// Router selon la méthode HTTP (obligatoire selon sujet)
	if (request.method == "GET") {
		return handleGet(request, server, location);
	} else if (request.method == "POST") {
		return handlePost(request, server, location);
	} else if (request.method == "DELETE") {
		return handleDelete(request, server, location);
	} else {
		// Méthode non supportée
		return HttpResponse::methodNotAllowed();
	}
}

HttpResponse RequestHandler::handleGet(const HttpRequest& request, const ServerConfig& server, const LocationConfig* location) {
	std::string root = resolvePath(location && !location->getRoot().empty() ? location->getRoot() : server.getRoot());
	std::string filepath = root + request.uri;
	
	// Sécurité : empêcher l'accès en dehors du document root
	if (request.uri.find("..") != std::string::npos) {
		return HttpResponse::badRequest();
	}
	
	struct stat st;
	if (stat(filepath.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
		// Directory
		std::string index = location ? location->getIndex() : server.getIndex();
		if (!index.empty()) {
			std::string indexPath = filepath + "/" + index;
			if (fileExists(indexPath)) {
				filepath = indexPath;
			} else if (location && location->getAutoIndex()) {
				return HttpResponse::ok(generateDirectoryListing(filepath, request.uri));
			} else {
				return HttpResponse::notFound();
			}
		} else if (location && location->getAutoIndex()) {
			return HttpResponse::ok(generateDirectoryListing(filepath, request.uri));
		} else {
			return HttpResponse::notFound();
		}
	}
	
	// Check CGI
	if (location && location->hasCGI(request.uri)) {
		return executeCGI(request, server, location);
	}
	
	// Vérifier que le fichier existe
	if (!fileExists(filepath)) {
		return HttpResponse::notFound();
	}
	
	// Lire le fichier
	std::string content = readFile(filepath);
	if (content.empty() && errno != 0) {
		return HttpResponse::internalServerError();
	}
	
	// Créer la réponse avec le bon MIME type
	HttpResponse response = HttpResponse::ok(content);
	response.setContentType(getMimeType(filepath));
	
	return response;
}

HttpResponse RequestHandler::handlePost(const HttpRequest& request, const ServerConfig& server, const LocationConfig* location) {
	// Check CGI
	if (location && location->hasCGI(request.uri)) {
		return executeCGI(request, server, location);
	}
	
	// Simple file upload
	std::string uploadDir = resolvePath(location ? location->getUploadDir() : server.getUpload());
	if (uploadDir.empty()) {
		return HttpResponse::methodNotAllowed();
	}
	
	std::string filepath = uploadDir + request.uri;
	
	// Créer le fichier avec le contenu du body
	std::ofstream file(filepath.c_str(), std::ios::binary);
	if (!file.is_open()) {
		return HttpResponse::internalServerError();
	}
	
	file.write(request.body.c_str(), request.body.length());
	file.close();
	
	// Retourner 201 Created
	return HttpResponse::created();
}

HttpResponse RequestHandler::handleDelete(const HttpRequest& request, const ServerConfig& server, const LocationConfig* location) {
	std::string root = resolvePath(location && !location->getRoot().empty() ? location->getRoot() : server.getRoot());
	std::string filepath = root + request.uri;
	
	// Sécurité
	if (request.uri.find("..") != std::string::npos) {
		return HttpResponse::badRequest();
	}
	
	// Vérifier que le fichier existe
	if (!fileExists(filepath)) {
		return HttpResponse::notFound();
	}
	
	// Supprimer le fichier
	if (remove(filepath.c_str()) == 0) {
		return HttpResponse::noContent(); // 204 No Content
	} else {
		return HttpResponse::internalServerError();
	}
}

std::string RequestHandler::generateDirectoryListing(const std::string& dirPath, const std::string& uri) {
	std::string html = "<html><head><title>Index of " + uri + "</title></head><body><h1>Index of " + uri + "</h1><ul>";
	
	DIR* dir = opendir(dirPath.c_str());
	if (dir) {
		struct dirent* entry;
		while ((entry = readdir(dir)) != NULL) {
			std::string name = entry->d_name;
			if (name != "." && name != "..") {
				html += "<li><a href=\"" + uri + (uri.empty() || uri[uri.size()-1] == '/' ? "" : "/") + name + "\">" + name + "</a></li>";
			}
		}
		closedir(dir);
	}
	html += "</ul></body></html>";
	return html;
}

HttpResponse RequestHandler::executeCGI(const HttpRequest& request, const ServerConfig& server, const LocationConfig* location) {
	std::string cgiPath = location->getCGI(request.uri);
	std::string root = resolvePath(server.getRoot());
	std::string scriptPath = root + request.uri;
	
	int pipefd[2];
	if (pipe(pipefd) == -1) {
		return HttpResponse::internalServerError();
	}
	
	pid_t pid = fork();
	if (pid == -1) {
		close(pipefd[0]);
		close(pipefd[1]);
		return HttpResponse::internalServerError();
	}
	
	if (pid == 0) {
		// Child
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		
		// Set environment variables
		setenv("REQUEST_METHOD", request.method.c_str(), 1);
		setenv("SCRIPT_FILENAME", scriptPath.c_str(), 1);
		std::ostringstream oss;
		oss << request.body.size();
		setenv("CONTENT_LENGTH", oss.str().c_str(), 1);
		setenv("CONTENT_TYPE", request.headers.count("content-type") ? request.headers.at("content-type").c_str() : "", 1);
		
		execl(cgiPath.c_str(), cgiPath.c_str(), scriptPath.c_str(), NULL);
		exit(1);
	} else {
		// Parent
		close(pipefd[1]);
		std::string output;
		char buf[1024];
		ssize_t n;
		while ((n = read(pipefd[0], buf, sizeof(buf))) > 0) {
			output.append(buf, n);
		}
		close(pipefd[0]);
		waitpid(pid, NULL, 0);
		
		HttpResponse resp = HttpResponse::ok(output);
		resp.setContentType("text/html");
		return resp;
	}
}

std::string RequestHandler::getMimeType(const std::string& filename) {
	size_t dotPos = filename.find_last_of('.');
	if (dotPos == std::string::npos) {
		return "application/octet-stream";
	}
	
	std::string extension = filename.substr(dotPos + 1);
	
	// MIME types basiques selon le sujet webserv
	if (extension == "html" || extension == "htm") return "text/html";
	if (extension == "css") return "text/css";
	if (extension == "js") return "application/javascript";
	if (extension == "json") return "application/json";
	if (extension == "txt") return "text/plain";
	if (extension == "jpg" || extension == "jpeg") return "image/jpeg";
	if (extension == "png") return "image/png";
	if (extension == "gif") return "image/gif";
	if (extension == "pdf") return "application/pdf";
	
	return "application/octet-stream";
}

std::string RequestHandler::readFile(const std::string& filepath) {
	std::ifstream file(filepath.c_str(), std::ios::binary);
	if (!file.is_open()) {
		return "";
	}
	
	// Lire tout le fichier
	file.seekg(0, std::ios::end);
	size_t size = file.tellg();
	file.seekg(0, std::ios::beg);
	
	std::string content(size, '\0');
	file.read(&content[0], size);
	file.close();
	
	return content;
}

bool RequestHandler::fileExists(const std::string& filepath) {
	struct stat st;
	return (stat(filepath.c_str(), &st) == 0 && S_ISREG(st.st_mode));
}
