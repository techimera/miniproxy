#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

static int create_and_bind(char* port);
int main(int argc,char* argv[])
{
   printf("CoreCLR experiments...\n");
   return 0;
}

static int create_and_bind(char* port)
{
    struct addrinfo hints;
    struct addrinfo *res;
    struct addrinfo *iterator;
    int hr ,sfd;

    memset(&hints,0,sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    hr = getaddrinfo(NULL,port,&hints,&res);
    if(hr != 0)
    {
      fprintf(stderr, "getaddrinfo: %s\n",gai_strerror(hr));
      return -1;
    }

    for(iterator=res;iterator!=NULL;iterator= iterator->ai_next)
    {
      sfd = socket(iterator->ai_family,iterator->ai_socktype,iterator->ai_protocol);
      if(sfd == -1 ) continue;
      hr = bind(sfd,iterator->ai_addr,iterator->ai_addrlen);
      if(hr == 0) break;
      close(sfd);
    }

    return 0;
}