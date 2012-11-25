#include "gl_code.h"
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <errno.h>

Listener *listener = NULL;
pthread_mutex_t exit_mutex = PTHREAD_MUTEX_INITIALIZER;
int exit_status = 0;

void *connection_entry(void *in) {
    int fd = (int)in;
    Message message;

    
    LOGI("Connection entry");
    while(true) {
        ssize_t bytes_read = read(fd,&message,sizeof(Message));
        if(bytes_read != sizeof(Message)) {
            if(bytes_read <= 0) {
                close(fd);
                return NULL;
            }
            LOGI("Only read %d bytes",bytes_read);
            continue;
        }
        LOGI("Got: %.8x %.8x %.8x",message.type,message.data[0],message.data[1]);
        switch(message.type) {
        case next_item:
            viewlist->CurrentView()->NextItem();
            break;
        case horizontal:
            viewlist->CurrentView()->Move(((GLfloat)message.data[0])/1000.0,0);
            break;
        case vertical:
            viewlist->CurrentView()->Move(0,-((GLfloat)message.data[0])/1000.0);
            break;
        case view_right:
            viewlist->Next();
            break;
        case view_left:
            viewlist->Prev();
            break;
        case view_absolute:
            viewlist->SwitchView(message.data[0],message.data[1],((float)message.data[2])/256);
            break;
        case view_rotary:
            viewlist->CurrentView()->SetRotary(message.data[0],((float)message.data[1])/256,1);
            break;
        case select_but:
            viewlist->CurrentView()->Select();
            break;
        case vertical_pos:
            viewlist->CurrentView()->VerticalPos(((float)message.data[0])/256,1);
            break;
        default:
            LOGI("Got unknown message type %d",message.type);
            break;
        }
    }
    return NULL;
}

void *listen_entry(void *in) {
    int exit = 0;
    struct sockaddr_in sock_addr;
    int reuse_addr = 1;
    fd_set socks;
    struct timeval timeout;
    int socket_fd;
    int readsocks;

    LOGI("listen entry");

    if(-1 == (socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)))
    {
        LOGE("socket error");
        return NULL;
    }

    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));

    memset((char *) &sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    sock_addr.sin_port = htons(4919);

    while(-1 == bind(socket_fd,(struct sockaddr *)&sock_addr, sizeof(sock_addr)))
    {
        LOGE("bind_error %d %s",errno,strerror(errno));
        //sleep(1);
        return NULL;
    }

    if(-1 == listen(socket_fd, 10))
    {
        LOGE("listen error");
        return NULL;
    }
    
    while(!exit)
    {
        exit = GetExit();
        if(exit)
            break;

        timeout.tv_sec  = 1;
        timeout.tv_usec = 0;

        FD_ZERO(&socks);
	FD_SET(socket_fd,&socks);

        readsocks = select(socket_fd+1, &socks, NULL, NULL, &timeout);
        if(readsocks > 0)
        {
            pthread_t temp_thread;
            int connect_fd = accept(socket_fd, NULL, NULL);
            pthread_create(&temp_thread,NULL,connection_entry,(void*)connect_fd); //bit of a dodgy cast!
        }
    }
    return NULL;
}



Listener::Listener(int port) {
    //Create a thread for listening
    SetExit(0);
    if(0 != pthread_create(&listen_thread,NULL,listen_entry,NULL))
    {
        throw THREAD_ERROR;
    }
}

Listener::~Listener() {
    SetExit(1);
    pthread_join(listen_thread,NULL);
}


int GetExit() {
    int out;
    pthread_mutex_lock(&exit_mutex);
    out = exit_status;
    pthread_mutex_unlock(&exit_mutex);
}

void SetExit(int in) {
    pthread_mutex_lock(&exit_mutex);
    exit_status = in;
    pthread_mutex_unlock(&exit_mutex);
}
