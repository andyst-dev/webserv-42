# webserv

A small HTTP/1.1 server developed as part of the 42 curriculum.
This project consists of building a non-blocking web server in C++98 that can parse HTTP requests, generate responses, serve static files, handle uploads and deletions, execute CGI scripts, and run multiple servers from an NGINX-style configuration file.

`webserv` was a good way to go deeper into C++ and network programming through a bigger project focused on sockets, event-driven I/O, HTTP behavior, configuration parsing, process handling, and overall server architecture.

## Features
- Non-blocking HTTP/1.1 server in C++98
- Support for GET, POST, and DELETE methods
- Static file serving
- CGI execution for dynamic content
- NGINX-style configuration parsing
- Multiple servers and ports
- Directory listing support
- Custom error pages and status code handling

## Project structure
- `includes/HTTP-Core/` — request parsing, response building, client state, request handling, and server loop classes
- `includes/parsing/` — configuration structures and parser classes for server and location blocks
- `srcs/HTTP-Core/` — implementation of the HTTP core, including request parsing and response generation
- `srcs/parsing/` — implementation of configuration parsing
- `srcs/main.cpp` — program entry point and server startup from a config file
- `confs/` — example configuration files
- `www/` — static files, CGI test files, upload target, and error pages used by the server
- `Makefile` — builds the `webserv` executable

## Mandatory part
The mandatory part implements a non-blocking HTTP/1.1 server following the subject constraints.

### Program
- `webserv` — loads a configuration file, starts one or more configured servers, and handles HTTP requests from connected clients

### Core behavior
- parses an NGINX-style configuration file
- listens on one or more configured ports
- handles multiple client connections through a single non-blocking event loop
- parses HTTP requests and headers
- generates HTTP responses with proper status codes
- serves static files
- supports GET, POST, and DELETE methods
- executes CGI scripts when configured
- serves custom error pages or fallback default errors when needed
- can expose directory listings depending on configuration

### What happens at runtime
- the program reads the configuration file and builds the server configuration state
- listening sockets are created for the configured servers and ports
- the event loop waits for readable and writable file descriptors
- incoming client data is parsed into HTTP requests
- requests are dispatched to the appropriate handler according to method, path, and configuration
- the server generates a response, sends it back to the client, and keeps managing connections without blocking

### Subject requirements to respect
- the server must stay non-blocking at all times
- all client I/O must go through a single `select()`-style event loop
- the program must not crash or hang on malformed or incomplete requests
- it must support at least GET, POST, and DELETE
- it must be able to serve a static website
- it must support file uploads
- it must support CGI for dynamic content
- it must listen on multiple ports when configured
- status codes and error handling must stay coherent with HTTP behavior

## Notes
This repository comes from a collaborative 42 project built with [HaruSnak](https://github.com/HaruSnak) and [tmoel1](https://github.com/tmoel1).

My parts in this project cover HTTP request parsing and HTTP response generation.
In practice, that includes:
- parsing the request line, headers, and body
- validating the structure of incoming HTTP requests
- handling request completeness and validity state
- building HTTP responses with the correct status line, headers, and body
- preparing standard responses for common server cases

The request/response side is a big part of `webserv`: the server receives raw data from a socket, turns it into a structured HTTP request, then builds a valid HTTP response that a browser or client can understand.
That means correctly identifying the method, URI, version, headers, and body, knowing when a request is complete, and sending back a response that matches the expected protocol behavior.

## Build
Build the project:

```bash
make
```

Clean object files:

```bash
make clean
```

Remove object files and executable:

```bash
make fclean
```

Rebuild everything:

```bash
make re
```

## Output
The project builds one executable:

```bash
webserv
```

## Usage
Run the server with a configuration file:

```bash
./webserv confs/default.conf
```

You can then test it for example with:

```bash
curl http://localhost:8080/
```

## Learning outcomes
This project was my first real introduction to building an HTTP server in C++.
It helped me get more comfortable with:
- socket programming
- non-blocking I/O
- HTTP request parsing
- HTTP response generation
- configuration-driven server design
- CGI execution
- multi-client event loop architecture
- file serving and status code handling
