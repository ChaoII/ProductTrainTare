/**
 *
 *  SeetaFace.h
 *
 */
#pragma once

#include <drogon/drogon.h>
#include <opencv2/opencv.hpp>
#include <drogon/plugins/Plugin.h>
#include "fastdeploy/vision.h"

struct TrainTareResult{
    uint64_t train_id;
    std::string filename;
    int flag;
    std::string text;
    std::string pic_url;
};


class PPOCR : public drogon::Plugin<PPOCR> {
public:
    PPOCR() {}

    /// This method must be called by drogon to initialize and start the plugin.
    /// It must be implemented by the user.
    void initAndStart(const Json::Value &config) override;

    /// This method must be called by drogon to shutdown the plugin.
    /// It must be implemented by the user.
    void shutdown() override;

    void predict(const cv::Mat &img, fastdeploy::vision::OCRResult *result);

    void on_rec();

    void train_ocr(const cv::Mat& img);

    void rec(const std::string& cur_time, const std::string& path);

private:

    std::vector<std::string> pic_suffixs_;
    std::string dir_name_;
    std::string last_timestamp_;
    TrainTareResult result_;
    std::vector<TrainTareResult> results_;
    std::string watch_dir_;
    std::string result_root_dir_;
    int time_interval_ = 0;
    int counter_=0;
    std::shared_ptr<fastdeploy::vision::ocr::DBDetector> det_model = nullptr;
    std::shared_ptr<fastdeploy::vision::ocr::Recognizer> rec_model = nullptr;
    std::shared_ptr<fastdeploy::pipeline::PPOCRv3> PPOCRv3 = nullptr;

};

