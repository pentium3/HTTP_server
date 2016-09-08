
#include "csapp.h"

static int Tcnt=0;

void run(int fd) 
{
    int is_static;
    struct stat sbuf;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char filename[MAXLINE];
    rio_t rio;

    Rio_readinitb(&rio, fd);
    if (!Rio_readlineb(&rio, buf, MAXLINE))
        return;
    printf("%s", buf);
    sscanf(buf, "%s %s %s", method, uri, version);
    if (strcasecmp(method, "GET")) 
    {
        clienterror(fd, method, "501", "Unsupported method", "Tiny does not support this method");
        return;
    }
    read_requesthdrs(&rio);

    parse_uri(uri, filename);
    if (stat(filename, &sbuf) < 0) 
    {
        clienterror(fd, filename, "404", "Not found", "Tiny couldn't find this file");
        return;
    }
          
    serve_static(fd, filename, sbuf.st_size);
}

void read_requesthdrs(rio_t *rp) 
{
    char buf[MAXLINE];
    Rio_readlineb(rp, buf, MAXLINE);
    printf("%s", buf);
    while(strcmp(buf, "\r\n")) 
    {
        Rio_readlineb(rp, buf, MAXLINE);
        printf("%s", buf);
    }
    return;
}

void parse_uri(char *uri, char *filename) 
{
    strcpy(filename, ".");
    strcat(filename, uri);
    if (uri[strlen(uri)-1] == '/')
           strcat(filename, "home.html");
}

void serve_static(int fd, char *filename, int filesize) 
{
    int srcfd;
    char *srcp, filetype[MAXLINE], buf[MAXBUF];
 
    get_filetype(filename, filetype); 
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
    sprintf(buf, "%sConnection: close\r\n", buf);
    sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
    sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
    Rio_writen(fd, buf, strlen(buf));
    printf("Response headers:\n");
    printf("%s", buf);

    srcfd = Open(filename, O_RDONLY, 0); 
    srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
    Close(srcfd); 
    Rio_writen(fd, srcp, filesize);
    Munmap(srcp, filesize); 
}

void get_filetype(char *filename, char *filetype) 
{
    if (strstr(filename, ".html"))
        strcpy(filetype, "text/html");
    else if (strstr(filename, ".gif"))
        strcpy(filetype, "image/gif");
    else if (strstr(filename, ".png"))
        strcpy(filetype, "image/png");
    else if (strstr(filename, ".jpg"))
        strcpy(filetype, "image/jpeg");
    else
        strcpy(filetype, "text/plain");
}  

void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg) 
{
    char buf[MAXLINE], body[MAXBUF];

    sprintf(body, "<html><title>Error</title>");
    sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
    sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
    sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
    sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
    Rio_writen(fd, buf, strlen(buf));
    Rio_writen(fd, body, strlen(body));
}

void *thread(void *vargp)
{
    Tcnt++;
    printf("Num of thread: %d\n",Tcnt);
    int connfd=*((int *)vargp);
    Pthread_detach(pthread_self());
    Free(vargp);
    run(connfd);
    Close(connfd);   
    Tcnt--;
    return NULL;
}

int main(int argc, char **argv) 
{
    pthread_t tid;
    int listenfd, connfd, *connfdp;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;

    if (argc != 2) 
    {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    listenfd = Open_listenfd(argv[1]);
    while (1) 
    {
        clientlen = sizeof(clientaddr);
        connfdp=malloc(sizeof(int));
        *connfdp = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        Getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0);
        printf("********************************\nAccepted connection from (%s, %s)\n", hostname, port);
        Pthread_create(&tid,NULL,thread,connfdp);
    }
}
