#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>


// forward defintion of all functions...
static int  make_socket_nonblocking(int socket);
static int  create_and_bind(char* port);
static void accept_connection(int sfd);
static void read_data(int fd);


static int server;
static int event_loop;

int main(int argc, char* argv[])
{
    int  hr;
    struct epoll_event  event;
    struct epoll_event* events;


    // create and bind to port
    server = create_and_bind("8080");
    make_socket_nonblocking(server);
    hr = listen(server, SOMAXCONN);
    if (hr == -1)
    {
        fprintf(stderr, "listen failed...\n");
        abort();
    }
    printf("%s\n", "listening on port 8080");


    event_loop =  epoll_create1(0);

    event.data.fd = server;
    event.events  = EPOLLIN | EPOLLET;

    hr = epoll_ctl(event_loop, EPOLL_CTL_ADD, server, &event);
    if (hr == -1)
    {
        perror("epoll_ctl");
        abort();
    }
    events = calloc(64, sizeof (event));

    int n, i;
    // the event loop
    for (;;)
    {
        n =  epoll_wait(event_loop, events, 64, -1);
        for (i = 0; i < n; i++)
        {
            // notification on the listening socket,which could mean
            // it's either and incoming connection or data
            if (server ==  events[i].data.fd)
            {
                accept_connection(server);
            }
            else
            {
                read_data(events[i].data.fd);
            }

        }
    }

    // naive cleanup
    free(events);
    close(server);

    return 0;
}
static void read_data(int fd)
{
    ssize_t nread;
    char buf[512];
    int nwrite;

    for (;;)
    {
        nread = read(fd, buf, 512);
        // no more data left to  read
        if (nread == 0 || (nread == -1 && errno != EAGAIN))
        {
            break;
        }
        else
        {
            nwrite = write(1, buf, nread);
        }
    }
    printf("closed connection on {%d}\n", fd);
}

static void accept_connection(int sfd)
{
    int hr;
    struct sockaddr in_addr;
    socklen_t in_len;
    int in_sockfd;
    char host[128] , server[128];
    struct epoll_event  event;

    for (;;)
    {
        in_len = sizeof in_addr;
        memset(&in_addr, 0, in_len);

        in_sockfd =  accept(sfd, &in_addr, &in_len);

        if ( in_sockfd == -1 )
        {
            if (errno == EAGAIN  || errno == EWOULDBLOCK ) {
                break;
            }
            else {
                perror("accept");
            }
        }
        hr = getnameinfo(&in_addr,
                         in_len,
                         host,
                         sizeof host,
                         server,
                         sizeof server,
                         NI_NUMERICHOST |  NI_NUMERICSERV);
        if (hr == 0)
        {
            printf("Connection accepted on {%d} (host = %s,port=%s)\n", in_sockfd, host, server);
        }

        make_socket_nonblocking(in_sockfd);
        event.data.fd = in_sockfd;
        event.events = EPOLLIN | EPOLLET;

        hr = epoll_ctl(event_loop, EPOLL_CTL_ADD, in_sockfd, &event);

        if ( hr == -1)
        {

            perror("epoll_ctl");
            abort();
        }
    }
}

static int make_socket_nonblocking(int socket)
{
    int flags, s;
    flags = fcntl(socket, F_GETFL, 0);
    flags |= O_NONBLOCK;
    s = fcntl(socket, F_SETFL, flags);
    if (s == -1)
    {
        fprintf(stderr, "failed to set socket non-blocking..\n");
        return -1;
    }
    return 0;
}

static int create_and_bind(char* port)
{
    struct addrinfo hints;
    struct addrinfo *res;
    struct addrinfo *addr;
    int hr , sfd;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    hr = getaddrinfo(NULL, port, &hints, &res);
    if (hr != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(hr));
        return -1;
    }

    for (addr = res; addr != NULL; addr = addr->ai_next)
    {
        sfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (sfd == -1 ) continue;
        hr = bind(sfd, addr->ai_addr, addr->ai_addrlen);
        if (hr == 0) break;
        close(sfd);
    }

    if (addr == NULL)
    {
        fprintf( stderr, "failed to bind socket\n");
        return -1;
    }

    freeaddrinfo(res);
    return sfd;
}
