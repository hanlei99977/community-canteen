#pragma once
#include "../../third_party/httplib.h"
#include "../service/service.h"

class Controller {
public:
    static void initRoutes(httplib::Server& server);
};