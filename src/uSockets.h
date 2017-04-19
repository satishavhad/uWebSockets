#ifndef USOCKETS_UWS_H
#define USOCKETS_UWS_H

#include "../uSockets/Berkeley.h"
#include "../uSockets/Epoll.h"

typedef uS::SocketDescriptor uv_os_sock_t;
#define WIN32_EXPORT

enum {
    HTTP_SOCKET_SERVER
};

namespace uS {

    // Hub is a uS::Loop
    using Loop = Epoll;

    // Group is a Context and knows its Loop (Hub)
    using Context = Berkeley<Loop>;

    // Socket knows its Context / Group
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
