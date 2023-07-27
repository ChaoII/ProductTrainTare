#include <chrono>
#include <drogon/drogon.h>
#include "task/OCRTask.h"
#include <workspace/workspace.h>

using namespace drogon;
using namespace std::chrono_literals;


int main() {
    wsp::workbranch br(1);
    drogon::app().registerPostHandlingAdvice([](const drogon::HttpRequestPtr &, const drogon::HttpResponsePtr &resp) {
        resp->addHeader("Access-Control-Allow-Origin", "*");
    });
    // Load config file
    drogon::app().loadConfigFile("../config.json");
    // 定时任务，每60s钟执行更新系统时间
    drogon::app().getLoop()->runAfter(5s, [&]() {
        br.submit(OCRTask::ocr_event);
    });
    drogon::app().run();
    return 0;
}
