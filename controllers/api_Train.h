#pragma once

#include <drogon/HttpController.h>
#include <drogon/drogon.h>

using namespace drogon;

namespace api {
    class Train : public drogon::HttpController<Train> {
    public:
        METHOD_LIST_BEGIN
            ADD_METHOD_TO(Train::get_coming_time, "/api/train/coming_time", Options, Post, "LoginFilter");
            ADD_METHOD_TO(Train::get_history, "/api/train/history", Options, Post, "LoginFilter");
            ADD_METHOD_TO(Train::get_latest, "/api/train/history/latest", Options, Get, "LoginFilter");
            ADD_METHOD_TO(Train::get_curimg, "/api/train/picture/latest", Options, Get, "LoginFilter");
        METHOD_LIST_END

        void get_coming_time(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

        void get_history(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

        void get_latest(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

        void get_curimg(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    };
}
