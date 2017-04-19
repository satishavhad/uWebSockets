#ifndef USOCKETS_UWS_H
#define USOCKETS_UWS_H

#include "../uSockets/Berkeley.h"
#include "../uSockets/Epoll.h"

typedef uS::SocketDescriptor uv_os_sock_t;
#define WIN32_EXPORT

namespace uS {

    struct Socket : public uS::Berkeley<uS::Epoll>::Socket {
        Socket *next = nullptr, *prev = nullptr;
    };

    struct Node : public uS::Berkeley<uS::Epoll> {

    };

    struct NodeData {

    };

    struct Timer : public uS::Epoll::Timer {

    };

    struct Poll : public uS::Epoll::Poll {

    };

    namespace TLS {
        struct Context {
            Context(void *ctx = nullptr);
        };

        struct SSL;
    }
}

#endif // USOCKETS_UWS_H
