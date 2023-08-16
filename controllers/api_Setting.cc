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
        sub["deviceName"] = "-";
        sub["distanceSteel"] = 0;
        sub["distanceCamera"] = 0;
        sub["cameraAddress"] = "-";
    } else {
        Settings setting = settings[0];
        sub["deviceName"] = setting.getValueOfDeviceName();
        sub["distanceSteel"] = setting.getValueOfDistanceSteel();
        sub["distanceCamera"] = setting.getValueOfDistanceCamera();
        sub["cameraAddress"] = setting.getValueOfCameraAddress();
        sub["customParam"] = setting.getValueOfCustomParam();
        sub["deviceVersion"] = setting.getValueOfDeviceVersion();
        sub["algorithmVersion"] = setting.getValueOfAlgorithmVersion();
        sub["systemVersion"] = setting.getValueOfSystemversion();
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


    std::string device_name = obj->get("deviceName", "").asString();
    std::string distance_steel = obj->get("distanceSteel", "").asString();
    std::string distance_camera = obj->get("distanceCamera", "").asString();
    std::string camera_address = obj->get("cameraAddress", "").asString();
    std::string custom_param = obj->get("customParam", "").asString();
    std::string current_dateTime = obj->get("currentDateTime", "").asString();
    std::string device_version = obj->get("deviceVersion", "").asString();
    std::string algorithm_version = obj->get("algorithmVersion", "").asString();
    std::string system_version = obj->get("systemVersion", "").asString();

    LOG_INFO << current_dateTime;

    Mapper<Settings> mp(drogon::app().getDbClient());
    auto settings = mp.findAll();
    if (settings.empty()) {
        Settings setting;
        setting.setDeviceName(device_name);
        setting.setDistanceSteel(distance_steel);
        setting.setDistanceCamera(distance_camera);
        setting.setCameraAddress(camera_address);
        setting.setCustomParam(custom_param);
        setting.setDeviceVersion(device_version);
        setting.setAlgorithmVersion(algorithm_version);
        setting.setSystemversion(system_version);
        //todo: add update device date time

        setting.setUpdateTime(trantor::Date::now().toCustomedFormattedStringLocal("%Y-%m-%d %H:%M:%S"));
        mp.insert(setting);
    } else {
        Settings setting = settings[0];
        setting.setDeviceName(device_name);
        setting.setDistanceSteel(distance_steel);
        setting.setDistanceCamera(distance_camera);
        setting.setCameraAddress(camera_address);
        setting.setCustomParam(custom_param);
        setting.setDeviceVersion(device_version);
        setting.setAlgorithmVersion(algorithm_version);
        setting.setSystemversion(system_version);
        //todo: add update device date time
        setting.setUpdateTime(trantor::Date::now().toCustomedFormattedStringLocal("%Y-%m-%d %H:%M:%S"));
        mp.update(setting);
    }
    result["code"] = 0;
    result["data"] = {};
    result["msg"] = "update success";
    auto resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);
}


void Setting::restart_device(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    Json::Value result;
    result["code"] = 0;
    result["data"] = {};
    result["msg"] = "restart success";
    auto resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);;
}