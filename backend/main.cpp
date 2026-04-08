#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <thread>
#include "../third_party/httplib.h"
#include "controller/controller.h"

namespace {
size_t resolveWorkerCount() {
    constexpr size_t kDefaultWorkerCount = 8;
    constexpr size_t kMaxWorkerCount = 64;

    size_t workers = std::thread::hardware_concurrency();
    if (workers == 0) {
        workers = kDefaultWorkerCount;
    }

    if (const char* env = std::getenv("CANTEEN_HTTP_WORKERS")) {
        try {
            workers = std::stoul(env);
        } catch (...) {
            std::cerr << "[WARN] invalid CANTEEN_HTTP_WORKERS=" << env
                      << ", fallback to " << workers << std::endl;
        }
    }

    workers = std::max<size_t>(1, std::min(workers, kMaxWorkerCount));
    return workers;
}
} // namespace

int main() {
    httplib::Server server;
    const auto workers = resolveWorkerCount();

    server.new_task_queue = [workers] {
        return new httplib::ThreadPool(workers);
    };

    Controller::initRoutes(server);

    std::cout << "Server started at http://0.0.0.0:8080"
              << ", worker threads: " << workers << std::endl;

    server.listen("0.0.0.0", 8080);

    return 0;
}