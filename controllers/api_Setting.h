#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
    class Setting : public drogon::HttpController<Setting> {
    public:
        METHOD_LIST_BEGIN
            ADD_METHOD_TO(Setting::get_settings, "/api/get_settings", Options, Get, "LoginFilter");
            ADD_METHOD_TO(Setting::update_settings, "/api/update_settings", Options, Post, "LoginFilter");
            ADD_METHOD_TO(Setting::restart_device, "/api/restart_device", Options, Get, "LoginFilter");
        METHOD_LIST_END

        void get_settings(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

        void update_settings(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

        void restart_device(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

    };
}
