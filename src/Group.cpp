#include "Group.h"
#include "Hub.h"

namespace uWS {

template <bool isServer>
void Group<isServer>::setUserData(void *user) {
    this->userData = user;
}

template <bool isServer>
void *Group<isServer>::getUserData() {
    return userData;
}

// kills connect and shutdown sockets (can use userData as tracker)
template <bool isServer>
void Group<isServer>::limboTimerCallback(uS::Loop::Timer *timer) {
    Group<isServer> *group = static_cast<Group<isServer> *>(timer->getData());

    // for each limboSocket
//    group->forEach([](uWS::WebSocket<isServer> *webSocket) {
//        if (webSocket->hasOutstandingPong) {
//            webSocket->terminate();
//        } else {
//            webSocket->hasOutstandingPong = true;
//        }
//    });
}

// this callback should be the main tick of all sockets in this group
// including any timer in connect or shutdown state
template <bool isServer>
void Group<isServer>::timerCallback(uS::Loop::Timer *timer) {
    Group<isServer> *group = static_cast<Group<isServer> *>(timer->getData());

    group->forEach(Group<isServer>::WEBSOCKET, [](uS::Socket *socket) {
        uWS::WebSocket<isServer> *webSocket = static_cast<uWS::WebSocket<isServer> *>(socket);
        if (webSocket->hasOutstandingPong) {
            webSocket->terminate();
        } else {
            webSocket->hasOutstandingPong = true;
        }
    });

    if (group->userPingMessage.length()) {
        group->broadcast(group->userPingMessage.data(), group->userPingMessage.length(), OpCode::TEXT);
    } else {
        group->broadcast(nullptr, 0, OpCode::PING);
    }
}

template <bool isServer>
void Group<isServer>::add(int chainIndex, uS::Socket *socket) {
    Socket *&head = chainHead[chainIndex];
    if (head) {
        head->prev = socket;
        socket->next = head;
    } else {
        socket->next = nullptr;

        if (chainIndex == HTTPSOCKET) {
            httpTimer = new uS::Loop::Timer(getLoop());
            httpTimer->setData(this);
            httpTimer->start([](uS::Loop::Timer *httpTimer) {
                Group<isServer> *group = static_cast<Group<isServer> *>(httpTimer->getData());
                group->forEach(Group<isServer>::HTTPSOCKET, [](uS::Socket *socket) {
                    HttpSocket<isServer> *httpSocket = static_cast<HttpSocket<isServer> *>(socket);
                    if (httpSocket->missedDeadline) {
                        httpSocket->terminate();
                    } else if (!httpSocket->outstandingResponsesHead) {
                        httpSocket->missedDeadline = true;
                    }
                });
            }, 1000, 1000);
        }
    }
    head = socket;
    socket->prev = nullptr;
}

template <bool isServer>
void Group<isServer>::remove(int chainIndex, uS::Socket *socket) {
    if (iterators.size()) {
        iterators.top() = socket->next;
    }
    if (socket->prev == socket->next) {
        chainHead[chainIndex] = nullptr;

        if (chainIndex == HTTPSOCKET) {
            httpTimer->stop();
            httpTimer->close();
        }
    } else {
        if (socket->prev) {
            static_cast<uS::Socket *>(socket->prev)->next = socket->next;
        } else {
            chainHead[chainIndex] = socket->next;
        }
        if (socket->next) {
            static_cast<uS::Socket *>(socket->next)->prev = socket->prev;
        }
    }
}

template <bool isServer>
void Group<isServer>::startAutoPing(int intervalMs, std::string userMessage) {
    timer = new uS::Loop::Timer(getLoop());
    timer->setData(this);
    timer->start(timerCallback, intervalMs, intervalMs);
    userPingMessage = userMessage;
}

template <bool isServer>
Group<isServer>::Group(int extensionOptions, uS::Loop *loop) : extensionOptions(extensionOptions), uS::Context(loop) {
    connectionHandler = [](WebSocket<isServer> *, HttpRequest) {};
    transferHandler = [](WebSocket<isServer> *) {};
    messageHandler = [](WebSocket<isServer> *, char *, size_t, OpCode) {};
    disconnectionHandler = [](WebSocket<isServer> *, int, char *, size_t) {};
    pingHandler = pongHandler = [](WebSocket<isServer> *, char *, size_t) {};
    errorHandler = [](errorType) {};
    httpRequestHandler = [](HttpResponse *, HttpRequest, char *, size_t, size_t) {};
    httpConnectionHandler = [](HttpSocket<isServer> *) {};
    httpDisconnectionHandler = [](HttpSocket<isServer> *) {};
    httpCancelledRequestHandler = [](HttpResponse *) {};
    httpDataHandler = [](HttpResponse *, char *, size_t, size_t) {};

    this->extensionOptions |= CLIENT_NO_CONTEXT_TAKEOVER | SERVER_NO_CONTEXT_TAKEOVER;
}

// this is really just implemented in the Context!
template <bool isServer>
void Group<isServer>::stopListening() {

    uS::Context::stopListening();

    // stop context

//    if (async) {
//        async->close();
//    }
}

template <bool isServer>
void Group<isServer>::onConnection(std::function<void (WebSocket<isServer> *, HttpRequest)> handler) {
    connectionHandler = handler;
}

template <bool isServer>
void Group<isServer>::onTransfer(std::function<void (WebSocket<isServer> *)> handler) {
    transferHandler = handler;
}

template <bool isServer>
void Group<isServer>::onMessage(std::function<void (WebSocket<isServer> *, char *, size_t, OpCode)> handler) {
    messageHandler = handler;
}

template <bool isServer>
void Group<isServer>::onDisconnection(std::function<void (WebSocket<isServer> *, int, char *, size_t)> handler) {
    disconnectionHandler = handler;
}

template <bool isServer>
void Group<isServer>::onPing(std::function<void (WebSocket<isServer> *, char *, size_t)> handler) {
    pingHandler = handler;
}

template <bool isServer>
void Group<isServer>::onPong(std::function<void (WebSocket<isServer> *, char *, size_t)> handler) {
    pongHandler = handler;
}

template <bool isServer>
void Group<isServer>::onError(std::function<void (typename Group::errorType)> handler) {
    errorHandler = handler;
}

template <bool isServer>
void Group<isServer>::onHttpConnection(std::function<void (HttpSocket<isServer> *)> handler) {
    httpConnectionHandler = handler;
}

template <bool isServer>
void Group<isServer>::onHttpRequest(std::function<void (HttpResponse *, HttpRequest, char *, size_t, size_t)> handler) {
    httpRequestHandler = handler;
}

template <bool isServer>
void Group<isServer>::onHttpData(std::function<void(HttpResponse *, char *, size_t, size_t)> handler) {
    httpDataHandler = handler;
}

template <bool isServer>
void Group<isServer>::onHttpDisconnection(std::function<void (HttpSocket<isServer> *)> handler) {
    httpDisconnectionHandler = handler;
}

template <bool isServer>
void Group<isServer>::onCancelledHttpRequest(std::function<void (HttpResponse *)> handler) {
    httpCancelledRequestHandler = handler;
}

template <bool isServer>
void Group<isServer>::onHttpUpgrade(std::function<void(HttpSocket<isServer> *, HttpRequest)> handler) {
    httpUpgradeHandler = handler;
}

template <bool isServer>
void Group<isServer>::broadcast(const char *message, size_t length, OpCode opCode) {

#ifdef UWS_THREADSAFE
    std::lock_guard<std::recursive_mutex> lockGuard(*asyncMutex);
#endif

    typename WebSocket<isServer>::PreparedMessage *preparedMessage = WebSocket<isServer>::prepareMessage((char *) message, length, opCode, false);
    forEach(WEBSOCKET, [preparedMessage](uS::Socket *socket) {
        static_cast<uWS::WebSocket<isServer> *>(socket)->sendPrepared(preparedMessage);
    });
    WebSocket<isServer>::finalizeMessage(preparedMessage);
}

template <bool isServer>
void Group<isServer>::terminate() {
    forEach(WEBSOCKET, [](uS::Socket *socket) {
        static_cast<uWS::WebSocket<isServer> *>(socket)->terminate();
    });
    stopListening();
}

template <bool isServer>
void Group<isServer>::close(int code, char *message, size_t length) {
    forEach(WEBSOCKET, [code, message, length](uS::Socket *socket) {
        static_cast<uWS::WebSocket<isServer> *>(socket)->close(code, message, length);
    });
    stopListening();
    if (timer) {
        timer->stop();
        timer->close();
    }
}

template struct Group<SERVER>;
template struct Group<CLIENT>;

}
