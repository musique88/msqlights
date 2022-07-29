#include "OscServer.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <cstdio>

extern "C" {
    #include "tinyosc/tinyosc.h"
}
#include <string>
#include "Engine.hpp"

namespace MsqLights {
    const std::string ADDRESS("msqlights");

    std::vector<std::string> SeparateAddresses(tosc_message osc) {
        std::vector<std::string> arr;
        std::string addr = osc.buffer;
        addr = addr.substr(1, addr.size() - 1);

        while(!addr.empty()) {
            int pos = addr.find('/');
            arr.push_back(addr.substr(0, pos));
            addr = addr.substr(pos + 1, addr.size() - pos);
            if(pos == std::string::npos)
                break;
        }

        return arr;
    }

    void DealWithOsc(Engine* e, tosc_message osc) {
        auto addr = SeparateAddresses(osc);
        
        if(addr.size() != 3)
            return;
        if (addr[0].compare(0, ADDRESS.length(), ADDRESS))
            return;
        
        std::string modifname = addr[1];
        std::vector<Modifier*>* mods = e->GetModifiers();
        for (unsigned int i = 0; i < mods->size(); i++){
            Modifier* mod = mods->at(i);
            if (mod->name_ != modifname)
                continue;
            switch(tosc_getFormat(&osc)[0]) {
                case 'f':
                    mod->SetParam(addr[2], tosc_getNextFloat(&osc));
                    break;
                case 'i':
                    mod->SetParam(addr[2], tosc_getNextInt32(&osc));
                    break;
            }
            return;
        }
    }

    OscServer::OscServer(Engine* e) {
        engine_ = e;
    }

    void OscServer::Start() {
        // open a socket to listen for datagrams (i.e. UDP packets) on port 9000
        fd_ = socket(AF_INET, SOCK_DGRAM, 0);
        fcntl(fd_, F_SETFL, O_NONBLOCK); // set the socket to non-blocking
        struct sockaddr_in sin;
        sin.sin_family = AF_INET;
        sin.sin_port = htons(9000);
        sin.sin_addr.s_addr = INADDR_ANY;
        bind(fd_, (struct sockaddr *) &sin, sizeof(struct sockaddr_in));
        printf("MsqLights is now listening OSC on port 9000 at address /msqlights.\n");
    }

    void OscServer::Update() {
        tosc_message osc;

        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(fd_, &readSet);
        struct timeval timeout = {0, 1000}; // select times out after 1 second
        if (select(fd_+1, &readSet, NULL, NULL, &timeout) > 0) {
            struct sockaddr sa; // can be safely cast to sockaddr_in
            socklen_t sa_len = sizeof(struct sockaddr_in);
            int len = 0;
            while ((len = (int) recvfrom(fd_, buffer_, sizeof(buffer_), 0, &sa, &sa_len)) > 0) {
                if (tosc_isBundle(buffer_)) {
                    tosc_bundle bundle;
                    tosc_parseBundle(&bundle, buffer_, len);
                    while (tosc_getNextMessage(&bundle, &osc)) {
                        tosc_message printable = osc;
                        tosc_printMessage(&printable);
                        DealWithOsc(engine_, osc);
                    }
                } else {
                    tosc_parseMessage(&osc, buffer_, len);
                    tosc_message printable = osc;
                    tosc_printMessage(&printable);
                    DealWithOsc(engine_, osc);
                }
            }
        }
    }

    void OscServer::Close() {
        close(fd_);
    }
}