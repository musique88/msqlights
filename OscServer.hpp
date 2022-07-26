#pragma once

namespace MsqLights {
    class Engine;
    class OscServer {
        int fd_;
        Engine* engine_;
        char buffer_[2048];
    public:
        OscServer(Engine* e);
        void Start();
        void Update();
        void Close();
    };
}