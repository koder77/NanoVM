/****************************************************************************
*
* Filename: exe_socket_ipv6.c
*
* Author:   Stefan Pietzonke
* Project:  nano, virtual machine
*
* Purpose:  socket functions (ip v6)
*
* (c) Copyright Stefan Pietzonke (jay-t@gmx.net), 2011
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
*
***************************************************************************/

/* fallback for Amiga OS or any other without MSG_NOSIGNAL */
#ifndef MSG_NOSIGNAL
    #define MSG_NOSIGNAL 0
#endif


/* inet_ntop for Windows wrapper by Jeroen Massar */
#if OS_WINDOWS
const char *inet_ntop(int af, const void *src, char *dst, socklen_t cnt)
{
        if (af == AF_INET)
        {
                struct sockaddr_in in;
                memset(&in, 0, sizeof(in));
                in.sin_family = AF_INET;
                memcpy(&in.sin_addr, src, sizeof(struct in_addr));
                getnameinfo((struct sockaddr *)&in, sizeof(struct sockaddr_in), dst, cnt, NULL, 0, NI_NUMERICHOST);
                return dst;
        }
        else if (af == AF_INET6)
        {
                struct sockaddr_in6 in;
                memset(&in, 0, sizeof(in));
                in.sin6_family = AF_INET6;
                memcpy(&in.sin6_addr, src, sizeof(struct in_addr6));
                getnameinfo((struct sockaddr *)&in, sizeof(struct sockaddr_in6), dst, cnt, NULL, 0, NI_NUMERICHOST);
                return dst;
        }
        return NULL;
}

int inet_pton(int af, const char *src, void *dst)
{
        struct addrinfo hints, *res, *ressave;

        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = af;

        if (getaddrinfo(src, NULL, &hints, &res) != 0)
        {
                printf ("Couldn't resolve host %s\n", src);
                return -1;
        }

        ressave = res;

        while (res)
        {
                memcpy(dst, res->ai_addr, res->ai_addrlen);
                res = res->ai_next;
        }

        freeaddrinfo(ressave);
        return 0;
}

#endif


extern struct varlist *varlist;
extern struct sock sock[];

#if OS_AROS
    extern void *socket_mutex;
#else
    extern pthread_mutex_t socket_mutex;
#endif

extern struct vm_mem vm_mem;
struct pthreads pthreads[MAXPTHREADS];

/* helper function (taken from bgnet_socket_programming) */
char *get_ip_str (const struct sockaddr *sa, char *s, size_t maxlen)
{
    switch (sa->sa_family)
    {
        case AF_INET:
            inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr), s, maxlen);
            break;

        case AF_INET6:
            inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr), s, maxlen);
            break;

        default:
            strncpy(s, "Unknown AF", maxlen);
            return NULL;
    }
    return s;
}

void init_sockets (void)
{
    S4 i;

    for (i = 0; i <= MAXSOCKETS - 1; i++)
    {
        sock[i].status = FILECLOSED;
    }
}


#if OS_WINDOWS

S2 init_winsock (void)
{
    WORD wVersionRequested;
    WSADATA wsaData;

    wVersionRequested = MAKEWORD (2, 2);

    if (WSAStartup (wVersionRequested, &wsaData))
    {
        return (FALSE);
    }

    if (LOBYTE (wsaData.wVersion) != 2 || HIBYTE (wsaData.wVersion) != 2)
    {
        return (FALSE);
    }

    return (TRUE);
}

#endif

#if OS_AROS

S2 init_arossock (void)
{
    SocketBase = (struct Library *) OpenLibrary ("bsdsocket.library", 3);
    if (SocketBase == NULL)
    {
        return (FALSE);
    }
    else
    {
        return (TRUE);
    }
}

#endif

S2 exe_close_socket (S2 sock)
{
    S2 error;

    #if OS_WINDOWS
        error = closesocket (sock);
    #else
        error = close (sock);
    #endif

    return (error);
}

