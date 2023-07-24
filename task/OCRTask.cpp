//
// Created by aichao on 2023/6/18.
//

#include "OCRTask.h"

#include <drogon/drogon.h>
using namespace drogon;

void OCRTask::ocr_event() {

    drogon::app().getPlugin<PPOCR>()->on_rec();
}

