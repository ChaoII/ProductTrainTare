/**
 *
 *  SeetaFace.cc
 *
 */

#include "PPOCR.h"
#include "custom/utils.h"
#include <filesystem>
#include "models/Record.h"
#include "models/Picture.h"


#define sep "/"
namespace fs = std::filesystem;
using Records = drogon_model::sqlite3::Record;
using Pictures = drogon_model::sqlite3::Picture;
using namespace drogon::orm;

void PPOCR::initAndStart(const Json::Value &config) {

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
    last_timestamp_ = mp.findAll()[0].getValueOfLastTimestamp();

    time_interval_ = config.get("time_interval", 10).asInt();
    result_root_dir_ = config.get("result_root_dir", "").asString();
    watch_dir_ = config.get("watch_dir", "").asString();

    const Json::Value &array = config.get("pic_suffix", "");
    drogon::Custom::json_array_2_vector(array, pic_suffixs_);

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

     det_model = std::make_shared<fastdeploy::vision::ocr::DBDetector> (
            det_model_file, det_params_file, det_option);
     rec_model = std::make_shared<fastdeploy::vision::ocr::Recognizer>(
            rec_model_file, rec_params_file, rec_label_file, rec_option);

    assert(det_model.Initialized());
    assert(cls_model.Initialized());
    assert(rec_model.Initialized());

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

void PPOCR::on_rec() {
    Mapper<Records> mp(drogon::app().getDbClient());
    Records records = mp.findAll()[0];
    try {
        for (const auto &entry: fs::directory_iterator(watch_dir_)) {
            if (entry.is_regular_file()) {
                std::string suffix = drogon::utils::splitString(entry.path().string(), ".")[1];
                std::transform(suffix.begin(), suffix.end(), suffix.begin(), [](unsigned char c) {
                    return std::tolower(c);
                });

                auto it = std::find(pic_suffixs_.begin(), pic_suffixs_.end(), suffix);
                if (it == pic_suffixs_.end()) {
                    return;
                }
                auto cur_time = drogon::utils::splitString(entry.path().filename().string(), ".")[0];
                auto cur_date_time = drogon::Custom::format_date_time(cur_time);
                auto last_date_time = drogon::Custom::format_date_time(last_timestamp_);

                uint64_t time_interval = cur_date_time.secondsSinceEpoch() - last_date_time.secondsSinceEpoch();

                LOG_INFO << time_interval;

                if (time_interval > time_interval_) {
                    results_.clear();
                    counter_ = 0;
                    // 1.创建文件夹
                    fs::path result_root_dir(result_root_dir_);
                    fs::path cur_time_(cur_time.substr(0, 12));
                    fs::path dir_name = result_root_dir / cur_time_;
                    if (!fs::exists(dir_name)) {
                        fs::create_directory(dir_name);
                    }
                    //创建火车id
                    result_.train_id = drogon::Custom::get_uuid();
                    dir_name_ = dir_name.string();
                    // 持久化，避免意外退出后程序初始化异常
                    records.setDirName(dir_name_);
                    mp.update(records);
                }
                rec(cur_time, entry.path().string());
                last_timestamp_ = cur_time;
                records.setLastTimestamp(last_timestamp_);
                mp.update(records);
            }
        }
    } catch (const std::filesystem::filesystem_error &e) {
        std::cout << "Error accessing directory: " << e.what() << std::endl;
    }
}

void PPOCR::rec(const std::string &cur_time, const std::string &path) {

    cv::Mat img = cv::imread(path);
    train_ocr(img);
    int index = counter_ / 2;
    int rem = counter_ % 2;
    if (result_.flag != -1) {
        counter_++;
    } else {
        rem = 2;
    }
    std::string file_name = cur_time + "_" + std::to_string(index) + "_" + std::to_string(rem) + ".jpg";
    fs::path new_file_name = fs::path(dir_name_) / fs::path(file_name);
    fs::rename(path, new_file_name);
    result_.pic_url = new_file_name.string();
    Mapper<Pictures> pp(drogon::app().getDbClient());
    Pictures picture;
    picture.setId(drogon::Custom::get_uuid());
    picture.setPicUrl(result_.pic_url);
    picture.setType(result_.flag);
    picture.setTrainId(result_.train_id);
    pp.insert(picture);
    results_.push_back(result_);
}

void PPOCR::train_ocr(const cv::Mat &img) {
    fastdeploy::vision::OCRResult results;
    predict(img, &results);
    bool ch_ret = false;
    std::smatch match_ret;
    size_t size = results.boxes.size();
    for (size_t i; i < size; i++) {
        auto str = results.text[i];
        std::string re = "[\u4e00-\u9fa5]+";
        std::regex rule(re);
        auto r = regex_match(str, rule);
        ch_ret |= r;
    }
    if (ch_ret and size >= 5) {
        //皮重图片
        std::regex pattern("(\\D*)([1-4]{2}[.]?\\d?t)");
        std::string temp;
        for (size_t i; i < size; i++) {
            std::string text = results.text[i];
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
                        temp = res + "t";
                    }
                }
                catch (std::invalid_argument &e) {
                    fprintf(stderr, "%s\n", e.what());
                };
            }
        }
        result_.text = temp;
        result_.flag = 0;
    } else if ((ch_ret && results.boxes.empty()) || results.boxes.empty()) {
        //车头或者其它
        result_.flag = -1;
        result_.text = "";
    } else {
        std::regex pattern("(\\d{2}).?(\\d{5})");
        std::string temp;
        for (size_t i; i < size; i++) {
            std::string text = results.text[i];
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
}



