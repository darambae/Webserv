<a id="summary"></a>
<hr>
<details><summary>**Table of Contents**</summary>
<br>

- [EXECVE](#EXECVE)
- [DUP](#DUP)
- [DUP2](#DUP2)
- [PIPE](#PIPE)
- [STRERROR](#STRERROR)
- [GAI_STRERROR](#GAI_STRERROR)
- [FORK](#FORK)
- [SOCKETPAIR](#SOCKETPAIR)
- [HTONS](#HTONS)
- [HTONL](#HTONL)
- [NTOHS](#NTOHS)
- [NTOHL](#NTOHL)
- [SELECT](#SELECT)
- [POLL](#POLL)
- [EPOLL_CREATE](#EPOLL_CREATE)
- [EPOLL_CTL](#EPOLL_CTL)
- [EPOLL_WAIT](#EPOLL_WAIT)
- [KQUEUE](#KQUEUE)
- [SOCKET](#SOCKET)
- [ACCEPT](#ACCEPT)
- [LISTEN](#LISTEN)
- [SEND](#SEND)
- [RECV](#RECV)
- [CHDIR](#CHDIR)
- [BIND](#BIND)
- [CONNECT](#CONNECT)
- [GETADDRINFO](#GETADDRINFO)
- [FREEADDRINFO](#FREEADDRINFO)
- [SETSOCKOPT](#SETSOCKOPT)
- [GETSOCKNAME](#GETSOCKNAME)
- [GETPROTOBYNAME](#GETPROTOBYNAME)
- [FCNTL](#FCNTL)
- [CLOSE](#CLOSE)
- [WRITE](#WRITE)
- [WAITPID](#WAITPID)
- [KILL](#KILL)
- [SIGNAL](#SIGNAL)
- [ACCESS](#ACCESS)
- [STAT](#STAT)
- [OPEN](#OPEN)
- [OPENDIR](#OPENDIR)
- [READDIR](#READDIR)
- [CLOSEDIR](#CLOSEDIR)

</details>
<hr>


## EXECVE

The `execve` function is a standard C function that replaces the current process image with a new process image.

- ***Function Description***: `execve` replaces the current process image with a new process image specified by filename. The `argv` is an array of argument strings passed to the new program. The `envp` is an array of strings, conventionally of the form `key=value`, which are passed as environment to the new program. It does not return on success, and -1 on error.

- ***Prototype***: `int execve(const char *filename, char *const argv[], char *const envp[]);`

- ***Library***:
```c
#include <unistd.h>
```

Example:

```c
#include <stdio.h>
#include <unistd.h>

int main() {
	char *argv[] = {"/bin/ls", "-l", NULL};
	char *envp[] = {NULL};
	if (execve("/bin/ls", argv, envp) == -1) {
		printf("Failed to execute /bin/ls.\n");
	}
	return 0;  // This line will not be executed if execve succeeds
}
```

*In this example, the program replaces its own process image with the `/bin/ls` program using `execve`. If `execve` fails, it prints "Failed to execute /bin/ls.".*
*If `execve` succeeds, the rest of the program will not be executed because the process image has been replaced.*


[Back to Summary](#summary)

---------------------------------------------------------------------------------
## DUP

The `dup` function is a standard C function that creates a copy of the file descriptor.

- ***Function Description***: `dup` creates a new file descriptor that is a copy of the old file descriptor. The new file descriptor is the lowest-numbered unused descriptor. It returns the new file descriptor on success, and -1 on error.

- ***Prototype***: `int dup(int oldfd);`

- ***Library***:
```c
#include <unistd.h>
```

Example:

```c
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
	int fd = open("/tmp/file", O_RDONLY);
	if (fd != -1) {
		int newfd = dup(fd);
		if (newfd != -1) {
			printf("Duplicated file descriptor: %d\n", newfd);
		} else {
			printf("Failed to duplicate file descriptor.\n");
		}
		close(fd);
	} else {
		printf("Failed to open /tmp/file.\n");
	}
	return 0;
}
```

*In this example, the program opens the file `/tmp/file` and duplicates its file descriptor using `dup`, storing the new file descriptor in `newfd`. If `dup` succeeds, it prints "Duplicated file descriptor: " followed by the new file descriptor.*
*If `dup` fails, it prints "Failed to duplicate file descriptor.". If the file fails to open, it prints "Failed to open /tmp/file.".*


[Back to Summary](#summary)

---------------------------------------------------------------------------------
## DUP2

The `dup2` function is a standard C function that duplicates a file descriptor to a specified file descriptor.

- ***Function Description***: `dup2` duplicates the file descriptor oldfd to newfd. If `newfd` was previously open, it is silently closed before being reused. If `oldfd` is not a valid file descriptor, then the call fails, and `newfd` is not closed. It returns the new file descriptor on success, and -1 on error.

- ***Prototype***: `int dup2(int oldfd, int newfd);`

- ***Library***:
```c
#include <unistd.h>
```

Example:

```c
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
	int fd = open("/tmp/file", O_RDONLY);
	if (fd != -1) {
		int newfd = dup2(fd, 10);
		if (newfd != -1) {
			printf("Duplicated file descriptor to: %d\n", newfd);
		} else {
			printf("Failed to duplicate file descriptor.\n");
		}
		close(fd);
	} else {
		printf("Failed to open /tmp/file.\n");
	}
	return 0;
}
```

*In this example, the program opens the file `/tmp/file` and duplicates its file descriptor to descriptor 10 using `dup2`, storing the new file descriptor in `newfd`. If `dup2` succeeds, it prints "Duplicated file descriptor to: " followed by the new file descriptor. If `dup2` fails, it prints "Failed to duplicate file descriptor.".*
*If the file fails to open, it prints "Failed to open /tmp/file.".*


[Back to Summary](#summary)

---------------------------------------------------------------------------------
## PIPE

The `pipe` function is a standard C function that creates a pipe, which is a unidirectional data channel that can be used for interprocess communication.

- ***Function Description***: `pipe` creates a pipe and places two file descriptors, one for reading and one for writing, into the array pointed to by `pipefd`. `pipefd[0]` is for reading, `pipefd[1]` is for writing. It returns 0 on success, and -1 on error.

- ***Prototype***: `int pipe(int pipefd[2]);`

- ***Library***:
```c
#include <unistd.h>
```

Example:

```c
#include <stdio.h>
#include <unistd.h>

int main() {
	int pipefd[2];
	if (pipe(pipefd) == 0) {
		printf("Created a pipe. Read fd: %d, Write fd: %d\n", pipefd[0], pipefd[1]);
	} else {
		printf("Failed to create a pipe.\n");
	}
	return 0;
}
```

*In this example, the program creates a pipe using `pipe` and stores the file descriptors in `pipefd`. If `pipe` succeeds, it prints "Created a pipe. Read fd: ", followed by the read file descriptor, ", Write fd: ", followed by the write file descriptor.*
*If `pipe` fails, it prints "Failed to create a pipe.".*


[Back to Summary](#summary)

---------------------------------------------------------------------------------

## STRERROR

The `strerror` function is a standard C function that returns a string describing the error code passed in the argument errno.

- ***Function Description***: `strerror` returns a pointer to a string that describes the error code passed in the argument errnum. The string is actually a static buffer that is overwritten by each call to `strerror`.

- ***Prototype***: `char *strerror(int errnum);`

- ***Library***:
```c
#include <string.h>
```

Example:

```c
#include <stdio.h>
#include <string.h>
#include <errno.h>

int main() {
	FILE *file = fopen("/nonexistent", "r");
	if (!file) {
		printf("Error: %s\n", strerror(errno));
	}
	return 0;
}
```

*In this example, the program tries to open a non-existent file. When the `fopen` call fails, it prints the error message associated with the current `errno` value using `strerror`.*


[Back to Summary](#summary)

---------------------------------------------------------------------------------


## GAI_STRERROR

The `gai_strerror` function is used to translate an error code returned by `getaddrinfo` into a human-readable string describing the error.

- ***Function Description***: `gai_strerror` returns a string describing the error code returned by `getaddrinfo`, which is useful for debugging network address translation issues.

- ***Prototype***: `const char *gai_strerror(int errcode);`

- ***Library***:

```c
#include <netdb.h>
```

Example:

```c
#include <stdio.h>
#include <netdb.h>

int main() {
    int errcode = EAI_NONAME;  // Example error code
    const char *errstr = gai_strerror(errcode);
    
    printf("Error message: %s\n", errstr);
    return 0;
}
```

*In this example, the program uses an error code (EAI_NONAME) returned by getaddrinfo and passes it to gai_strerror to get a human-readable error message, which is then printed to the console.*

[Back to Summary](#summary)
---------------------------------------------------------------------------------

## FORK

The `fork` function is a standard C function that creates a new process.

- ***Function Description***: `fork` creates a new process by duplicating the existing process. The new process, called the child, is an exact copy of the calling process, called the parent, except for a few values changed, including the process ID and parent process ID. It returns the process ID of the child process to the parent, and 0 to the child. If it fails, it returns -1.

- ***Prototype***: `pid_t fork(void);`

- ***Library***:
```c
#include <unistd.h>
```

Example:

```c

#include <stdio.h>
#include <unistd.h>

int main() {
	pid_t pid = fork();

	if (pid < 0) {
		printf("Failed to fork.\n");
	} else if (pid == 0) {
		printf("This is the child process.\n");
	} else {
		printf("This is the parent process, child PID is %d.\n", pid);
	}
	return 0;
}
```

*In this example, the program creates a new process using `fork`. If `fork` fails, it prints "Failed to fork.". If `fork` succeeds, the parent process prints its own process ID and the child's process ID, and the child process prints "This is the child process.".*


[Back to Summary](#summary)

---------------------------------------------------------------------------------

## SOCKETPAIR

The `socketpair` function is used to create a pair of connected sockets, which can be used for inter-process communication (IPC).

- ***Function Description***: `socketpair` creates two connected sockets, usually in the UNIX domain, which can be used to send and receive data between two processes or threads.

- ***Prototype***: `int socketpair(int domain, int type, int protocol, int sv[2]);`

- ***Library***:

```cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
```

``` cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

int main() {
    int sv[2];  // Array to hold the pair of sockets

    // Create the socket pair
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1) {
        perror("socketpair");
        return 1;
    }

    // Write a message to the first socket
    const char* message = "Hello, world!";
    write(sv[0], message, 13);

    // Buffer to hold the message from the second socket
    char buf[100] = {0};
    read(sv[1], buf, sizeof(buf));

    // Output the message received from the second socket
    std::cout << "Received message: " << buf << std::endl;

    return 0;
}

```

*In this C++ example, `socketpair` creates two connected sockets (sv[0] and sv[1]). A message is written to the first socket (sv[0]), and the message is read from the second socket (sv[1]). The program then prints the received message using `std::cout` to demonstrate IPC between the two sockets.*


[Back to Summary](#summary)
---------------------------------------------------------------------------------

## HTONS

The `htons` function is used to convert a 16-bit number from host byte order to network byte order.

- ***Function Description***: `htons` (host to network short) converts a 16-bit integer (short) from the host machine's byte order (which may be little-endian or big-endian) to network byte order (big-endian), which is required for network communication.

- ***Prototype***: `uint16_t htons(uint16_t hostshort);`

- ***Library***:

```cpp
#include <arpa/inet.h>
#include <iostream>
```

```cpp
#include <arpa/inet.h>
#include <iostream>

int main() {
    uint16_t host_port = 8080;  // Example port number in host byte order
    uint16_t network_port = htons(host_port);  // Convert to network byte order

    std::cout << "Host order: " << host_port << std::endl;
    std::cout << "Network order: " << network_port << std::endl;

    return 0;
}

```

*In this C++ example, `htons` is used to convert the port number 8080 from host byte order to network byte order. The original host order and the converted network order are then printed using `std::cout` to illustrate the byte order conversion.*

[Back to Summary](#summary)
---------------------------------------------------------------------------------

## HTONL

The `htonl` function is used to convert a 32-bit number from host byte order to network byte order.

- ***Function Description***: `htonl` (host to network long) converts a 32-bit integer from the host machine's byte order (which may be little-endian or big-endian) to network byte order (big-endian), required for sending data over the network.

- ***Prototype***: `uint32_t htonl(uint32_t hostlong);`

- ***Library***:

```cpp
#include <arpa/inet.h>
#include <iostream>
```

```cpp
#include <arpa/inet.h>
#include <iostream>

int main() {
    uint32_t host_address = 0xC0A80001;  // Example IP address in host byte order (192.168.0.1)
    uint32_t network_address = htonl(host_address);  // Convert to network byte order

    std::cout << "Host order: " << std::hex << host_address << std::endl;
    std::cout << "Network order: " << std::hex << network_address << std::endl;

    return 0;
}

```

*In this C++ example, `htonl` converts the IP address 192.168.0.1 (represented as 0xC0A80001 in hex) from host byte order to network byte order. The original host and network order values are printed using `std::cout` in hexadecimal format to illustrate the conversion.*

[Back to Summary](#summary)
---------------------------------------------------------------------------------

## NTOHS

The `ntohs` function is used to convert a 16-bit number from network byte order to host byte order.

- ***Function Description***: `ntohs` (network to host short) converts a 16-bit integer from network byte order (big-endian) to the host machine's byte order (which may be little-endian or big-endian), allowing for proper interpretation of data received over the network.

- ***Prototype***: `uint16_t ntohs(uint16_t netshort);`

- ***Library***:

```cpp
#include <arpa/inet.h>
#include <iostream>
```

```cpp
#include <arpa/inet.h>
#include <iostream>

int main() {
    uint16_t network_port = 0x1F90;  // Example port number in network byte order (8080 in decimal)
    uint16_t host_port = ntohs(network_port);  // Convert to host byte order

    std::cout << "Network order: " << network_port << std::endl;
    std::cout << "Host order: " << host_port << std::endl;

    return 0;
}

```

*In this C++ example, `ntohs` is used to convert the network byte order port number 0x1F90 (which is 8080 in decimal) to host byte order. The original network order and the converted host order values are printed using `std::cout` to demonstrate the byte order conversion.*

[Back to Summary](#summary)
---------------------------------------------------------------------------------

## NTOHL

The `ntohl` function is used to convert a 32-bit number from network byte order to host byte order.

- ***Function Description***: `ntohl` (network to host long) converts a 32-bit integer from network byte order (big-endian) to the host machine's byte order (which may be little-endian or big-endian), allowing for proper interpretation of data received over the network.

- ***Prototype***: `uint32_t ntohl(uint32_t netlong);`

- ***Library***:

```cpp
#include <arpa/inet.h>
#include <iostream>
```

```cpp
#include <arpa/inet.h>
#include <iostream>

int main() {
    uint32_t network_address = 0xC0A80001;  // Example IP address in network byte order (192.168.0.1)
    uint32_t host_address = ntohl(network_address);  // Convert to host byte order

    std::cout << "Network order: " << std::hex << network_address << std::endl;
    std::cout << "Host order: " << std::hex << host_address << std::endl;

    return 0;
}

```

*In this C++ example, `ntohl` is used to convert the IP address `0xC0A80001` (which corresponds to 192.168.0.1 in decimal) from network byte order to host byte order. The original network and converted host order values are printed using `std::cout` in hexadecimal format to illustrate the conversion.*

[Back to Summary](#summary)
---------------------------------------------------------------------------------

## SELECT

The `select` function is used to monitor multiple file descriptors to see if they are ready for reading, writing, or if an error has occurred.

- ***Function Description***: `select` allows a program to monitor multiple file descriptors, waiting until one or more of the file descriptors become "ready" for some class of I/O operation (e.g., reading or writing). This is useful for implementing multiplexing I/O.

- ***Prototype***: `int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);`

- ***Library***:

```cpp
#include <sys/select.h>
#include <unistd.h>
#include <iostream>
```


```cpp
#include <sys/select.h>
#include <unistd.h>
#include <iostream>

int main() {
    fd_set readfds;  // File descriptor set for reading
    int fd = 0;      // File descriptor for standard input (stdin)

    // Clear the file descriptor set
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);

    // Set timeout to 5 seconds
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    // Wait for input on stdin
    int result = select(fd + 1, &readfds, nullptr, nullptr, &timeout);

    if (result == -1) {
        perror("select");
        return 1;
    } else if (result == 0) {
        std::cout << "Timeout occurred! No data after 5 seconds." << std::endl;
    } else {
        std::cout << "Data is available to read from stdin." << std::endl;
    }

    return 0;
}

```

*In this C++ example, `select` is used to monitor the standard input (`stdin`). The program sets a timeout of 5 seconds and checks if there is data available to read. If data becomes available within the timeout, it prints a message indicating that data is ready; otherwise, it reports a timeout.*


[Back to Summary](#summary)
---------------------------------------------------------------------------------

## POLL

The `poll` function is used to monitor multiple file descriptors to see if they are ready for reading, writing, or if an error has occurred, similar to `select` but more scalable for larger numbers of file descriptors.

- ***Function Description***: `poll` waits for one of a set of file descriptors to become ready for some kind of I/O operation (e.g., reading or writing). It's more flexible and scalable than `select` when dealing with a large number of file descriptors.

- ***Prototype***: `int poll(struct pollfd fds[], nfds_t nfds, int timeout);`

- ***Library***:

```cpp
#include <poll.h>
#include <unistd.h>
#include <iostream>

```


```cpp
#include <poll.h>
#include <unistd.h>
#include <iostream>

int main() {
    struct pollfd fds[1];  // Array to hold file descriptors
    fds[0].fd = 0;         // Standard input (stdin)
    fds[0].events = POLLIN;  // Monitor for input availability

    int timeout = 5000;  // Set timeout to 5000 milliseconds (5 seconds)

    // Poll stdin for input with a 5-second timeout
    int result = poll(fds, 1, timeout);

    if (result == -1) {
        perror("poll");
        return 1;
    } else if (result == 0) {
        std::cout << "Timeout occurred! No data after 5 seconds." << std::endl;
    } else {
        if (fds[0].revents & POLLIN) {
            std::cout << "Data is available to read from stdin." << std::endl;
        }
    }

    return 0;
}

```

*In this C++ example, `poll` is used to monitor the standard input (stdin). The program sets a timeout of 5 seconds, and if data becomes available within that time, it prints a message indicating that data is ready to be read. Otherwise, it reports a timeout.*

[Back to Summary](#summary)
---------------------------------------------------------------------------------
## EPOLL_CREATE

The `epoll_create` function is used to create an epoll instance, which can monitor multiple file descriptors for events like readability, writability, or errors. It's more efficient than `poll` or `select` for large numbers of file descriptors.

- ***Function Description***: `epoll_create` creates an epoll instance, which can then be used to monitor a large number of file descriptors for I/O events. It returns a file descriptor for the new epoll instance.

- ***Prototype***: `int epoll_create(int size);`  
  *(Note: `epoll_create1` is the modern replacement, which includes flags)*

- ***Library***:

```cpp
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>

```


```cpp
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>

int main() {
    // Create an epoll instance
    int epoll_fd = epoll_create(1);  // '1' is ignored but must be > 0

    if (epoll_fd == -1) {
        perror("epoll_create");
        return 1;
    }

    std::cout << "Epoll instance created successfully with fd: " << epoll_fd << std::endl;

    // Close the epoll instance
    close(epoll_fd);

    return 0;
}

```

*In this C++ example, `epoll_create` is used to create an `epoll` instance. The file descriptor for the `epoll` instance is printed using `std::cout`, and the `epoll` instance is closed afterward using close. Although the size argument is ignored in modern implementations, it must still be greater than 0.*

[Back to Summary](#summary)
---------------------------------------------------------------------------------
## EPOLL_CTL

The `epoll_ctl` function is used to control an epoll instance by adding, modifying, or removing file descriptors to be monitored for specific events.

- ***Function Description***: `epoll_ctl` performs control operations on the file descriptors being monitored by an epoll instance. You can add, modify, or remove file descriptors with this function using various operations like `EPOLL_CTL_ADD`, `EPOLL_CTL_MOD`, and `EPOLL_CTL_DEL`.

- ***Prototype***: `int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);`

- ***Library***:

```cpp
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>
```


```cpp
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>

int main() {
    // Create an epoll instance
    int epoll_fd = epoll_create(1);
    if (epoll_fd == -1) {
        perror("epoll_create");
        return 1;
    }

    // Set up the file descriptor to monitor (in this case, standard input - stdin)
    int fd = 0;  // Standard input (fd = 0)

    struct epoll_event event;
    event.events = EPOLLIN;  // Monitor for input events
    event.data.fd = fd;      // Associate the file descriptor with the event

    // Add stdin to the epoll instance for monitoring input
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1) {
        perror("epoll_ctl");
        close(epoll_fd);
        return 1;
    }

    std::cout << "File descriptor added to epoll instance for monitoring input." << std::endl;

    // Clean up
    close(epoll_fd);

    return 0;
}
```

*In this C++ example, `epoll_ctl` is used to add the standard input (stdin) file descriptor to an `epoll` instance for monitoring input events. The `EPOLL_CTL_ADD` operation is used to add the file descriptor, and `EPOLLIN` is specified as the event to monitor (indicating that input is available). The file descriptor is then added to the `epoll` instance, and afterward, the `epoll` instance is closed.*

[Back to Summary](#summary)
---------------------------------------------------------------------------------
## EPOLL_WAIT

The `epoll_wait` function waits for events on the file descriptors associated with an epoll instance. It blocks until one or more file descriptors are ready for the requested I/O operation or until the timeout expires.

- ***Function Description***: `epoll_wait` waits for I/O events on file descriptors monitored by an epoll instance. It returns when one or more file descriptors have events ready, or it times out if no events occur.

- ***Prototype***: `int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);`

- ***Library***:

```cpp
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>
```


```cpp
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>

int main() {
    // Create an epoll instance
    int epoll_fd = epoll_create(1);
    if (epoll_fd == -1) {
        perror("epoll_create");
        return 1;
    }

    // Set up the file descriptor to monitor (standard input - stdin)
    int fd = 0;  // Standard input

    struct epoll_event event;
    event.events = EPOLLIN;  // Monitor for input events
    event.data.fd = fd;      // Associate the file descriptor with the event

    // Add stdin to the epoll instance
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1) {
        perror("epoll_ctl");
        close(epoll_fd);
        return 1;
    }

    // Wait for events on the monitored file descriptors
    struct epoll_event events[1];  // Array to hold the events
    int timeout = 10000;  // Timeout of 10 seconds

    int nfds = epoll_wait(epoll_fd, events, 1, timeout);
    if (nfds == -1) {
        perror("epoll_wait");
        close(epoll_fd);
        return 1;
    } else if (nfds == 0) {
        std::cout << "Timeout occurred! No events after 10 seconds." << std::endl;
    } else {
        std::cout << "Data is available to read from stdin." << std::endl;
    }

    // Clean up
    close(epoll_fd);

    return 0;
}
```

*In this C++ example, `epoll_wait` is used to wait for events on the standard input (stdin) file descriptor monitored by an `epoll` instance. The function waits for up to 10 seconds. If an input event occurs, the program prints a message indicating data is available; otherwise, it reports a timeout.*

[Back to Summary](#summary)
---------------------------------------------------------------------------------
## KQUEUE

The `kqueue` function is used to create a kernel event queue, which is a scalable mechanism for monitoring multiple file descriptors or other kernel events (like timers, signals, etc.) on systems like FreeBSD, macOS, and OpenBSD.

- ***Function Description***: `kqueue` creates a new kernel event queue and returns a file descriptor that can be used to monitor events on file descriptors or other kernel events such as timers or signals.

- ***Prototype***: `int kqueue(void);`

- ***Library***:

```cpp
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
```


```cpp
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>

int main() {
    // Create a new kqueue
    int kq = kqueue();
    if (kq == -1) {
        perror("kqueue");
        return 1;
    }

    std::cout << "Kqueue created with fd: " << kq << std::endl;

    // We could set up events here (like monitoring file descriptors), but for this basic example, we just close it.
    close(kq);

    return 0;
}

```

*In this C++ example, `kqueue` is used to create a new kernel event queue. The program prints the file descriptor associated with the new `kqueue`. Although no specific events are monitored in this simple example, `kqueue` can be used to watch for events on file descriptors, signals, or timers. After creating the `kqueue`, the program closes it.*

[Back to Summary](#summary)
---------------------------------------------------------------------------------
## SOCKET

The `socket` function is used to create a socket, which is an endpoint for communication between two machines or processes over a network.

- ***Function Description***: `socket` creates a new socket with the specified domain, type, and protocol. The socket can then be used to communicate with other sockets over a network using the chosen protocol (such as TCP or UDP).

- ***Prototype***: `int socket(int domain, int type, int protocol);`

- ***Library***:

```cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
```


```cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

int main() {
    // Create a TCP socket (IPv4)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return 1;
    }

    std::cout << "Socket created with file descriptor: " << sockfd << std::endl;

    // Close the socket after use
    close(sockfd);

    return 0;
}
```

*In this C++ example, `socket` is used to create a new TCP `socket` using IPv4 (AF_INET) and the stream socket type (SOCK_STREAM). The program prints the file descriptor for the `socket` and then closes the `socket` after it is no longer needed.*

[Back to Summary](#summary)
---------------------------------------------------------------------------------
## ACCEPT

The `accept` function is used in a server application to accept a connection request from a client. It creates a new socket for communication with the client.

- ***Function Description***: `accept` extracts the first connection request from the queue of pending connections for a listening socket. It creates a new socket for the connection and returns the file descriptor for that socket.

- ***Prototype***: `int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);`

- ***Library***:

```cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <netinet/in.h>
#include <cstring>
```


```cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <netinet/in.h>
#include <cstring>

int main() {
    // Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return 1;
    }

    // Set up the server address structure
    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    // Bind the socket to the address and port
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(sockfd);
        return 1;
    }

    // Listen for incoming connections
    if (listen(sockfd, 10) == -1) {
        perror("listen");
        close(sockfd);
        return 1;
    }

    std::cout << "Waiting for a connection..." << std::endl;

    // Accept an incoming connection
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd == -1) {
        perror("accept");
        close(sockfd);
        return 1;
    }

    std::cout << "Connection accepted!" << std::endl;

    // Close the sockets
    close(client_fd);
    close(sockfd);

    return 0;
}

```

*In this C++ example, a TCP socket is created and bound to port 8080. The server listens for incoming connections, and accept is used to accept a connection request from a client. The server prints a message when a connection is accepted and closes the sockets afterward.*

[Back to Summary](#summary)
---------------------------------------------------------------------------------
## LISTEN

The `listen` function is used to mark a socket as a passive socket that will accept incoming connection requests. It is typically used in server applications after binding a socket to a local address.

- ***Function Description***: `listen` makes a socket ready to accept incoming connection requests. It defines a backlog, which is the maximum number of pending connections that can be queued for acceptance.

- ***Prototype***: `int listen(int sockfd, int backlog);`

- ***Library***:

```cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <netinet/in.h>
#include <cstring>
```


```cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <netinet/in.h>
#include <cstring>

int main() {
    // Create a TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return 1;
    }

    // Set up the server address structure
    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    // Bind the socket to the address and port
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(sockfd);
        return 1;
    }

    // Listen for incoming connections with a backlog of 10
    if (listen(sockfd, 10) == -1) {
        perror("listen");
        close(sockfd);
        return 1;
    }

    std::cout << "Listening for incoming connections on port 8080..." << std::endl;

    // Cleanup (in a real server, accept would be called here)
    close(sockfd);

    return 0;
}
```

*In this C++ example, a TCP socket is created and bound to port 8080. The listen function is called to prepare the socket for incoming connections, with a backlog of 10, meaning the server can queue up to 10 pending connection requests. After that, the program waits for connections (though this example doesn't include the accept step). The socket is then closed.*

[Back to Summary](#summary)
---------------------------------------------------------------------------------
## SEND

The `send` function is used to transmit data over a connected socket. It sends data to another socket, typically used in TCP connections.

- ***Function Description***: `send` sends data on a socket that is connected to a remote host. The function allows specifying flags to control the behavior of the operation, such as blocking or non-blocking mode.

- ***Prototype***: `ssize_t send(int sockfd, const void *buf, size_t len, int flags);`

- ***Library***:

```cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
```


```cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <netinet/in.h>
#include <cstring>

int main() {
    // Create a TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return 1;
    }

    // Set up the server address structure
    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Localhost
    server_addr.sin_port = htons(8080);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(sockfd);
        return 1;
    }

    // Data to be sent
    const char *msg = "Hello, Server!";
    ssize_t bytes_sent = send(sockfd, msg, strlen(msg), 0);
    if (bytes_sent == -1) {
        perror("send");
    } else {
        std::cout << "Sent " << bytes_sent << " bytes to the server." << std::endl;
    }

    // Close the socket
    close(sockfd);

    return 0;
}

```

*In this C++ example, a TCP socket is created and connected to a server running on localhost at port 8080. The send function is used to send the message "Hello, Server!" to the server. The number of bytes sent is printed, and the socket is closed after the transmission is complete.*

[Back to Summary](#summary)
---------------------------------------------------------------------------------
## RECV

The `recv` function is used to receive data from a connected socket. It reads incoming data from a socket, typically used in TCP connections.

- ***Function Description***: `recv` receives data from a socket, storing it into the specified buffer. It can be used in both blocking and non-blocking modes, and flags can control the behavior of the operation.

- ***Prototype***: `ssize_t recv(int sockfd, void *buf, size_t len, int flags);`

- ***Library***:

```cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
```


```cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <netinet/in.h>
#include <cstring>

int main() {
    // Create a TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return 1;
    }

    // Set up the server address structure
    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Localhost
    server_addr.sin_port = htons(8080);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(sockfd);
        return 1;
    }

    // Buffer to receive the message
    char buffer[1024];
    ssize_t bytes_received = recv(sockfd, buffer, sizeof(buffer), 0);
    if (bytes_received == -1) {
        perror("recv");
    } else {
        buffer[bytes_received] = '\0';  // Null-terminate the received string
        std::cout << "Received message: " << buffer << std::endl;
    }

    // Close the socket
    close(sockfd);

    return 0;
}

```

*In this C++ example, a TCP socket is created and connected to a server running on localhost at port 8080. The `recv` function is used to receive data from the server and store it in a buffer. After receiving the message, it prints the message and closes the socket.*

[Back to Summary](#summary)
---------------------------------------------------------------------------------

## CHDIR

The `chdir` function is a standard C function that changes the current working directory of the process.

- ***Function Description***: `chdir` changes the current working directory of the process to the directory specified by path. It returns 0 on success, and -1 on error.

- ***Prototype***: `int chdir(const char *path);`

- ***Library***:
```c
#include <unistd.h>
```

Example:

```c
#include <stdio.h>
#include <unistd.h>

int main() {
	if (chdir("/tmp") == 0) {
		printf("Changed the current working directory to /tmp\n");
	} else {
		printf("Failed to change the current working directory.\n");
	}
	return 0;
}
```

*In this example, the program changes the current working directory to `/tmp` using `chdir`. If `chdir` succeeds, it prints "Changed the current working directory to /tmp".*
*If `chdir` fails, it prints "Failed to change the current working directory.".*

[Back to Summary](#summary)
---------------------------------------------------------------------------------
## BIND

The `bind` function is used to associate a socket with a specific local address and port number. This is typically used in server applications to specify where the server should listen for incoming connections.

- ***Function Description***: `bind` assigns a local address and port to a socket. After binding, the socket can listen for incoming connections on that address and port.

- ***Prototype***: `int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);`

- ***Library***:

```cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <netinet/in.h>
#include <cstring>
```


```cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <netinet/in.h>
#include <cstring>

int main() {
    // Create a TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return 1;
    }

    // Set up the server address structure
    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any address
    server_addr.sin_port = htons(8080);          // Port number

    // Bind the socket to the address and port
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(sockfd);
        return 1;
    }

    std::cout << "Socket successfully bound to port 8080." << std::endl;

    // Cleanup (in a real server, listen and accept would be called here)
    close(sockfd);

    return 0;
}

```

*In this C++ example, a TCP socket is created, and the `bind` function is used to associate it with port 8080 on any available network interface (INADDR_ANY). After binding, a message is printed confirming the operation, and the socket is closed. In a complete server application, the next steps would typically involve listening for and accepting connections.*

[Back to Summary](#summary)
---------------------------------------------------------------------------------
## CONNECT

The `connect` function is used to establish a connection to a specified server using a socket. It is typically used in client applications to connect to a server.

- ***Function Description***: `connect` initiates a connection on a socket to a specified address and port. This function is essential for establishing a client-server communication link.

- ***Prototype***: `int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);`

- ***Library***:

```cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <netinet/in.h>
#include <cstring>
```


```cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <netinet/in.h>
#include <cstring>

int main() {
    // Create a TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return 1;
    }

    // Set up the server address structure
    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Localhost
    server_addr.sin_port = htons(8080); // Port number

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(sockfd);
        return 1;
    }

    std::cout << "Connected to the server on port 8080." << std::endl;

    // Close the socket
    close(sockfd);

    return 0;
}

```

*In this C++ example, a TCP socket is created and configured to connect to a server running on localhost at port 8080. The connect function is called to establish the connection. If the connection is successful, a message is printed to confirm it, and the socket is closed afterward. This code is typically found in a client application that needs to communicate with a server.*

[Back to Summary](#summary)
---------------------------------------------------------------------------------
## GETADDRINFO

The `getaddrinfo` function is used to resolve hostnames and service names into a list of address structures that can be used for socket connections. It provides a convenient way to get both IPv4 and IPv6 addresses.

- ***Function Description***: `getaddrinfo` performs DNS resolution and retrieves address information based on a specified hostname and service name. It handles both IPv4 and IPv6 and supports multiple address types and socket types.

- ***Prototype***: `int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);`

- ***Library***:

```cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <cstring>
```


```cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <cstring>

int main() {
    struct addrinfo hints, *res;
    int status;

    // Set up the hints structure
    std::memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP

    // Resolve the hostname and service
    const char *hostname = "localhost";
    const char *service = "8080"; // Port number
    status = getaddrinfo(hostname, service, &hints, &res);
    if (status != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        return 1;
    }

    // Print resolved addresses
    for (struct addrinfo *p = res; p != nullptr; p = p->ai_next) {
        std::cout << "Address family: " << p->ai_family << std::endl;
        std::cout << "Socket type: " << p->ai_socktype << std::endl;
    }

    // Free the memory allocated by getaddrinfo
    freeaddrinfo(res);

    return 0;
}
```

*In this C++ example, the `getaddrinfo` function is used to resolve the hostname "localhost" and the service "8080". The hints structure specifies that both IPv4 and IPv6 addresses are acceptable. If successful, it prints the address family and socket type for each resolved address. Finally, the memory allocated by `getaddrinfo` is freed. This function is commonly used in client applications to set up a socket for communication.*

[Back to Summary](#summary)
---------------------------------------------------------------------------------
## FREEADDRINFO

The `freeaddrinfo` function is used to free the memory allocated for the address information returned by `getaddrinfo`. It is essential for preventing memory leaks in applications that use this function.

- ***Function Description***: `freeaddrinfo` releases the memory associated with the linked list of `addrinfo` structures that was allocated by `getaddrinfo`.

- ***Prototype***: `void freeaddrinfo(struct addrinfo *res);`

- ***Library***:

```cpp
#include <netdb.h>
```


```cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <cstring>

int main() {
    struct addrinfo hints, *res;
    int status;

    // Set up the hints structure
    std::memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP

    // Resolve the hostname and service
    const char *hostname = "localhost";
    const char *service = "8080"; // Port number
    status = getaddrinfo(hostname, service, &hints, &res);
    if (status != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        return 1;
    }

    // Use the resolved addresses (omitted for brevity)

    // Free the memory allocated by getaddrinfo
    freeaddrinfo(res);

    std::cout << "Memory freed successfully." << std::endl;

    return 0;
}
```

*In this C++ example, the `getaddrinfo` function is used to resolve the hostname "localhost" and the service "8080". After using the resolved addresses (though not explicitly shown here), the `freeaddrinfo` function is called to free the allocated memory. This step is crucial in preventing memory leaks in applications that dynamically allocate memory for address information.*

[Back to Summary](#summary)
---------------------------------------------------------------------------------
## SETSOCKOPT

The `setsockopt` function is used to configure socket options for a socket. It allows you to set various parameters that control the behavior of sockets, such as timeouts, buffer sizes, and other socket-level options.

- ***Function Description***: `setsockopt` sets the value of a socket option specified by the level and option name. This function is crucial for tuning socket performance and behavior based on application requirements.

- ***Prototype***: `int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);`

- ***Library***:

```cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
```


```cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <cstring>
#include <unistd.h>

int main() {
    // Create a TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return 1;
    }

    // Set the option to reuse the address
    int optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        perror("setsockopt");
        close(sockfd);
        return 1;
    }

    std::cout << "Socket option SO_REUSEADDR set successfully." << std::endl;

    // Close the socket
    close(sockfd);

    return 0;
}
```

*In this C++ example, a TCP socket is created, and the `setsockopt` function is used to set the SO_REUSEADDR option, which allows the socket to reuse the address. If successful, a message is printed confirming the operation. Finally, the socket is closed. This function is commonly used in server applications to allow multiple sockets to bind to the same address and port.*

[Back to Summary](#summary)
---------------------------------------------------------------------------------
## GETSOCKNAME

The `getsockname` function retrieves the current address to which a socket is bound. This can be useful for servers to find out the local address and port they are using after binding.

- ***Function Description***: `getsockname` fills a `sockaddr` structure with the address information of the specified socket. It is often used to determine the local endpoint of a socket.

- ***Prototype***: `int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);`

- ***Library***:

```cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <cstring>
#include <netinet/in.h>
```


```cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>

int main() {
    // Create a TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return 1;
    }

    // Set up the server address structure
    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any address
    server_addr.sin_port = htons(8080);          // Port number

    // Bind the socket
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(sockfd);
        return 1;
    }

    // Get the address the socket is bound to
    struct sockaddr_in local_addr;
    socklen_t addr_len = sizeof(local_addr);
    if (getsockname(sockfd, (struct sockaddr *)&local_addr, &addr_len) == -1) {
        perror("getsockname");
        close(sockfd);
        return 1;
    }

    // Print the local address and port
    std::cout << "Socket is bound to " 
              << inet_ntoa(local_addr.sin_addr) << ":" 
              << ntohs(local_addr.sin_port) << std::endl;

    // Close the socket
    close(sockfd);

    return 0;
}
```

*In this C++ example, a TCP socket is created and bound to port 8080 on any available interface. The `getsockname` function is then used to retrieve the address the socket is bound to, which is printed to the console. Finally, the socket is closed. This function is particularly useful for confirming that the socket has been bound to the expected address and port.*

[Back to Summary](#summary)
---------------------------------------------------------------------------------
## GETPROTOBYNAME

The `getprotobyname` function retrieves protocol information for a specified protocol name. It is used to obtain protocol number and related details for protocols such as TCP and UDP.

- ***Function Description***: `getprotobyname` returns a pointer to a `protoent` structure containing information about the specified protocol, including its name and number.

- ***Prototype***: `struct protoent *getprotobyname(const char *name);`

- ***Library***:

```cpp
#include <netdb.h>
#include <iostream>
```


```cpp
#include <netdb.h>
#include <iostream>

int main() {
    const char *protocol_name = "tcp"; // Example protocol name
    struct protoent *protocol_info;

    // Retrieve protocol information
    protocol_info = getprotobyname(protocol_name);
    if (protocol_info == nullptr) {
        std::cerr << "No information found for protocol: " << protocol_name << std::endl;
        return 1;
    }

    // Print protocol information
    std::cout << "Protocol Name: " << protocol_info->p_name << std::endl;
    std::cout << "Protocol Number: " << protocol_info->p_proto << std::endl;

    return 0;
}
```

*In this C++ example, the `getprotobyname` function is called with the protocol name "tcp". If successful, it retrieves and prints the protocol's name and number. This function is useful for applications that need to work with specific protocols and requires knowledge of the protocol names used in the network programming context.*

[Back to Summary](#summary)
---------------------------------------------------------------------------------
## FCNTL

The `fcntl` function is used to perform various operations on file descriptors. It can be used to set file descriptor flags, obtain file descriptor information, and manage file locks.

- ***Function Description***: `fcntl` can change the properties of a file descriptor or retrieve information about it. Common uses include setting the file descriptor to non-blocking mode or getting the current flags.

- ***Prototype***: `int fcntl(int fd, int cmd, ... /* arg */);`

- ***Library***:

```cpp
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
```


```cpp
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

int main() {
    // Open a file
    int fd = open("example.txt", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // Get the current flags of the file descriptor
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1) {
        perror("fcntl");
        close(fd);
        return 1;
    }

    // Set the file descriptor to non-blocking mode
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl");
        close(fd);
        return 1;
    }

    std::cout << "File descriptor set to non-blocking mode." << std::endl;

    // Close the file descriptor
    close(fd);

    return 0;
}
```

*In this C++ example, a file named "example.txt" is opened with read and write permissions. The `fcntl` function is then used to retrieve the current flags of the file descriptor, followed by setting the file descriptor to non-blocking mode. If successful, a message is printed confirming the operation. Finally, the file descriptor is closed. This function is commonly used in network programming to manage socket properties and behavior.*

[Back to Summary](#summary)
---------------------------------------------------------------------------------
## CLOSE

The `close` function is a standard C function that closes a file descriptor.

- ***Function Description***: `close` closes a file descriptor, so that it no longer refers to any file and may be reused. It returns zero on success, or -1 if an error occurs.

- ***Prototype***: `int close(int fd);`

- ***Library***:
```c
#include <unistd.h>
```

Example:

```c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
	int fd = open("myfile.txt", O_RDONLY);
	if (fd == -1) {
		printf("Failed to open file.\n");
	} else {
		printf("File opened successfully.\n");
		if (close(fd) == 0) {
			printf("File closed successfully.\n");
		} else {
			printf("Failed to close file.\n");
		}
	}
	return 0;
}
```


*In this example, the program attempts to open the file myfile.txt in read-only mode. If the file is opened successfully, it prints "File opened successfully.". Then it attempts to close the file. If the file is closed successfully, it prints "File closed successfully.".*
*If the file cannot be closed, it prints "Failed to close file.".*

[Back to Summary](#summary)
---------------------------------------------------------------------------------
## WRITE

The `write` function is used to write data to a file descriptor, which can be a file, socket, or any other writable resource. It is a low-level output operation.

- ***Function Description***: `write` transfers data from a buffer to the specified file descriptor, returning the number of bytes successfully written.

- ***Prototype***: `ssize_t write(int fd, const void *buf, size_t count);`

- ***Library***:

```cpp
#include <unistd.h>
#include <iostream>
```


```cpp
#include <unistd.h>
#include <iostream>
#include <fcntl.h>

int main() {
    // Open a file for writing
    int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    const char *message = "Hello, World!\n";
    ssize_t bytes_written = write(fd, message, strlen(message));

    if (bytes_written == -1) {
        perror("write");
        close(fd);
        return 1;
    }

    std::cout << "Written " << bytes_written << " bytes to output.txt." << std::endl;

    // Close the file descriptor
    close(fd);

    return 0;
}
```

*In this C++ example, a file named "output.txt" is opened for writing. The write function is called to write the string "Hello, World!" to the file. If successful, it prints the number of bytes written to the console. Finally, the file descriptor is closed. This function is commonly used for outputting data to files or sockets in low-level programming.*

[Back to Summary](#summary)
---------------------------------------------------------------------------------


## WAITPID

The `waitpid` function is a standard C function that makes a parent process wait for a specific child process to exit.

- ***Function Description***: `waitpid` suspends the calling process until the specified child process exits. It returns the process ID of the child process that exited, 0 if the child is still running, or -1 if an error occurs. The exit status of the child process is stored in the integer pointed to by the status parameter.

- ***Prototype***: `pid_t waitpid(pid_t pid, int *status, int options);`

- ***Library***:
```c
#include <sys/wait.h>
```

Example:

```c

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
	pid_t pid = fork();

	if (pid < 0) {
		printf("Failed to fork.\n");
	} else if (pid == 0) {
		printf("This is the child process.\n");
	} else {
		int status;
		waitpid(pid, &status, 0);
		printf("This is the parent process, child has exited.\n");
	}
	return 0;
}
```

*In this example, the program creates a new process using `fork`. If `fork` fails, it prints "Failed to fork.".*
*If `fork` succeeds, the child process prints "This is the child process.", and the parent process waits for the specific child to exit before printing "This is the parent process, child has exited.".*

[Back to Summary](#summary)
---------------------------------------------------------------------------------

## KILL

The `kill` function is a standard C function that sends a signal to a specific process or a group of processes.

- ***Function Description***: `kill` sends a signal to a process or a group of processes. The pid parameter specifies the process or group to send to, and the sig parameter specifies the signal to send. It returns 0 on success, and -1 on error.

- ***Prototype***: `int kill(pid_t pid, int sig);`

- ***Library***:
```c
#include <signal.h>
```

Example:

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main() {
	printf("Sending SIGSTOP to this process\n");
	kill(getpid(), SIGSTOP);
	return 0;
}
```

*In this example, the program sends the `SIGSTOP` signal to itself using kill. This will cause the process to stop. Note that you can continue the process with the `fg` command if you're running it in a terminal.*


[Back to Summary](#summary)
---------------------------------------------------------------------------------

## SIGNAL

The `signal` function is a standard C function that sets a function to handle a signal.

- ***Function Description***: `signal` sets a function to handle a signal. The function (or action) to be called when the signal occurs is passed as the second argument. It returns the previous action for the signal, or `SIG_ERR` if an error occurs.

- ***Prototype***: `void (*signal(int sig, void (*func)(int)))(int);`

- ***Library***:
```c
#include <signal.h>
```

Example:

```c

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void handle_sigint(int sig) {
	printf("Caught signal %d\n", sig);
}

int main() {
	signal(SIGINT, handle_sigint);
	while (1) {
		printf("Running...\n");
		sleep(1);
	}
	return 0;
}
```

*In this example, the program sets a function `handle_sigint` to handle the `SIGINT` signal (usually generated by the `Ctrl+C` command).*
*If the `SIGINT` signal is caught, the program prints "Caught signal 2". The program runs indefinitely, printing "Running..." every second, until it receives a `SIGINT` signal.*

[Back to Summary](#summary)
---------------------------------------------------------------------------------

## ACCESS

The `access` function is a standard C function that checks the file's accessibility.

- ***Function Description***: `access` checks whether the calling process can access the file pathname. It checks the file's permissions based on the mode parameter which can be `F_OK` (tests for the existence of the file), `R_OK` (tests for read permission), `W_OK` (tests for write permission), and `X_OK` (tests for execute or search permission).

- ***Prototype***: `int access(const char *pathname, int mode);`

- ***Library***:
```c
#include <unistd.h>
```

Example:

```c
#include <stdio.h>
#include <unistd.h>

int main() {
	if (access("myfile.txt", F_OK) != -1) {
		printf("File exists.\n");
	} else {
		printf("File doesn't exist.\n");
	}
	return 0;
}
```

*In this example, the program checks if the file myfile.txt exists. If the file exists, it prints "File exists.", otherwise it prints "File doesn't exist.".*

[Back to Summary](#summary)
---------------------------------------------------------------------------------
## STAT

The `stat` function retrieves information about a file or directory, such as its size, permissions, and timestamps. This information is stored in a `stat` structure.

- ***Function Description***: `stat` fills a `stat` structure with details about the specified file, allowing applications to gather metadata about files and directories.

- ***Prototype***: `int stat(const char *path, struct stat *buf);`

- ***Library***:

```cpp
#include <sys/stat.h>
#include <iostream>
#include <cstring>
```


```cpp
#include <sys/stat.h>
#include <iostream>
#include <cstring>

int main() {
    const char *filepath = "example.txt"; // Specify the file name
    struct stat file_info;

    // Retrieve file information
    if (stat(filepath, &file_info) == -1) {
        perror("stat");
        return 1;
    }

    // Print file information
    std::cout << "File: " << filepath << std::endl;
    std::cout << "Size: " << file_info.st_size << " bytes" << std::endl;
    std::cout << "Permissions: " << std::oct << (file_info.st_mode & 0777) << std::dec << std::endl;
    std::cout << "Last modified: " << ctime(&file_info.st_mtime); // Convert to local time

    return 0;
}

```

*In this C++ example, the `stat` function is used to retrieve information about the file "example.txt". If successful, it prints the file size in bytes, its permissions in octal format, and the last modified time. This function is useful for applications that need to manage or display metadata about files and directories.*

[Back to Summary](#summary)
---------------------------------------------------------------------------------

## OPEN

The `open` function is a standard C function that opens a file or device.

- ***Function Description***: `open` opens the file specified by pathname. The flags parameter determines the file access mode (read, write, read/write, create, etc.). It returns a file descriptor for the opened file, or -1 if an error occurs.

- ***Prototype***: `int open(const char *pathname, int flags);`

- ***Library***:
```c
#include <fcntl.h>
```

Example:

```c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
	int fd = open("myfile.txt", O_RDONLY);
	if (fd == -1) {
		printf("Failed to open file.\n");
	} else {
		printf("File opened successfully.\n");
		close(fd);
	}
	return 0;
}
```

*In this example, the program attempts to open the file myfile.txt in read-only mode. If the file is opened successfully, it prints "File opened successfully." and then closes the file.*
*If the file cannot be opened, it prints "Failed to open file.".*

[Back to Summary](#summary)
---------------------------------------------------------------------------------

## OPENDIR

The `opendir` function is a standard C function that opens a directory stream corresponding to the directory name, and returns a pointer to the directory stream. The stream is positioned at the first entry in the directory.

- ***Function Description***: `opendir` opens a directory stream corresponding to the directory name. The stream is positioned at the first entry in the directory. It returns a pointer to the directory stream. On error, NULL is returned.

- ***Prototype***: `DIR *opendir(const char *name);`

- ***Library***:
```c
#include <dirent.h>
```

Example:

```c
#include <stdio.h>
#include <dirent.h>

int main() {
	DIR *dir = opendir("/tmp");
	if (dir) {
		printf("Opened the directory /tmp\n");
	} else {
		printf("Failed to open the directory /tmp.\n");
	}
	closedir(dir);
	return 0;
}
```

*In this example, the program opens the directory `/tmp` using `opendir` and stores the directory stream in `dir`. If `opendir` succeeds, it prints "Opened the directory /tmp". If `opendir` fails, it prints "Failed to open the directory /tmp.".*
*After it's done with the directory, it closes the directory stream using `closedir`.*

[Back to Summary](#summary)
---------------------------------------------------------------------------------
## READDIR

The `readdir` function reads entries from a directory stream. It returns a pointer to a `dirent` structure representing the next directory entry.

- ***Function Description***: `readdir` retrieves the next entry in the directory stream, allowing applications to iterate over the files and directories within a specified directory.

- ***Prototype***: `struct dirent *readdir(DIR *dirp);`

- ***Library***:

```cpp
#include <dirent.h>
#include <iostream>
```


```cpp
#include <dirent.h>
#include <iostream>
#include <cstring>

int main() {
    const char *directory_path = "."; // Specify the directory path (current directory)
    DIR *dir = opendir(directory_path); // Open the directory

    if (dir == nullptr) {
        perror("opendir");
        return 1;
    }

    struct dirent *entry;
    
    // Read entries from the directory
    while ((entry = readdir(dir)) != nullptr) {
        std::cout << "Found: " << entry->d_name << std::endl; // Print the entry name
    }

    // Close the directory
    closedir(dir);

    return 0;
}

```

*In this C++ example, the opendir function is used to open the current directory. The `readdir` function is then called in a loop to read and print the names of all entries in the directory until there are no more entries. Finally, the directory is closed with closedir. This function is useful for applications that need to list files and directories within a specified directory.*

[Back to Summary](#summary)
---------------------------------------------------------------------------------
## CLOSEDIR

The `closedir` function closes a directory stream that was opened with `opendir`. This is necessary to release resources associated with the directory stream.

- ***Function Description***: `closedir` frees up the resources allocated for a directory stream and invalidates the pointer to that directory stream.

- ***Prototype***: `int closedir(DIR *dirp);`

- ***Library***:

```cpp
#include <dirent.h>
#include <iostream>
```


```cpp
#include <dirent.h>
#include <iostream>

int main() {
    const char *directory_path = "."; // Specify the directory path (current directory)
    DIR *dir = opendir(directory_path); // Open the directory

    if (dir == nullptr) {
        perror("opendir");
        return 1;
    }

    struct dirent *entry;
    
    // Read entries from the directory
    while ((entry = readdir(dir)) != nullptr) {
        std::cout << "Found: " << entry->d_name << std::endl; // Print the entry name
    }

    // Close the directory
    if (closedir(dir) == -1) {
        perror("closedir");
        return 1;
    }

    std::cout << "Directory closed successfully." << std::endl;

    return 0;
}

```

*In this C++ example, the `closedir` function is called after reading all entries from a directory. It releases the resources associated with the directory stream opened by opendir. If successful, it prints a confirmation message. Closing the directory is important to prevent resource leaks in applications that handle file system operations.*

[Back to Summary](#summary)