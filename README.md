<img src="readme/Webserv.png" alt="Webserv" width="900"/>

<div align="center">

# WebServ
### An HTTP/1.1 Server Project at 42 School Using C++98

[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![License][license-shield]][license-url]

</div>

---

## 🇬🇧 English

<details>
<summary><b>📖 Click to expand/collapse English version</b></summary>

### 📖 About

**WebServ** is a compulsory project for 42 School students. It consists of creating an HTTP/1.1 web server in C++98 that implements essential features of a modern web server, including request parsing, response generation, CGI execution, and configuration file management.

This project teaches:
- Network programming with sockets and non-blocking I/O
- HTTP protocol implementation (parsing requests and generating responses)
- Configuration file parsing (NGINX-style syntax)
- CGI (Common Gateway Interface) for dynamic content
- Multi-server and virtual host management
- Error handling and status code management

### 🧠 Skills Learned

By completing the WebServ project, students develop essential skills in C++ programming and network development:

- **Socket programming**: Learning to create, bind, listen, and manage TCP sockets for HTTP communication.
- **Non-blocking I/O**: Using select() for handling multiple client connections simultaneously.
- **HTTP parsing**: Implementing robust parsing of HTTP requests and headers.
- **Response generation**: Creating proper HTTP responses with status codes and content.
- **Configuration parsing**: Reading and validating NGINX-style configuration files.
- **CGI integration**: Executing external programs (PHP, Python) via CGI interface.
- **Multi-threading concepts**: Managing concurrent connections without threads.
- **Error handling**: Implementing comprehensive error pages and status codes.
- **File management**: Serving static files, handling uploads, and directory listing.
- **Code organization**: Structuring large C++ projects with classes and inheritance.

