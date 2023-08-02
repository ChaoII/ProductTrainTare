

#include <filesystem>
#include "PPOCR.h"
#include "custom/utils.h"
#include "models/Picture.h"
#include "models/Train.h"

#define sep "/"
namespace fs = std::filesystem;

using Pictures = drogon_model::sqlite3::Picture;
using Trains = drogon_model::sqlite3::Train;
using namespace drogon::orm;

void PPOCR::initAndStart(const Json::Value &config) {

    // 加载配置参数
    std::string det_model_dir = config.get("det_model_dir", "").asString();
    std::string rec_model_dir = config.get("rec_model_dir", "").asString();
    std::string rec_label_file = config.get("rec_label_file", "").asString();

    int max_side_len = config.get("max_side_len", "").asInt();
    double det_db_thresh = config.get("det_db_thresh", "").asDouble();
    double det_db_box_thresh = config.get("det_db_box_thresh", "").asDouble();
    double det_db_unclip_ratio = config.get("det_db_unclip_ratio", "").asDouble();
    std::string det_db_score_mode = config.get("det_db_score_mode", "").asString();
    int use_dilation = config.get("use_dilation", "").asInt();
    int global_thread_num = config.get("global_thread_num", 1).asInt();

    Mapper<Records> mp(drogon::app().getDbClient());
    auto records = mp.findAll();
    if (records.empty()) {
        records_.setId(drogon::Custom::get_uuid());
        records_.setLastTimestamp("20220611073615283");
        records_.setDirName("dst\\202206150736");
        mp.insert(records_);
    } else {
        records_ = records[0];
    }
    last_timestamp_ = records_.getValueOfLastTimestamp();
    dir_name_ = records_.getValueOfDirName();

    time_interval_ = config.get("time_interval", 10).asInt();
    result_root_dir_ = config.get("result_root_dir", "").asString();
    watch_dir_ = config.get("watch_dir", "").asString();

    const Json::Value &array = config.get("pic_suffix", "");
    drogon::Custom::json_array_2_vector(array, pic_suffixs_);

    // 创建目标图片目录
    if (!fs::exists(result_root_dir_)) {
        fs::create_directory(result_root_dir_);
    }

    auto det_model_file = det_model_dir + sep + "inference.pdmodel";
    auto det_params_file = det_model_dir + sep + "inference.pdiparams";

    auto rec_model_file = rec_model_dir + sep + "inference.pdmodel";
    auto rec_params_file = rec_model_dir + sep + "inference.pdiparams";

    fastdeploy::RuntimeOption option;
    option.UseCpu();
    option.UseOrtBackend();
    option.SetCpuThreadNum(global_thread_num);

    auto det_option = option;
    auto rec_option = option;

    det_model = std::make_shared<fastdeploy::vision::ocr::DBDetector>(
            det_model_file, det_params_file, det_option);
    rec_model = std::make_shared<fastdeploy::vision::ocr::Recognizer>(
            rec_model_file, rec_params_file, rec_label_file, rec_option);

    assert(det_model->Initialized());
    assert(rec_model->Initialized());

    det_model->GetPreprocessor().SetMaxSideLen(max_side_len);
    det_model->GetPostprocessor().SetDetDBThresh(det_db_thresh);
    det_model->GetPostprocessor().SetDetDBBoxThresh(det_db_box_thresh);
    det_model->GetPostprocessor().SetDetDBUnclipRatio(det_db_unclip_ratio);
    det_model->GetPostprocessor().SetDetDBScoreMode(det_db_score_mode);
    det_model->GetPostprocessor().SetUseDilation(use_dilation);

    PPOCRv3 = std::make_shared<fastdeploy::pipeline::PPOCRv3>(det_model.get(), rec_model.get());

    if (!PPOCRv3->Initialized()) {
        std::cerr << "Failed to initialize PP-OCR." << std::endl;
        return;
    }
}

void PPOCR::shutdown() {
    /// Shutdown the plugin
}

void PPOCR::predict(const cv::Mat &img, fastdeploy::vision::OCRResult *result) {

    PPOCRv3->Predict(img, result);
}

void PPOCR::train_recognition_event() {
    Mapper<Records> mp(drogon::app().getDbClient());
    for (const auto &entry: fs::directory_iterator(watch_dir_)) {
        if (entry.is_regular_file()) {
            std::string suffix = drogon::utils::splitString(entry.path().string(), ".")[1];
            drogon::Custom::tolower_string(suffix);
            if (!drogon::Custom::target_in_vector<std::string>(pic_suffixs_, suffix)) {
                return;
            }
            auto cur_time = drogon::utils::splitString(entry.path().filename().string(), ".")[0];
            auto cur_date_time = drogon::Custom::format_date_time(cur_time);
            auto last_date_time = drogon::Custom::format_date_time(last_timestamp_);
            uint64_t time_interval = cur_date_time.secondsSinceEpoch() - last_date_time.secondsSinceEpoch();

            if (time_interval > time_interval_) {
                // 1.创建文件夹
                fs::path result_root_dir(result_root_dir_);
                fs::path cur_time_(cur_time.substr(0, 12));
                fs::path dir_name = result_root_dir / cur_time_;
                if (!fs::exists(dir_name)) {
                    fs::create_directory(dir_name);
                    fs::create_directory(dir_name / fs::path("result"));
                }
                // 记录当前火车所有图片存放的目录
                dir_name_ = dir_name.string();
                //创建火车id并新增一辆火车记录，后面的车厢全部归为该火车
                result_.train_id = drogon::Custom::get_uuid();
                auto clientPtr = drogon::app().getDbClient();
                clientPtr->execSqlAsync("INSERT INTO train VALUES(?,?);",
                                        [](const drogon::orm::Result &result) {},
                                        [](const DrogonDbException &e) {
                                            LOG_ERROR << e.base().what();
                                        },
                                        result_.train_id,
                                        cur_date_time.toCustomedFormattedStringLocal(
                                                "%Y-%m-%d %H:%M:%S"));

                // 持久化，避免意外退出后程序初始化异常
                records_.setDirName(dir_name_);
                mp.update(records_);
            }
            train_ocr(cur_time, entry.path().string());
            last_timestamp_ = cur_time;
            records_.setLastTimestamp(last_timestamp_);
            mp.update(records_);
        }
    }
}

