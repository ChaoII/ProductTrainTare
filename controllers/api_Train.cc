#include "api_Train.h"
#include "models/History.h"
#include "models/Train.h"
#include "models/Picture.h"
#include "custom/utils.h"
#include <drogon/orm/Mapper.h>

using namespace api;
using namespace drogon::orm;
using Historys = drogon_model::sqlite3::History;
using Trains = drogon_model::sqlite3::Train;
using Picture = drogon_model::sqlite3::Picture;


void Train::get_coming_time(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    Json::Value result, root, history_list, sub;
    auto obj = req->getJsonObject();
    if (obj == nullptr) {
        result["code"] = -1;
        result["data"] = {};
        result["msg"] = "request params error";
        auto resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }
    std::string start_time = drogon::Custom::front_time_to_backend(obj->get("startTime", "").asString());
    std::string end_time = drogon::Custom::front_time_to_backend(obj->get("endTime", "").asString());
    auto clientPtr = drogon::app().getDbClient();
    clientPtr->execSqlAsync(
            R"(SELECT t.id AS trainId, t.coming_time, COUNT(h.train_id) AS count FROM train AS t
                    LEFT JOIN history AS h ON t.id = h.train_id WHERE t.coming_time > ? and t.coming_time <= ?
                    GROUP BY t.id, t.coming_time;)",
            [=](const drogon::orm::Result &results) {
                Json::Value result, root, history_list, sub;
                for (auto &row: results) {
                    sub["trainId"] = row["trainId"].as<std::string>();
                    sub["comingTime"] = row["coming_time"].as<std::string>();
                    sub["count"] = row["count"].as<int>();
                    history_list.append(sub);
                }
                root["history"] = history_list;
                result["code"] = 0;
                result["data"] = root;
                result["msg"] = "success";
                auto resp = HttpResponse::newHttpJsonResponse(result);
                callback(resp);
            },
            [=](const DrogonDbException &e) {
                Json::Value result, root, history_list, sub;
                LOG_ERROR << e.base().what();
                result["code"] = -1;
                result["data"] = {};
                result["msg"] = "request params error";
                auto resp = HttpResponse::newHttpJsonResponse(result);
                callback(resp);
            }, start_time, end_time);
}

void Train::get_history(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    Json::Value result, root, history_list, sub;
    auto obj = req->getJsonObject();
    if (obj == nullptr) {
        result["code"] = -1;
        result["data"] = {};
        result["msg"] = "request params error";
        auto resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }
    std::string train_id = obj->get("trainId", "").asString();
    LOG_INFO << train_id;
    int page_size = obj->get("pageSize", "").asInt();
    int page_index = obj->get("pageIndex", "").asInt();
    Mapper<Historys> mp(drogon::app().getDbClient());
    auto conditions = Criteria(Historys::Cols::_train_id, CompareOperator::Like, train_id);
    auto histories = mp.limit(page_size).offset(
            (page_index - 1) * page_size).findBy(conditions);
    size_t total = mp.count(conditions);
    for (auto &history: histories) {
        sub["id"] = history.getValueOfId();
        sub["dateTime"] = history.getValueOfTime();
        sub["trainMode"] = history.getValueOfTrainMode();
        sub["trainNum"] = history.getValueOfTrainNum();
        sub["deadWeight"] = history.getValueOfDeadWeight();
        sub["roughWeight"] = history.getValueOfRoughWeight();
        sub["volume"] = history.getValueOfVolume();
        sub["length"] = history.getValueOfLength();
        sub["imgUrl"] = history.getValueOfPicUrl();
        history_list.append(sub);
    }
    root["history"] = history_list;
    result["code"] = 0;
    root["total"] = total;
    result["data"] = root;
    result["msg"] = "success";
    auto resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);
}

void Train::get_latest(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    Mapper<Historys> mp(drogon::app().getDbClient());
    auto histories = mp.orderBy(Historys::Cols::_time, SortOrder::DESC).limit(1).findAll();
    Json::Value root, result, sub;
    if (histories.empty()) {
        result["code"] = 0;
        result["data"] = {};
        result["msg"] = "successful";
        auto resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }
    auto history = histories[0];
    sub["id"] = history.getValueOfId();
    sub["trainMode"] = history.getValueOfTrainMode();
    sub["trainNum"] = history.getValueOfTrainNum();
    sub["deadWeight"] = history.getValueOfDeadWeight();
    sub["roughWeight"] = history.getValueOfRoughWeight();
    sub["volume"] = history.getValueOfVolume();
    sub["length"] = history.getValueOfLength();
    sub["imgUrl"] = history.getValueOfPicUrl();
    result["code"] = 0;
    result["data"] = sub;
    result["msg"] = "success";
    auto resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);
}

void Train::get_curimg(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    Mapper<Picture> mp(drogon::app().getDbClient());
    auto pictures = mp.orderBy(Picture::Cols::_time, SortOrder::DESC).limit(1).findAll();
    Json::Value root, result, sub;
    if (pictures.empty()) {
        result["code"] = 0;
        result["data"] = {};
        result["msg"] = "successful";
        auto resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }
    auto picture = pictures[0];
    sub["id"] = picture.getValueOfId();
    sub["imgUrl"] = picture.getValueOfPicUrl();
    result["code"] = 0;
    result["data"] = sub;
    result["msg"] = "success";
    auto resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);
}