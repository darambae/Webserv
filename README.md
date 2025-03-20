# Webserv

> **Versions**: [English](#webserv) | [Français](#webserv-🇫🇷) | [한국어](#webserv-🇰🇷)

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Usage](#usage)
- [Configuration](#configuration)
- [Request Handling](#request-handling)
- [Error Handling](#error-handling)
- [Supported HTTP Methods](#supported-http-methods)

---

## Introduction

**Webserv** is an Ecole 42 project that involves building a simple HTTP server in C++. Inspired by Nginx, Webserv handles client requests, serves static files, and supports dynamic content processing. Through **Webserv**, you will gain hands-on experience with socket programming, HTTP parsing, multiplexing, and server configurations.

---

## Features

- **Multi-Client Handling**: Uses non-blocking sockets and `poll()` for handling multiple clients simultaneously.
- **HTTP/1.1 Support**: Implements core HTTP features such as request parsing, response handling, and status codes.
- **Configurable Server**: Reads settings from a configuration file to define server behavior (ports, routes, error pages, etc.).
- **Static File Serving**: Serves HTML, CSS, JavaScript, and other static resources.
- **CGI Support**: Executes scripts like PHP or Python via the Common Gateway Interface (CGI).
- **Error Management**: Returns proper HTTP status codes for incorrect requests or missing resources.

---

## Usage

To compile and run the server:

```bash
make
./webserv config_file.conf
```

**Examples**:

- Starting the server with a configuration file:
  ```bash
  ./webserv config/default.conf
  ```
- Sending a GET request to fetch an HTML file:
  ```bash
  curl http://localhost:8080/index.html
  ```
- Sending a POST request to upload data:
  ```bash
  curl -X POST -d "filename=test" http://localhost:8080/upload
  ```

---

## Configuration

Webserv uses a configuration file to define:

- **Listening Ports**: Specify which ports the server should listen on.
- **Server Blocks**: Define multiple virtual hosts.
- **Root Directory**: Set the document root for serving files.
- **Error Pages**: Customize responses for different HTTP status codes.
- **CGI Scripts**: Configure paths for executing scripts.

**Example Configuration File:**

```conf
server {
    listen 8080;
    server_name myserver;
    root /var/www/html;
    index index.html;
    error_page 404 /errors/404.html;
    
    location /cgi-bin/ {
        cgi_pass /usr/bin/python3;
        cgi_extension .py;
    }
}
```

---

## Request Handling

1. **Client Connection**: The server accepts incoming TCP connections.
2. **Request Parsing**: Reads and parses the HTTP request (method, headers, body).
3. **Routing**: Matches the request URL with configured routes.
4. **Response Generation**:
   - Static files are read and returned.
   - CGI scripts are executed, and output is returned.
   - Proper status codes are sent based on request validity.
5. **Response Delivery**: The response is sent back to the client.
6. **Connection Closure**: The connection is closed if needed.

---

## Error Handling

- **400 Bad Request**: Invalid or malformed request.
- **404 Not Found**: Requested file or directory does not exist.
- **405 Method Not Allowed**: HTTP method not supported.
- **408 Request Timeout**: The server timed out waiting for the client to send a complete request within the allowed time.
- **413 Payload Too Large**: The server is refusing to process a request because the request payload exceeds the server's configured limit.

---

## Supported HTTP Methods

### `GET`

- **Usage**: Requests a resource from the server.
- **Example**:
  ```bash
  curl http://localhost:8080/index.html
  ```

### `POST`

- **Usage**: Sends data to the server (e.g., form submissions, file uploads).
- **Example**:
  ```bash
  curl -X POST -d "message=hello" http://localhost:8080/message
  ```

### `DELETE`

- **Usage**: Requests the removal of a resource from the server.
- **Example**:
  ```bash
  curl -X DELETE http://localhost:8080/file.txt
  ```

---

# Webserv 🇫🇷

## Table des Matières

- [Introduction](#introduction)
- [Fonctionnalités](#fonctionnalités)
- [Configuration](#configuration)
- [Traitement des Requêtes](#traitement-des-requêtes)
- [Gestion des Erreurs](#gestion-des-erreurs)
- [Méthodes HTTP Prises en Charge](#méthodes-http-prises-en-charge)

---

## Introduction

**Webserv** est un projet de l'École 42 visant à développer un serveur HTTP simple en C++. Inspiré de Nginx, Webserv gère les requêtes clients, sert des fichiers statiques et prend en charge le traitement dynamique des contenus.

---

## Fonctionnalités

- **Gestion Multi-Clients** : Utilisation de sockets non bloquants et de `poll()`.
- **Support HTTP/1.1** : Implémente les fonctionnalités de base du protocole HTTP.
- **Configuration Personnalisable** : Lecture d'un fichier de configuration pour définir les paramètres du serveur.
- **Support CGI** : Exécution de scripts PHP ou Python via CGI.
- **Gestion des Erreurs** : Réponses adaptées avec les codes HTTP appropriés.

---

## Configuration

**Exemple de fichier de configuration :**

```conf
server {
    listen 8080;
    server_name monserveur;
    root /var/www/html;
    index index.html;
    error_page 404 /errors/404.html;
}
```

---

## Traitement des Requêtes

1. **Connexion Client** : Acceptation de la connexion.
2. **Analyse de la Requête** : Lecture et traitement des entêtes HTTP.
3. **Routage** : Correspondance de l'URL avec les routes configurées.
4. **Génération de la Réponse** :
   - Envoi d'un fichier statique.
   - Exécution d'un script CGI.
   - Gestion des codes d'erreur.
5. **Envoi de la Réponse** : Transmission au client.
6. **Fermeture de la Connexion**.

---

## Méthodes HTTP Prises en Charge

### `GET`

- **Usage** : Récupération d'une ressource.

### `POST`

- **Usage** : Envoi de données au serveur.

### `DELETE`

- **Usage** : Suppression d'une ressource.

---

# Webserv 🇰🇷

## 목차

- [소개](#소개)
- [기능](#기능)
- [사용법](#사용법)
- [설정](#설정)
- [요청 처리](#요청-처리)
- [오류 처리](#오류-처리)
- [지원되는 HTTP 메소드](#지원되는-http-메소드)

---

## 소개

**Webserv**는 Ecole 42 프로젝트로, C++을 사용하여 간단한 HTTP 서버를 구축하는 과제입니다. Nginx에서 영감을 받아 클라이언트 요청을 처리하고 정적 파일을 제공하며 CGI를 통해 동적 콘텐츠 처리를 지원합니다.

---

## 기능

- **다중 클라이언트 처리**: 비동기 소켓과 `poll()`을 사용하여 다중 요청을 처리합니다.
- **HTTP/1.1 지원**: 요청 파싱, 응답 처리 및 상태 코드 구현.
- **구성 가능 서버**: 설정 파일을 통해 포트, 루트 디렉토리, 오류 페이지 등을 정의할 수 있습니다.
- **정적 파일 제공**: HTML, CSS, JavaScript 및 기타 정적 리소스를 제공합니다.
- **CGI 지원**: PHP 및 Python 스크립트 실행 가능.
- **오류 관리**: 올바르지 않은 요청이나 누락된 리소스에 대해 적절한 HTTP 상태 코드를 반환합니다.

---

## 설정

**예제 설정 파일:**

```conf
server {
    listen 8080;
    server_name myserver;
    root /var/www/html;
    index index.html;
    error_page 404 /errors/404.html;
}
```

---

## 요청 처리

1. **클라이언트 연결**
2. **요청 파싱**
3. **라우팅 처리**
4. **응답 생성**
5. **응답 전송**
6. **연결 종료**

---

## 지원되는 HTTP 메소드

### `GET`

- **사용법**: 리소스 요청.

### `POST`

- **사용법**: 서버로 데이터 전송.

### `DELETE`

- **사용법**: 리소스 삭제 요청.

---

이 README는 **Webserv**의 기능과 사용법을 안내합니다.