void PPOCR::train_ocr(const std::string &cur_time, const std::string &path) {
    cv::Mat img = cv::imread(path);
    match_result(img);
//    if (result_.flag == -1) return;
    std::string file_name = cur_time + ".jpg";
    fs::path new_file_name = fs::path(dir_name_) / fs::path(file_name);
    fs::rename(path, new_file_name);
    std::string cur_time_ = drogon::Custom::format_date_time(cur_time).toCustomedFormattedStringLocal(
            "%Y-%m-%d %H:%M:%S");
    postprocess(img, cur_time);
    result_.pic_url = new_file_name.string();
    {
        auto clientPtr = drogon::app().getDbClient();
        clientPtr->execSqlAsync("INSERT INTO picture VALUES (?, ?, ?, ?, ?, ?);",
                                [](const drogon::orm::Result &result) {},
                                [](const DrogonDbException &e) {
                                    LOG_ERROR << e.base().what();
                                }, drogon::Custom::get_uuid(), result_.pic_url, result_.train_id,
                                result_.flag, result_.text, cur_time_);
    }
}

void PPOCR::match_result(const cv::Mat &img) {
    fastdeploy::vision::OCRResult results;
    predict(img, &results);
    bool has_chinese = check_chinese_flag(results);
    LOG_INFO << "has_chinese: " << has_chinese << "\tbox size: " << results.text.size();
    if (has_chinese and results.text.size() >= 5) {
        //皮重图片
        match_weight(results);
    } else if ((has_chinese && results.boxes.size() < 5) || results.boxes.empty()) {
        //车头或者其它
        result_.flag = -1;
        result_.text = "";
    } else {
        // 车号图片
        match_train_num(results);
    }
}

bool PPOCR::check_chinese_flag(const fastdeploy::vision::OCRResult &results) {
    bool chinese_flag = false;
    for (size_t i = 0; i < results.boxes.size(); i++) {
        auto str = results.text[i];
        std::regex pattern("[\u4e00-\u9fa5]+");
        auto has_chinese = regex_search(str, pattern);
        chinese_flag |= has_chinese;
    }
    return chinese_flag;
}

void PPOCR::match_weight(const fastdeploy::vision::OCRResult &results) {
    std::regex pattern(R"((\D*)([1-4]\d[.]?\d?)t)");
    std::string temp;
    std::smatch match_ret;
    for (size_t i = 0; i < results.boxes.size(); i++) {
        std::string text = results.text[i];
        LOG_INFO << text;
        std::string::const_iterator iterStart = text.begin();
        std::string::const_iterator iterEnd = text.end();
        regex_search(iterStart, iterEnd, match_ret, pattern);
        if (!match_ret.empty()) {
            temp = match_ret[2].str();
            try {
                float data = std::stof(temp);
                if (data > 100) {
                    data = data / 10.0f;
                    auto res = drogon::utils::formattedString("%.1f", data);
                    temp = res;
                }
            }
            catch (std::invalid_argument &e) {
                LOG_ERROR << e.what();
            }
        }
    }
    result_.text = temp;
    result_.flag = 0;
}

void PPOCR::match_train_num(const fastdeploy::vision::OCRResult &results) {
    std::regex pattern("(\\d{2}).?(\\d{5})");
    std::string temp;
    std::smatch match_ret;
    for (size_t i = 0; i < results.boxes.size(); i++) {
        std::string text = results.text[i];
        LOG_INFO << text;
        std::string::const_iterator iterStart = text.begin();
        std::string::const_iterator iterEnd = text.end();
        regex_search(iterStart, iterEnd, match_ret, pattern);
        if (!match_ret.empty()) {
            temp = match_ret[1].str() + match_ret[2].str();
        }
    }
    result_.text = temp;
    result_.flag = 1;
}

void PPOCR::postprocess(const cv::Mat &img, const std::string &cur_time) {
    auto clientPtr = drogon::app().getDbClient();
    int cur_case_index = count_ / 2;
    if (last_case_index_ == cur_case_index) {
        cv::Mat result_img = drogon::Custom::concat_mat_horizontal(last_image_, img);
        std::string result_file_name = cur_time + "_" + std::to_string(cur_case_index) + ".jpg";
        fs::path result_path = fs::path(dir_name_) / fs::path("result") / result_file_name;
        cv::imwrite(result_path.string(), result_img);
        clientPtr->execSqlAsync(
                "INSERT INTO history VALUES (?, ?, ?, ?, ?,?, ?, ?, ?, ?);",
                [](const drogon::orm::Result &result) {},
                [](const DrogonDbException &e) {
                    LOG_ERROR << e.base().what();
                },
                drogon::Custom::get_uuid(), drogon::Custom::format_date_time(cur_time).toCustomedFormattedStringLocal(
                        "%Y-%m-%d %H:%M:%S"), "-", result_.text, last_result_.text, 0.0, 0.0, 0.0,
                result_path.string(), result_.train_id);
    }
    last_result_ = result_;
    last_case_index_ = cur_case_index;
    last_image_ = img;
    if (result_.flag != -1) {
        count_++;
    }

}



