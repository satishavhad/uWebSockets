#ifndef USOCKETS_UWS_H
#define USOCKETS_UWS_H

#include "../uSockets/Berkeley.h"
#include "../uSockets/Epoll.h"

typedef uS::SocketDescriptor uv_os_sock_t;
#define WIN32_EXPORT

enum {
    HTTP_SOCKET_SERVER,
    WEB_SOCKET_SERVER,
    HTTP_SOCKET_CLIENT,
    WEB_SOCKET_CLIENT
};

namespace uS {

    using Loop = Epoll;
    using Context = Berkeley<Loop>;
    using Socket = Context::Socket;

    // TLS is completeley separate
    namespace TLS {
        struct Context {
            Context(void *ctx = nullptr) {

            }
        };

        struct SSL;
    }
}

#endif // USOCKETS_UWS_H