## Approach
I worked on this project with my teammate, [Astoll](https://github.com/42andy), shortly after completing the C++ modules at 42. For people who had just been introduced to object-oriented programming, WebServ was a real challenge but despite our initial doubts, it turned out to be a rather enjoyable project.

[Astoll](https://github.com/42andy) mainly handled the parsing of HTTP requests and the generation of responses.
As for me, I focused on parsing the configuration file, as well as setting up the initial structure for processing HTTP requests.

The rest of the project was split between pair programming especially the core structure of WebServ and individual tasks, such as test tools and certain specific features.

### **Features**

**HTTP/1.1 Parser:** *Complete request analysis with headers and body parsing.*<br>

**HTTP Responses:** *Response generation with proper status codes and content.*<br>

**CGI Support:** *Execution of PHP and Python scripts via CGI interface.*<br>

**Multi-server configuration:** *Support for multiple servers on different ports with virtual hosts.*<br>

**File upload and management:** *POST uploads, DELETE operations, and static file serving.*<br>

**Custom error pages:** *Configurable error pages for different HTTP status codes.*<br>

**Directory listing:** *Autoindex feature for browsing directories.*<br>

### **Features to be added:**

**HTTPS support:** *SSL/TLS encryption for secure connections.*<br>

**Advanced CGI features:** *Support for more scripting languages and better environment handling.*<br>

**Load balancing:** *Distribution of requests across multiple server instances.*<br>

**Caching mechanisms:** *HTTP caching headers and content caching.*<br>

### 📋 Table of Contents

- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Compilation](#compilation)
- [Function Reference](#function-reference)
- [Credits](#credits)

<a name="features"></a>

### ✨ Features

- **Complete HTTP/1.1 server implementation** with request parsing and response generation
- **CGI execution** for PHP and Python scripts
- **Multi-server support** with virtual hosts and different ports
- **File upload and management** with POST and DELETE methods
- **Custom error pages** and status code handling
- **Directory listing** (autoindex) functionality
- **NGINX-style configuration** with location blocks and directives
- **Non-blocking I/O** using select() for concurrent connections
- **Strict C++98 compliance** with 42 School norming standards

<a name="installation"></a>

### 🚀 Installation

```bash
# Clone the repository
git clone https://github.com/HaruSnak/42-webserv
cd 42-webserv
```

<a name="usage"></a>

### 💻 Usage

Compile and run the server with a configuration file:

```bash
make
./webserv confs/default.conf
```

The server will start and display:
```
WebServer started successfully
Listening on socket 3
```

Test with curl:
```bash
curl http://localhost:8080/
```

<a name="project-structure"></a>

### 📂 Project Structure

```
42-webserv/
├── Makefile                    # Build script
├── LICENSE                     # License file
├── README.md                   # This file
├── README-Template.md          # Template for README
├── confs/                      # Configuration files
│   ├── default.conf
│   ├── error1.conf
│   └── eval.conf
├── includes/                   # Header files
│   ├── utils.hpp
│   ├── HTTP-Core/
│   │   ├── Client.hpp
│   │   ├── HttpParser.hpp
│   │   ├── HttpResponse.hpp
│   │   └── RequestHandler.hpp
│   │   └── WebServ.hpp
│   └── parsing/
│       ├── ConfigParser.hpp
│       ├── GenericConfigParser.hpp
│       ├── GenericConfigParser.tpp
│       ├── LocationConfig.hpp
│       ├── LocationConfigParser.hpp
│       ├── ServerConfig.hpp
│       └── ServerConfigParser.hpp
├── readme/                     # README assets
├── srcs/                       # Source files
│   ├── main.cpp
│   ├── errors/
│   │   └── parsing/
│   │       └── errors.cpp
│   ├── HTTP-Core/
│   │   ├── Client.cpp
│   │   ├── HttpParser.cpp
│   │   ├── HttpResponse.cpp
│   │   ├── RequestHandler.cpp
│   │   └── WebServ.cpp
│   └── parsing/
│       ├── ConfigParser.cpp
│       ├── LocationConfig.cpp
│       ├── LocationConfigParser.cpp
│       ├── ServerConfig.cpp
│       └── ServerConfigParser.cpp
├── test_file.txt
├── test_script.sh
├── www/                        # Web root directory
│   ├── index.html
│   ├── index.htlm
│   ├── test.php
│   ├── upload.html
│   ├── api/
│   │   ├── index.php
│   │   └── test.py
│   ├── errors/
│   │   └── 404.html
│   ├── static/
│   │   ├── index.html
│   │   ├── script.js
│   │   └── style.css
│   └── uploads/
└── testdir/
    └── file.txt
```

<a name="compilation"></a>

### 🔧 Compilation

Compile the project using the Makefile:

```bash
make          # Compile the server
make clean    # Remove object files
make fclean   # Remove executable and object files
make re       # Recompile everything
```

<a name="function-reference"></a>

### 📚 Function Reference

#### Main Functions
- [`main`](srcs/main.cpp) - Server initialization and main loop
- [`WebServ::run`](srcs/HTTP-Core/WebServ.cpp) - Main server loop with select()
- [`HttpParser::parseRequest`](srcs/HTTP-Core/HttpParser.cpp) - HTTP request parsing
- [`RequestHandler::handleRequest`](srcs/HTTP-Core/RequestHandler.cpp) - Request processing logic

#### Key Features
- **Configuration Parsing**: NGINX-style config file parsing with server and location blocks
- **HTTP Methods**: GET, POST, DELETE implementation with proper responses
- **CGI Execution**: External script execution with environment variables
- **File Serving**: Static file serving with MIME type detection
- **Error Handling**: Custom error pages and HTTP status codes

### 👨‍🎓 Note
<p align="left">
    <img src="https://image.noelshack.com/fichiers/2024/11/2/1710273269-100.png"
         alt="100/100" width="180" height="184">
</p>

<a name="credits"></a>

### 📖 Credits

- **42 School Norm**: [Official C++ Coding Standard](https://cdn.intra.42.fr/pdf/pdf/960/norme.en.pdf)
- **CPP Reference**: [C++ Documentation](https://en.cppreference.com/w/)
- **NGINX Documentation**: [Configuration Guide](https://nginx.org/en/docs/beginners_guide.html#conf_structure)
- **HTTP Methods**: [MDN Web Docs](https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods)
- **Non-blocking I/O**: [IBM Documentation](https://www.ibm.com/docs/en/i/7.2.0?topic=designs-example-nonblocking-io-select)

### 📄 License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

</details>

---

## 🇫🇷 Français

<details>
<summary><b>📖 Cliquez pour développer/réduire la version française</b></summary>

### 📖 À propos

**WebServ** est un projet obligatoire pour les étudiants de l'école 42. Il s'agit de créer un serveur web HTTP/1.1 en C++98 qui implémente les fonctionnalités essentielles d'un serveur web moderne, incluant le parsing des requêtes, la génération de réponses, l'exécution CGI et la gestion des fichiers de configuration.

Ce projet enseigne :
- La programmation réseau avec les sockets et l'I/O non-bloquante
- L'implémentation du protocole HTTP (parsing des requêtes et génération de réponses)
- Le parsing de fichiers de configuration (syntaxe style NGINX)
- Le CGI (Common Gateway Interface) pour le contenu dynamique
- La gestion multi-serveur et d'hôtes virtuels
- La gestion d'erreurs et des codes de statut

### 🧠 Compétences acquises

En complétant le projet WebServ, les étudiants développent des compétences essentielles en programmation C++ et développement réseau :

- **Programmation socket** : Apprendre à créer, lier, écouter et gérer des sockets TCP pour la communication HTTP.
- **I/O non-bloquante** : Utiliser select() pour gérer plusieurs connexions client simultanément.
- **Parsing HTTP** : Implémenter un parsing robuste des requêtes et en-têtes HTTP.
- **Génération de réponses** : Créer des réponses HTTP appropriées avec codes de statut et contenu.
- **Parsing de configuration** : Lire et valider des fichiers de configuration style NGINX.
- **Intégration CGI** : Exécuter des programmes externes (PHP, Python) via l'interface CGI.
- **Concepts multi-threading** : Gérer des connexions concurrentes sans threads.
- **Gestion d'erreurs** : Implémenter des pages d'erreur complètes et des codes de statut.
- **Gestion de fichiers** : Servir des fichiers statiques, gérer les uploads et lister les répertoires.
- **Organisation du code** : Structurer de grands projets C++ avec des classes et l'héritage.

## Approche
J'ai travaillé sur ce projet avec mon coéquipier, [Astoll](https://github.com/42andy), peu après avoir terminé les modules C++ à 42. Pour des personnes qui venaient d'être introduites à la programmation orientée objet, WebServ était un vrai défi mais malgré nos doutes initiaux, il s'est avéré être un projet plutôt agréable.

[Astoll](https://github.com/42andy) s'est principalement occupé du parsing des requêtes HTTP et de la génération de réponses.
Quant à moi, je me suis concentré sur le parsing du fichier de configuration, ainsi que sur la mise en place de la structure initiale pour le traitement des requêtes HTTP.

Le reste du projet a été partagé entre la programmation en binôme surtout pour la structure de base de WebServ et des tâches individuelles, telles que les outils de test et certaines fonctionnalités spécifiques.

### **Fonctionnalités**

**Parseur HTTP/1.1 :** *Analyse complète des requêtes avec parsing des en-têtes et du corps.*<br>

**Réponses HTTP :** *Génération de réponses avec codes de statut et contenu appropriés.*<br>

**Support CGI :** *Exécution de scripts PHP et Python via l'interface CGI.*<br>

**Configuration multi-serveur :** *Support pour plusieurs serveurs sur différents ports avec hôtes virtuels.*<br>

**Upload et gestion de fichiers :** *Uploads POST, opérations DELETE et service de fichiers statiques.*<br>

**Pages d'erreur personnalisées :** *Pages d'erreur configurables pour différents codes de statut HTTP.*<br>

**Listing de répertoires :** *Fonctionnalité autoindex pour naviguer dans les répertoires.*<br>

### **Fonctionnalités à ajouter :**

**Support HTTPS :** *Chiffrement SSL/TLS pour des connexions sécurisées.*<br>

**Fonctionnalités CGI avancées :** *Support pour plus de langages de script et meilleure gestion de l'environnement.*<br>

**Équilibrage de charge :** *Distribution des requêtes sur plusieurs instances de serveur.*<br>

**Mécanismes de cache :** *En-têtes de cache HTTP et cache de contenu.*<br>

### 📋 Table des matières

- [Caractéristiques](#caractéristiques)
- [Installation](#installation-1)
- [Utilisation](#utilisation)
- [Structure du projet](#structure-du-projet)
- [Compilation](#compilation-1)
- [Référence des fonctions](#référence-des-fonctions)
- [Crédits](#crédits-1)

<a name="caractéristiques"></a>

### ✨ Caractéristiques

- **Implémentation complète de serveur HTTP/1.1** avec parsing des requêtes et génération de réponses
- **Exécution CGI** pour les scripts PHP et Python
- **Support multi-serveur** avec hôtes virtuels et ports différents
- **Upload et gestion de fichiers** avec méthodes POST et DELETE
- **Pages d'erreur personnalisées** et gestion des codes de statut
- **Listing de répertoires** (fonctionnalité autoindex)
- **Configuration style NGINX** avec blocs serveur et location
- **I/O non-bloquante** utilisant select() pour les connexions concurrentes
- **Conformité stricte C++98** avec les normes de l'école 42

<a name="installation-1"></a>

### 🚀 Installation

```bash
# Cloner le dépôt
git clone https://github.com/HaruSnak/42-webserv
cd 42-webserv
```

<a name="utilisation"></a>

### 💻 Utilisation

Compilez et lancez le serveur avec un fichier de configuration :

```bash
make
./webserv confs/default.conf
```

Le serveur démarrera et affichera :
```
WebServer started successfully
Listening on socket 3
```

Testez avec curl :
```bash
curl http://localhost:8080/
```

<a name="structure-du-projet"></a>

### 📂 Structure du projet

```
42-webserv/
├── Makefile                    # Script de build
├── LICENSE                     # Fichier de licence
├── README.md                   # Ce fichier
├── README-Template.md          # Template pour README
├── confs/                      # Fichiers de configuration
│   ├── default.conf
│   ├── error1.conf
│   └── eval.conf
├── includes/                   # Fichiers d'en-tête
│   ├── utils.hpp
│   ├── HTTP-Core/
│   │   ├── Client.hpp
│   │   ├── HttpParser.hpp
│   │   ├── HttpResponse.hpp
│   │   └── RequestHandler.hpp
│   │   └── WebServ.hpp
│   └── parsing/
│       ├── ConfigParser.hpp
│       ├── GenericConfigParser.hpp
│       ├── GenericConfigParser.tpp
│       ├── LocationConfig.hpp
│       ├── LocationConfigParser.hpp
│       ├── ServerConfig.hpp
│       └── ServerConfigParser.hpp
├── readme/                     # Ressources README
├── srcs/                       # Fichiers sources
│   ├── main.cpp
│   ├── errors/
│   │   └── parsing/
│   │       └── errors.cpp
│   ├── HTTP-Core/
│   │   ├── Client.cpp
│   │   ├── HttpParser.cpp
│   │   ├── HttpResponse.cpp
│   │   ├── RequestHandler.cpp
│   │   └── WebServ.cpp
│   └── parsing/
│       ├── ConfigParser.cpp
│       ├── LocationConfig.cpp
│       ├── LocationConfigParser.cpp
│       ├── ServerConfig.cpp
│       └── ServerConfigParser.cpp
├── test_file.txt
├── test_script.sh
├── www/                        # Répertoire racine web
│   ├── index.html
│   ├── index.htlm
│   ├── test.php
│   ├── upload.html
│   ├── api/
│   │   ├── index.php
│   │   └── test.py
│   ├── errors/
│   │   └── 404.html
│   ├── static/
│   │   ├── index.html
│   │   ├── script.js
│   │   └── style.css
│   └── uploads/
└── testdir/
    └── file.txt
```

<a name="compilation-1"></a>

### 🔧 Compilation

Compilez le projet en utilisant le Makefile :

```bash
make          # Compiler le serveur
make clean    # Supprimer les fichiers objets
make fclean   # Supprimer l'exécutable et les fichiers objets
make re       # Recompiler tout
```

<a name="référence-des-fonctions"></a>

### 📚 Référence des fonctions

#### Fonctions principales
- [`main`](srcs/main.cpp) - Initialisation du serveur et boucle principale
- [`WebServ::run`](srcs/HTTP-Core/WebServ.cpp) - Boucle principale du serveur avec select()
- [`HttpParser::parseRequest`](srcs/HTTP-Core/HttpParser.cpp) - Parsing des requêtes HTTP
- [`RequestHandler::handleRequest`](srcs/HTTP-Core/RequestHandler.cpp) - Logique de traitement des requêtes

#### Fonctionnalités clés
- **Parsing de configuration** : Parsing de fichiers de config style NGINX avec blocs serveur et location
- **Méthodes HTTP** : Implémentation GET, POST, DELETE avec réponses appropriées
- **Exécution CGI** : Exécution de scripts externes avec variables d'environnement
- **Service de fichiers** : Service de fichiers statiques avec détection de type MIME
- **Gestion d'erreurs** : Pages d'erreur personnalisées et codes de statut HTTP

### 👨‍🎓 Note
<p align="left">
    <img src="https://image.noelshack.com/fichiers/2024/11/2/1710273269-100.png"
         alt="100/100" width="180" height="184">
</p>

<a name="crédits-1"></a>

### 📖 Crédits

- **Norme 42** : [Standard C++ officiel](https://cdn.intra.42.fr/pdf/pdf/960/norme.en.pdf)
- **CPP Reference** : [Documentation C++](https://en.cppreference.com/w/)
- **Documentation NGINX** : [Guide de configuration](https://nginx.org/en/docs/beginners_guide.html#conf_structure)
- **Méthodes HTTP** : [MDN Web Docs](https://developer.mozilla.org/fr/docs/Web/HTTP/Reference/Methods)
- **I/O non-bloquante** : [Documentation IBM](https://www.ibm.com/docs/en/i/7.2.0?topic=designs-example-nonblocking-io-select)

### 📄 Licence

Ce projet est sous licence **MIT** - voir le fichier [LICENSE](LICENSE) pour plus de détails.

</details>

---

[contributors-shield]: https://img.shields.io/github/contributors/HaruSnak/42-webserv.svg?style=for-the-badge
[contributors-url]: https://github.com/HaruSnak/42-webserv/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/HaruSnak/42-webserv.svg?style=for-the-badge
[forks-url]: https://github.com/HaruSnak/42-webserv/network/members
[stars-shield]: https://img.shields.io/github/stars/HaruSnak/42-webserv.svg?style=for-the-badge
[stars-url]: https://github.com/HaruSnak/42-webserv/stargazers
[issues-shield]: https://img.shields.io/github/issues/HaruSnak/42-webserv.svg?style=for-the-badge
[issues-url]: https://github.com/HaruSnak/42-webserv/issues
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://www.linkedin.com/in/shany-moreno-5a863b2aa
[license-shield]: https://img.shields.io/github/license/HaruSnak/42-webserv.svg?style=for-the-badge
[license-url]: https://github.com/HaruSnak/42-webserv/blob/master/LICENSE