U1 exe_open_server (S8 *slist_ind, U1 *hostname, S8 port, S4 threadnum)
{
    /* reg1 = socket number, reg2 = hostname exp: "168.60.100.200", reg3 = port */

    NINT yes = 1;
    S2 server, error;
    S2 status;
    U1 port_string[256];
    S4 i, socketh = -1;
    struct addrinfo hints;
    struct addrinfo *servinfo;
    // will point to the results
    memset (&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;
    // don't care IPv4 or IPv6

    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

    snprintf (port_string, 256, "%d", port);
    // convert integer port number to a string

    pthread_mutex_lock (&socket_mutex);
    for (i = 0; i < MAXSOCKETS; i++)
    {
        if (sock[i].status == FILECLOSED)
        {
            socketh = i;
            break;
        }
    }

    if (socketh == -1)
    {
        /* error socket list full */
        pthread_mutex_unlock (&socket_mutex);
        return (ERR_FILE_OPEN);
    }

    *slist_ind = socketh;

    #if OS_WINDOWS
        if (! init_winsock ())
        {
            pthread_mutex_unlock (&socket_mutex);
            return (ERR_FILE_OPEN);
        }
    #endif

    #if OS_AROS
        if (! init_arossock ())
        {
            pthread_mutex_unlock (&socket_mutex);
            return (ERR_FILE_OPEN);
        }
    #endif

    if ((status = getaddrinfo (hostname, port_string, &hints, &servinfo)) != 0)
    {
        pthread_mutex_unlock (&socket_mutex);
        return (status);    /* return error code */
    }

    server = socket (servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if (server == -1)
    {
        pthread_mutex_unlock (&socket_mutex);
        return (errno);
    }


    /* avoiding socket already in use error */

    error = (setsockopt (server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (NINT)));
    if (error == -1)
    {
        pthread_mutex_unlock (&socket_mutex);
        return (errno);
    }

    error = bind (server, servinfo->ai_addr, servinfo->ai_addrlen);
    if (error == -1)
    {
        pthread_mutex_unlock (&socket_mutex);
        return (errno);
    }

    error = listen (server, SOMAXCONN);
    if (error == -1)
    {
        pthread_mutex_unlock (&socket_mutex);
        return (errno);
    }

    sock[*slist_ind].sock = server;
    sock[*slist_ind].servinfo = servinfo;
    sock[*slist_ind].type = SOCKSERVER;
    sock[*slist_ind].status = FILEOPEN;
    strcpy (sock[*slist_ind].client_ip, "");
    pthread_mutex_unlock (&socket_mutex);

    return (ERR_FILE_OK);
}

U1 exe_open_accept_server (S8 slist_ind, S8 *new_slist_ind)
{
    S2 connection;
    struct sockaddr_storage client;
    socklen_t addr_size;
    S4 i, socketh = -1;

    if (slist_ind < 0 || slist_ind > MAXSOCKETS - 1)
    {
        return (ERR_FILE_NUMBER);
    }

    if (sock[slist_ind].status != FILEOPEN || sock[slist_ind].type != SOCKSERVER)
    {
        return (ERR_FILE_READ);
    }

    addr_size = sizeof client;
    connection = accept (sock[slist_ind].sock, (struct sockaddr *) &client, &addr_size);
    if (connection == -1)
    {
        return (errno);
    }

    pthread_mutex_lock (&socket_mutex);
    for (i = 0; i < MAXSOCKETS; i++)
    {
        if (sock[i].status == FILECLOSED)
        {
            socketh = i;
            break;
        }
    }

    if (socketh == -1)
    {
        /* error socket list full */
        pthread_mutex_unlock (&socket_mutex);
        return (ERR_FILE_READ);
    }

    *new_slist_ind = socketh;

    if (get_ip_str ((struct sockaddr *) &client, sock[*new_slist_ind].client_ip, SOCKADDRESSLEN) == NULL)
    {
        pthread_mutex_unlock (&socket_mutex);
        return (ERR_FILE_READ);
    }

    sock[*new_slist_ind].serv_conn = connection;
    sock[*new_slist_ind].status = FILEOPEN;
    sock[*new_slist_ind].type = SOCKSERVER;
    pthread_mutex_unlock (&socket_mutex);

    return (ERR_FILE_OK);
}

U1 exe_open_client (S8 *slist_ind, U1 *hostname, S8 port, S4 threadnum)
{
    /* reg1 = socket number, reg2 = hostname exp: "168.60.100.200", reg3 = port */

    S2 client, error;
    S2 status;
    U1 port_string[256];
    S4 i, socketh = -1;
    struct addrinfo hints;
    struct addrinfo *servinfo;
    // will point to the results
    memset (&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;
    // don't care IPv4 or IPv6

    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

    snprintf (port_string, 256, "%d", port);
    // convert integer port number to a string

    pthread_mutex_lock (&socket_mutex);
    for (i = 0; i < MAXSOCKETS; i++)
    {
        if (sock[i].status == FILECLOSED)
        {
            socketh = i;
            break;
        }
    }

    if (socketh == -1)
    {
        /* error socket list full */
        pthread_mutex_unlock (&socket_mutex);
        return (ERR_FILE_OPEN);
    }

    *slist_ind = socketh;

    #if OS_WINDOWS
        if (! init_winsock ())
        {
            pthread_mutex_unlock (&socket_mutex);
            return (ERR_FILE_OPEN);
        }
    #endif

    #if OS_AROS
        if (! init_arossock ())
        {
            pthread_mutex_unlock (&socket_mutex);
            return (ERR_FILE_OPEN);
        }
    #endif

    if ((status = getaddrinfo (hostname, port_string, &hints, &servinfo)) != 0)
    {
        pthread_mutex_unlock (&socket_mutex);
        return (status);    /* return error code */
    }

    client = socket (servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if (client == -1)
    {
        pthread_mutex_unlock (&socket_mutex);
        return (errno);
    }

    error = connect (client, servinfo->ai_addr, servinfo->ai_addrlen);
    if (error == -1)
    {
        pthread_mutex_unlock (&socket_mutex);
        return (errno);
    }

    sock[*slist_ind].sock = client;
    sock[*slist_ind].type = SOCKCLIENT;
    sock[*slist_ind].status = FILEOPEN;
    pthread_mutex_unlock (&socket_mutex);

    return (ERR_FILE_OK);
}

U1 exe_close_server (S8 slist_ind)
{
    if (slist_ind < 0 || slist_ind > MAXSOCKETS - 1)
    {
        return (ERR_FILE_NUMBER);
    }

    if (sock[slist_ind].status == FILECLOSED || sock[slist_ind].type != SOCKSERVER)
    {
        return (ERR_FILE_CLOSE);
    }

    if (shutdown (sock[slist_ind].sock, SHUT_RDWR) == -1)   /* 0x02*/
    {
        return (ERR_FILE_CLOSE);
    }

    if (exe_close_socket (sock[slist_ind].sock) == -1)
    {
        return (ERR_FILE_CLOSE);
    }
    else
    {
        pthread_mutex_lock (&socket_mutex);
        sock[slist_ind].status = FILECLOSED;
        pthread_mutex_unlock (&socket_mutex);

        CLOSEWINSOCK();

        // servinfo now points to a linked list of 1 or more struct addrinfos
        // ... do everything until you don't need servinfo anymore ....
        freeaddrinfo (sock[slist_ind].servinfo); // free the linked-list

        return (ERR_FILE_OK);
    }
}


U1 exe_close_accept_server (S8 slist_ind)
{
    if (slist_ind < 0 || slist_ind > MAXSOCKETS - 1)
    {
        return (ERR_FILE_NUMBER);
    }

    if (sock[slist_ind].status == FILECLOSED || sock[slist_ind].type != SOCKSERVER)
    {
        return (ERR_FILE_CLOSE);
    }

    if (shutdown (sock[slist_ind].serv_conn, SHUT_RDWR) == -1)
    {
        return (ERR_FILE_CLOSE);
    }

    if (exe_close_socket (sock[slist_ind].serv_conn) == -1)
    {
        return (ERR_FILE_CLOSE);
    }
    else
    {
        strcpy (sock[slist_ind].client_ip, "");

        pthread_mutex_lock (&socket_mutex);
        sock[slist_ind].status = FILECLOSED;
        pthread_mutex_unlock (&socket_mutex);

        return (ERR_FILE_OK);
    }
}

U1 exe_close_client (S8 slist_ind)
{
    if (slist_ind < 0 || slist_ind > MAXSOCKETS - 1)
    {
        return (ERR_FILE_NUMBER);
    }

    if (sock[slist_ind].status == FILECLOSED || sock[slist_ind].type != SOCKCLIENT)
    {
        return (ERR_FILE_CLOSE);
    }

    if (shutdown (sock[slist_ind].sock, SHUT_RDWR) == -1)
    {
        return (ERR_FILE_CLOSE);
    }

    if (exe_close_socket (sock[slist_ind].sock) == -1)
    {
        return (ERR_FILE_CLOSE);
    }
    else
    {
        pthread_mutex_lock (&socket_mutex);
        sock[slist_ind].status = FILECLOSED;
        pthread_mutex_unlock (&socket_mutex); 

        CLOSEWINSOCK();

        return (ERR_FILE_OK);
    }
}

U1 exe_getclientaddr (S8 slist_ind, U1 *s_reg)
{
    if (slist_ind < 0 || slist_ind > MAXSOCKETS - 1)
    {
        return (ERR_FILE_NUMBER);
    }

    if (sock[slist_ind].status == FILECLOSED || sock[slist_ind].type != SOCKSERVER)
    {
        return (ERR_FILE_READ);
    }

    if (strlen (sock[slist_ind].client_ip) <= MAXSTRING_VAR)
    {
        strcpy (s_reg, sock[slist_ind].client_ip);

        return (ERR_FILE_OK);
    }
    else
    {
        return (ERR_FILE_READ);
    }
}

U1 exe_gethostname (U1 *s_reg)
{
    #if OS_WINDOWS
        if (! init_winsock ())
        {
            return (ERR_FILE_READ);
        }
    #endif

    #if OS_AROS
        if (! init_arossock ())
        {
            return (ERR_FILE_READ);
        }
    #endif

    if (gethostname (s_reg, MAXSTRING_VAR + 1) == -1)
    {
        CLOSEWINSOCK();

        return (ERR_FILE_READ);
    }
    else
    {
        CLOSEWINSOCK();

        return (ERR_FILE_OK);
    }
}

U1 exe_gethostbyname (U1 *s_reg1, U1 *s_reg2)
{
    struct hostent *hp = NULL;
    struct in_addr addr;

    #if OS_WINDOWS
        if (! init_winsock ())
        {
            return (ERR_FILE_READ);
        }
    #endif

    #if OS_AROS
        if (! init_arossock ())
        {
            return (ERR_FILE_READ);
        }
    #endif

    hp = (struct hostent *) gethostbyname (s_reg1);
    if (hp == NULL)
    {
        CLOSEWINSOCK();

        return (ERR_FILE_READ);
    }

    if (hp->h_addr_list[0] != NULL)
    {
        memcpy (&addr, hp->h_addr_list[0], sizeof (struct in_addr));
        if (strlen (inet_ntoa (addr)) <= MAXSTRING_VAR)
        {
            strcpy (s_reg2, inet_ntoa (addr));

            CLOSEWINSOCK();

            return (ERR_FILE_OK);
        }
        else
        {
            CLOSEWINSOCK();

            return (ERR_FILE_READ);
        }
    }
    else
    {
        CLOSEWINSOCK();

        return (ERR_FILE_READ);
    }
}

U1 exe_gethostbyaddr (U1 *s_reg1, U1 *s_reg2)
{
    struct hostent *hp;
    struct in_addr hostaddr;

    #if OS_WINDOWS
        if (! init_winsock ())
        {
            return (ERR_FILE_READ);
        }
    #endif

    #if OS_AROS
        if (! init_arossock ())
        {
            return (ERR_FILE_READ);
        }
    #endif

    hostaddr.s_addr = inet_addr (s_reg1);
    hp = (struct hostent *) gethostbyaddr ((U1 *) &hostaddr, sizeof (struct in_addr), AF_INET);
    if (hp == NULL)
    {
        CLOSEWINSOCK();

        return (ERR_FILE_READ);
    }

    if (strlen (hp->h_name) <= MAXSTRING_VAR - 1)
    {
        strcpy (s_reg2, hp->h_name);

        CLOSEWINSOCK();

        return (ERR_FILE_OK);
    }
    else
    {
        strcpy (s_reg2, "");

        CLOSEWINSOCK();

        return (ERR_FILE_READ);
    }
}

#if ! OS_ANDROID
S8 htonq (S8 hostd)
{
    U1 *netdptr;
    U1 *hostdptr;
    S2 i;
    S8 netd;

    netdptr = (U1 *) &netd;

    hostdptr = (U1 *) &hostd;
    hostdptr += sizeof (S8) - 1;

    #if BS_LITTLE_ENDIAN
        for (i = 0; i <= sizeof (S8) - 1; i++)
        {
            *netdptr++ = *hostdptr--;
        }
    #else
        netd = hostd;
    #endif

    return (netd);
}

S8 ntohq (S8 netd)
{
    U1 *netdptr;
    U1 *hostdptr;
    S2 i;
    S8 hostd;

    hostdptr = (U1 *) &hostd;

    netdptr = (U1 *) &netd;
    netdptr += sizeof (S8) - 1;

    #if BS_LITTLE_ENDIAN
        for (i = 0; i <= sizeof (S8) - 1; i++)
        {
            *hostdptr++ = *netdptr--;
        }
    #else
        hostd = netd;
    #endif

    return (hostd);
}
#endif

F8 htond (F8 hostd)
{
    U1 *netdptr;
    U1 *hostdptr;
    S2 i;
    F8 netd;

    netdptr = (U1 *) &netd;

    hostdptr = (U1 *) &hostd;
    hostdptr += sizeof (F8) - 1;

    #if BS_LITTLE_ENDIAN
        for (i = 0; i <= sizeof (F8) - 1; i++)
        {
            *netdptr++ = *hostdptr--;
        }
    #else
        netd = hostd;
    #endif

    return (netd);
}

F8 ntohd (F8 netd)
{
    U1 *netdptr;
    U1 *hostdptr;
    S2 i;
    F8 hostd;

    hostdptr = (U1 *) &hostd;

    netdptr = (U1 *) &netd;
    netdptr += sizeof (F8) - 1;

    #if BS_LITTLE_ENDIAN
        for (i = 0; i <= sizeof (F8) - 1; i++)
        {
            *hostdptr++ = *netdptr--;
        }
    #else
        hostd = netd;
    #endif

    return (hostd);
}


U1 exe_sread (S4 slist_ind, S4 len)
{
    U1 *buf;
    S2 sockh = 0, ret;
    S4 todo, buf_ind = 0;

    if (slist_ind < 0 || slist_ind > MAXSOCKETS - 1)
    {
        return (ERR_FILE_NUMBER);
    }

    if (sock[slist_ind].status != FILEOPEN)
    {
        return (ERR_FILE_OPEN);
    }

    if (len < 0 || len > SOCKBUFSIZE)
    {
        return (ERR_FILE_READ);
    }

    switch (sock[slist_ind].type)
    {
        case SOCKSERVER:
            sockh = sock[slist_ind].serv_conn;
            break;

        case SOCKCLIENT:
            sockh = sock[slist_ind].sock;
            break;
    }

    todo = len;
    buf = sock[slist_ind].buf;

    while (todo > 0)
    {
        ret = recv (sockh, &(buf[buf_ind]), todo, MSG_NOSIGNAL);
        if (ret == -1)
        {
            return (errno);
        }

        todo = todo - ret;
        buf_ind = buf_ind + ret;
    }

    return (ERR_FILE_OK);
}

U1 exe_swrite (S4 slist_ind, S4 len)
{
    U1 *buf;
    S2 sockh = 0, ret;
    S4 todo, buf_ind = 0;

    if (slist_ind < 0 || slist_ind > MAXSOCKETS - 1)
    {
        return (ERR_FILE_NUMBER);
    }

    if (sock[slist_ind].status != FILEOPEN)
    {
        return (ERR_FILE_OPEN);
    }

    if (len < 0 || len > SOCKBUFSIZE)
    {
        return (ERR_FILE_WRITE);
    }

    switch (sock[slist_ind].type)
    {
        case SOCKSERVER:
            sockh = sock[slist_ind].serv_conn;
            break;

        case SOCKCLIENT:
            sockh = sock[slist_ind].sock;
            break;
    }

    todo = len;
    buf = sock[slist_ind].buf;

    while (todo > 0)
    {
        ret = send (sockh, &(buf[buf_ind]), todo, MSG_NOSIGNAL);
        if (ret == -1)
        {
            return (errno);
        }

        todo = todo - ret;
        buf_ind = buf_ind + ret;
    }

    return (ERR_FILE_OK);
}

/* read ------------------------------------------------------------- */

U1 exe_sread_byte (S8 slist_ind, S8 *reg)
{
    U1 ret;

    ret = exe_sread (slist_ind, sizeof (U1));
    if (ret != ERR_FILE_OK)
    {
        return (ret);
    }

    *reg = (S8) sock[slist_ind].buf[0];

    return (ERR_FILE_OK);
}

U1 exe_sread_array_byte (S8 slist_ind, struct varlist *varlist, S4 b_var, S8 len)
{
    U1 ret;
    U1 *new_b_memptr;
    S4 i;

    if (len < 0)
    {
        return (ERR_FILE_READ);
    }

    ret = exe_sread (slist_ind, len);
    if (ret != ERR_FILE_OK)
    {
        return (ret);
    }

    if (varlist[b_var].size >= len)
    {
        for (i = len - 1; i >= 0; i--)
        {
            new_b_memptr = (U1 *) get_vmvar (varlist, b_var, i, VM_READ);
            if (vm_mem.error)
            {
                return (FALSE);
            }

            *new_b_memptr = sock[slist_ind].buf[i];
        }
    }
    else
    {
        /* array overflow */

        return (ERR_FILE_READ);
    }

    return (ERR_FILE_OK);
}

U1 exe_sread_int (S8 slist_ind, S8 *reg)
{
    U1 ret, *ptr;
    S2 i;
    S2 n;

    ret = exe_sread (slist_ind, sizeof (S2));
    if (ret != ERR_FILE_OK)
    {
        return (ret);
    }

    ptr = (U1 *) &n;

    for (i = 0; i <= sizeof (S2) - 1; i++)
    {
        *ptr++ = sock[slist_ind].buf[i];
    }

    *reg = ntohs ((S2) n);

    return (ERR_FILE_OK);
}

U1 exe_sread_lint (S8 slist_ind, S8 *reg)
{
    U1 ret, *ptr;
    S2 i;
    S4 n;

    ret = exe_sread (slist_ind, sizeof (S4));
    if (ret != ERR_FILE_OK)
    {
        return (ret);
    }

    ptr = (U1 *) &n;

    for (i = 0; i <= sizeof (S4) - 1; i++)
    {
        *ptr++ = sock[slist_ind].buf[i];
    }

    *reg = ntohl (n);

    return (ERR_FILE_OK);
}

U1 exe_sread_qint (S8 slist_ind, S8 *reg)
{
    U1 ret, *ptr;
    S2 i;
    S8 n;

    ret = exe_sread (slist_ind, sizeof (S8));
    if (ret != ERR_FILE_OK)
    {
        return (ret);
    }

    ptr = (U1 *) &n;

    for (i = 0; i <= sizeof (S8) - 1; i++)
    {
        *ptr++ = sock[slist_ind].buf[i];
    }

    *reg = ntohq (n);

    return (ERR_FILE_OK);
}

U1 exe_sread_double (S8 slist_ind, F8 *reg)
{
    U1 ret, *ptr;
    S2 i;
    F8 n;

    ret = exe_sread (slist_ind, sizeof (F8));
    if (ret != ERR_FILE_OK)
    {
        return (ret);
    }

    ptr = (U1 *) &n;

    for (i = 0; i <= sizeof (F8) - 1; i++)
    {
        *ptr++ = sock[slist_ind].buf[i];
    }

    *reg = ntohd (n);

    return (ERR_FILE_OK);
}

U1 exe_sread_string (S8 slist_ind, U1 *s_reg, S8 len)
{
    U1 ret;

    if (len < 0)
    {
        return (ERR_FILE_READ);
    }

    ret = exe_sread (slist_ind, len);
    if (ret != ERR_FILE_OK)
    {
        return (ret);
    }

    sock[slist_ind].buf[len] = BINUL;

    if (MAXSTRING_VAR + 1 >= len + 1)
    {
        strcpy (s_reg, sock[slist_ind].buf);
    }
    else
    {
        /* string overflow */

        return (ERR_FILE_READ);
    }

    return (ERR_FILE_OK);
}

U1 exe_sread_line_string (S8 slist_ind, U1 *s_reg)
{
    /* read CRLF or LF terminated line */

    U1 ret;
    U1 ch;
    U1 end = FALSE;
    U1 error = FALSE;
    S4 i = 0, len;

    len = MAXSTRING_VAR - 2;

    while (! end)
    {
        ret = exe_sread (slist_ind, 1);
        if (ret != ERR_FILE_OK)
        {
            error = TRUE; end = TRUE;

            if (i == 0)
            {
                /* error at first read, break while */
                break;
            }
        }

        ch = sock[slist_ind].buf[0];

        if (ch != '\n')
        {
            if (i <= len)
            {
                s_reg[i] = ch;
                i++;
            }
            else
            {
               error = TRUE; end = TRUE;
            }
        }
        else
        {
            /* line end */
            /* check if last char was a CR */

            if (s_reg[i - 1] == '\r')
            {
                i--;
            }

            s_reg[i] = BINUL;

            end = TRUE;
        }
    }

    if (error)
    {
        return (ERR_FILE_READ);
    }
    else
    {
        return (ERR_FILE_OK);
    }
}


/* write ------------------------------------------------------------ */

U1 exe_swrite_byte (S8 slist_ind, S8 reg)
{
    U1 ret;

    if (slist_ind < 0 || slist_ind > MAXSOCKETS - 1)
    {
        return (ERR_FILE_NUMBER);
    }

    sock[slist_ind].buf[0] = (U1) reg;

    ret = exe_swrite (slist_ind, sizeof (U1));

    return (ret);
}

U1 exe_swrite_array_byte (S8 slist_ind, struct varlist *varlist, S4 b_var, S8 len)
{
    U1 ret;
    U1 *new_b_memptr;
    S4 i;

    if (slist_ind < 0 || slist_ind > MAXSOCKETS - 1)
    {
        return (ERR_FILE_NUMBER);
    }

    if (len < 0 || len > SOCKBUFSIZE)
    {
        return (ERR_FILE_WRITE);
    }

    if (varlist[b_var].s_m == NULL)
    {
        return (ERR_FILE_WRITE);
    }

    for (i = len - 1; i >= 0; i--)
    {
        new_b_memptr = (U1 *) get_vmvar (varlist, b_var, i, VM_WRITE);
        if (vm_mem.error)
        {
            return (FALSE);
        }

        sock[slist_ind].buf[i] = *new_b_memptr;
    }

    ret = exe_swrite (slist_ind, len);

    return (ret);
}

U1 exe_swrite_int (S8 slist_ind, S8 reg)
{
    U1 ret, *ptr;
    S2 i;
    S2 n;

    if (slist_ind < 0 || slist_ind > MAXSOCKETS - 1)
    {
        return (ERR_FILE_NUMBER);
    }

    n = htons ((S2) reg);
    ptr = (U1 *) &n;

    for (i = 0; i <= sizeof (S2) - 1; i++)
    {
        sock[slist_ind].buf[i] = *ptr++;
    }

    ret = exe_swrite (slist_ind, sizeof (S2));

    return (ret);
}

U1 exe_swrite_lint (S8 slist_ind, S8 reg)
{
    U1 ret, *ptr;
    S2 i;
    S4 n;

    if (slist_ind < 0 || slist_ind > MAXSOCKETS - 1)
    {
        return (ERR_FILE_NUMBER);
    }

    n = htonl (reg);
    ptr = (U1 *) &n;

    for (i = 0; i <= sizeof (S4) - 1; i++)
    {
        sock[slist_ind].buf[i] = *ptr++;
    }

    ret = exe_swrite (slist_ind, sizeof (S4));

    return (ret);
}

U1 exe_swrite_qint (S8 slist_ind, S8 reg)
{
    U1 ret, *ptr;
    S2 i;
    S8 n;

    if (slist_ind < 0 || slist_ind > MAXSOCKETS - 1)
    {
        return (ERR_FILE_NUMBER);
    }

    n = htonq (reg);
    ptr = (U1 *) &n;

    for (i = 0; i <= sizeof (S8) - 1; i++)
    {
        sock[slist_ind].buf[i] = *ptr++;
    }

    ret = exe_swrite (slist_ind, sizeof (S8));

    return (ret);
}

U1 exe_swrite_double (S8 slist_ind, F8 reg)
{
    U1 ret, *ptr;
    S2 i;
    F8 n;

    if (slist_ind < 0 || slist_ind > MAXSOCKETS - 1)
    {
        return (ERR_FILE_NUMBER);
    }

    n = htond (reg);
    ptr = (U1 *) &n;

    for (i = 0; i <= sizeof (F8) - 1; i++)
    {
        sock[slist_ind].buf[i] = *ptr++;
    }

    ret = exe_swrite (slist_ind, sizeof (F8));

    return (ret);
}

U1 exe_swrite_string (S8 slist_ind, U1 *s_reg)
{
    U1 ret, buf_end = FALSE;
    S4 i, j, buf_ind = -1, len, nlch_len;

    if (slist_ind < 0 || slist_ind > MAXSOCKETS - 1)
    {
        return (ERR_FILE_NUMBER);
    }

    if (strlen (s_reg) > SOCKBUFSIZE)
    {
        return (ERR_FILE_WRITE);
    }

    nlch_len = strlen (varlist[FILE_NEWLINE].s_m) - 1;
    len = strlen (s_reg) - 1;

    for (i = 0; i <= len; i++)
    {
        if (s_reg[i] == '\n')
        {
            /* write newline string to buffer */

            for (j = 0; j <= nlch_len; j++)
            {
                if (buf_ind < SOCKBUFSIZE - 2)
                {
                    buf_ind++;
                    sock[slist_ind].buf[buf_ind] = varlist[FILE_NEWLINE].s_m[j];
                }
                else
                {
                    buf_end = TRUE;
                    break;
                }
            }
        }
        else
        {
            if (buf_ind < SOCKBUFSIZE - 2)
            {
                buf_ind++;
                sock[slist_ind].buf[buf_ind] = s_reg[i];
            }
            else
            {
                buf_end = TRUE;
            }
        }

        if (buf_end)
        {
            break;
        }
    }

    if (buf_end)
    {
        return (ERR_FILE_WRITE);
    }

    buf_ind++;
    sock[slist_ind].buf[buf_ind] = BINUL;

    ret = exe_swrite (slist_ind, strlen (sock[slist_ind].buf));

    return (ret);
}

U1 exe_swrite_str_lint (S8 slist_ind, S8 reg)
{
    U1 ret;

    if (slist_ind < 0 || slist_ind > MAXSOCKETS - 1)
    {
        return (ERR_FILE_NUMBER);
    }

    sprintf (sock[slist_ind].buf, "%li", reg);

    ret = exe_swrite (slist_ind, strlen (sock[slist_ind].buf));

    return (ret);
}

U1 exe_swrite_str_double (S8 slist_ind, F8 reg)
{
    U1 ret;

    if (slist_ind < 0 || slist_ind > MAXSOCKETS - 1)
    {
        return (ERR_FILE_NUMBER);
    }

    sprintf (sock[slist_ind].buf, "%.10lf", reg);

    ret = exe_swrite (slist_ind, strlen (sock[slist_ind].buf));

    return (ret);
}

U1 exe_swrite_newline (S8 slist_ind, S8 reg)
{
    U1 ret;
    S4 i;
    S4 len;

    if (slist_ind < 0 || slist_ind > MAXSOCKETS - 1)
    {
        return (ERR_FILE_NUMBER);
    }

    len = reg * strlen (varlist[FILE_NEWLINE].s_m);
    if (len > SOCKBUFSIZE)
    {
        return (ERR_FILE_WRITE);
    }

    strcpy (sock[slist_ind].buf, "");

    for (i = reg; i > 0; i--)
    {
        strcat (sock[slist_ind].buf, varlist[FILE_NEWLINE].s_m);
    }

    ret = exe_swrite (slist_ind, strlen (sock[slist_ind].buf));

    return (ret);
}

U1 exe_swrite_space (S8 slist_ind, S8 reg)
{
    U1 ret;
    S4 i;

    if (slist_ind < 0 || slist_ind > MAXSOCKETS - 1)
    {
        return (ERR_FILE_NUMBER);
    }

    if (reg > SOCKBUFSIZE)
    {
        return (ERR_FILE_WRITE);
    }

    for (i = reg; i > 0; i--)
    {
        sock[slist_ind].buf[i] = ' ';
    }
    sock[slist_ind].buf[i] = BINUL;

    ret = exe_swrite (slist_ind, strlen (sock[slist_ind].buf));

    return (ret);
}
