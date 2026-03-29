#include <iostream>
#include "../third_party/httplib.h"
#include "controller/controller.h"

int main() {
    httplib::Server server;

    Controller::initRoutes(server);

    std::cout << "Server started at http://0.0.0.0:8080" << std::endl;

    server.listen("0.0.0.0", 8080);

    return 0;
}