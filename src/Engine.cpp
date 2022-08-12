#include "Engine.hpp"
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

namespace MsqLights {
    Engine::EmptyModifiable::EmptyModifiable(Engine* engine)
    : Modifiable(engine) {
    }

    Engine::Engine() 
    : emptyModifiable(this), oscServer(this), 
    random(std::chrono::system_clock::now().time_since_epoch().count()) {
        debug = false;
        selectedPage = 0;
        lastTime = std::chrono::steady_clock::now();
    }

    bool Engine::SendDmx(Engine* e, ola::client::OlaClientWrapper* wrapper) {
        e->dmxBuffer.Set(e->dmxValues, 512);
        wrapper->GetClient()->SendDMX(0, e->dmxBuffer, ola::client::SendDMXArgs());
        std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
        e->deltaTime = (now - e->lastTime).count();
        e->lastTime = now;
        e->Update();
        return true;
    }

    void Engine::DmxIO(Engine* e) {
        ola::io::SelectServer *ss = e->dmxClient.GetSelectServer();
        ss->RegisterRepeatingTimeout(25, ola::NewCallback(&Engine::SendDmx, e, &e->dmxClient));
        ss->Run();
    }

    void Engine::ConnectionAcceptor(Engine* e) {
	    int socket_desc, new_socket, c, *new_sock;
	    struct sockaddr_in server , client;
	    char *message;
	
	    //Create socket
	    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	    if (socket_desc == -1)
	    	std::cerr << "Could not create socket" << std::endl;
	
	    //Prepare the sockaddr_in structure
	    server.sin_family = AF_INET;
	    server.sin_addr.s_addr = INADDR_ANY;
	    server.sin_port = htons( 8888 );
	
	    //Bind
	    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	    	std::cerr << "Could not bind socket" << std::endl;

        listen(socket_desc, 20);

        std::cout << "Waiting for connections on 8888" << std::endl;
        c = sizeof(struct sockaddr_in);
        while((new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c))) {
            e->connectionsThreads.emplace_back(&Engine::ConnectionManager, e, new_socket);
        }
    }

    void Engine::ConnectionManager(Engine* e, int socket_desc) {
        //Get the socket descriptor
	    int sock = socket_desc;
	    int read_size;
	    char *message , client_message[2000];

        std::cout << "Client " << socket_desc << " connected" << std::endl;
	
	    //Send some messages to the client
	    message = "Greetings! I am your connection handler\n";
	    write(sock , message , strlen(message));
	
	    message = "Now type something and i shall repeat what you type \n";
	    write(sock , message , strlen(message));
	
	    //Receive a message from client
	    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
	    {
	    	//Send the message back to client
	    	write(sock , client_message , strlen(client_message));
	    }
	
	    if(read_size == 0)
	    {
	    	puts("Client disconnected");
	    	fflush(stdout);
	    }
	    else if(read_size == -1)
	    {
	    	perror("recv failed");
	    }
    }

    // REDO
    void Engine::Save() {
        rapidjson::Document d;
        d.SetObject();
        rapidjson::Value fixturesArray;
        fixturesArray.SetArray();
        rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
        for(unsigned int i = 0; i < fixtures.size(); i++)
            fixturesArray.PushBack(fixtures[i], allocator);
        d.AddMember("fixtures", fixturesArray, allocator);
        rapidjson::Value allTheModifiables;
        allTheModifiables.SetArray();
        for(auto &[key, val]: modifiables)
            allTheModifiables.PushBack(val->Serialize(allocator), allocator);
        
        d.AddMember("modifiers", allTheModifiables, allocator);

        rapidjson::StringBuffer sb;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
        std::ofstream file("data.json");
        d.Accept(writer);
        file << sb.GetString();
    }

    void Engine::Load() {
        fixtures.clear();

        std::ifstream file("data.json");
        std::stringstream ss;
        ss << file.rdbuf();
        rapidjson::Document d;
        d.Parse(ss.str().c_str());

        auto fixturesArray = d["fixtures"].GetArray();

        auto allTheModifiers = d["modifiers"].GetArray();
        for(unsigned int i = 0; i < allTheModifiers.Size(); i++) {
            auto page = allTheModifiers[i].GetArray(); 
            for(unsigned int j = 0; j < page.Size(); j++) {
                auto mod = page[j].GetObject();
                const char* typeStr = mod["type"].GetString();

                Modifier* modif = nullptr;
                if(!strcmp(typeStr, "Rectangle"))
                    modif = new GlobalModifier(this);
                else if(!strcmp(typeStr, "Spot"))
                    modif = new SpotModifier(this);
                modif->Init();
                modif->Load(page[j]);
            }
        }
    }

    void Engine::Init() {
        for (unsigned int i = 0; i < DMX_ADDRESSES; i++) 
            dmxValues[i] = 0;
        dmxClient.Setup();
        oscServer.Start();
    }

    void Engine::Update() {
        for (unsigned int i = 0; i < DMX_ADDRESSES; i++) 
            dmxValues[i] = 0;
 
        oscServer.Update();

        for(auto&[_,val]: modifiables)
            val->Update();
    }

    void Engine::Close() {
        oscServer.Close();
    }

    void Engine::Loop() {
        Init();
        dmxIOThread = std::thread(Engine::DmxIO, this);
        connectionAcceptorThread = std::thread(Engine::ConnectionAcceptor, this);
        while(true) {
            for(unsigned int i = 0; i < commandQueue.size(); i++) {
                ExecuteCommand(commandQueue.front());
                commandQueue.pop();
            }
        }
        Close();
    }

    void Engine::ExecuteCommand(std::string command) {

    }   

    float Engine::GetDeltaTime() {
        return deltaTime;
    }
}