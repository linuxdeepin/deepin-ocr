// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <vector>
#include <string>
#include <postprocess_op.h>
#include <utility.h>

namespace ncnn {
class Net;
}

class Details
{
public:
    Details(const char *recParamPath, const char *recBinPath, const std::vector<std::string> &dict, int recOut);
    ~Details();

    std::vector<std::string> run(const cv::Mat matrix);

private:
    std::vector<std::string> recognizeTexts(const std::vector<cv::Mat> &detectImg);
    std::string ctcDecode(const std::vector<float> &recNetOutputData, int h, int w);
    std::vector<std::vector<std::vector<int> > > detectText(const cv::Mat &src, float thresh, float boxThresh, float unclipRatio);

    ncnn::Net *detNet; //检测网络
    ncnn::Net *recNet; //识别网络
    std::vector<std::string> keys; //字典
    int recOutIndex; //识别结果输出位置，默认的MobileNetV3模型出来的都是146

    PaddleOCR::PostProcessor postProcessor;
    PaddleOCR::Utility utilityTool;
};
