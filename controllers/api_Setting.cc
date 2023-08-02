#include "api_Setting.h"
#include "models/Settings.h"
#include "drogon/orm/Mapper.h"

using namespace api;
using namespace drogon::orm;
using Settings = drogon_model::sqlite3::Settings;
// Add definition of your processing function here


void Setting::get_settings(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    Json::Value result, sub;
    Mapper<Settings> mp(drogon::app().getDbClient());
    auto settings = mp.findAll();
    if (settings.empty()) {
        sub["distanceSteel"] = 0;
        sub["distanceCamera"] = 0;
        sub["cameraAddress"] = "-";
    } else {
        Settings setting = settings[0];
        sub["distanceSteel"] = setting.getValueOfDistanceSteel();
        sub["distanceCamera"] = setting.getValueOfDistanceCamera();
        sub["cameraAddress"] = setting.getValueOfCameraAddress();
    }
    result["code"] = 0;
    result["data"] = sub;
    result["msg"] = "update success";
    auto resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);
}

void Setting::update_settings(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    Json::Value result, sub;
    auto obj = req->getJsonObject();
    if (obj == nullptr) {
        result["code"] = -1;
        result["data"] = {};
        result["msg"] = "request params error, must be {'username':'xx','password':'xx'}";
        auto resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }
    std::string distance_steel = obj->get("distanceSteel", "").asString();
    std::string distance_camera = obj->get("distanceCamera", "").asString();
    std::string camera_address = obj->get("cameraAddress", "").asString();
    Mapper<Settings> mp(drogon::app().getDbClient());
    auto settings = mp.findAll();
    if (settings.empty()) {
        Settings setting;
        setting.setDistanceSteel(distance_steel);
        setting.setDistanceCamera(distance_camera);
        setting.setCameraAddress(camera_address);
        setting.setUpdateTime(trantor::Date::now().toCustomedFormattedStringLocal("%Y-%m-%d %H:%M:%S"));
        mp.insert(setting);
    } else {
        Settings setting = settings[0];
        setting.setDistanceSteel(distance_steel);
        setting.setDistanceCamera(distance_camera);
        setting.setCameraAddress(camera_address);
        setting.setUpdateTime(trantor::Date::now().toCustomedFormattedStringLocal("%Y-%m-%d %H:%M:%S"));
        mp.update(setting);
    }
    result["code"] = 0;
    result["data"] = {};
    result["msg"] = "update success";
    auto resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);
}