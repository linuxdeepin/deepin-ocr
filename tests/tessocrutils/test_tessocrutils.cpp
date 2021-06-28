/*

* Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.

*

* Author:     wangcong <wangcong@uniontech.com>

*

* Maintainer: wangcong <wangcong@uniontech.com>

*

* This program is free software: you can redistribute it and/or modify

* it under the terms of the GNU General Public License as published by

* the Free Software Foundation, either version 3 of the License, or

* any later version.

*

* This program is distributed in the hope that it will be useful,

* but WITHOUT ANY WARRANTY; without even the implied warranty of

* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the

* GNU General Public License for more details.

*

* You should have received a copy of the GNU General Public License

* along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "../../src/tessocrutils/tessocrutils.h"
#include "stub.h"
#include "addr_pri.h"
#include <QDir>
#include <QStandardPaths>

#include <gtest/gtest.h>

class TessOcrUtilsTest:public testing::Test{

public:
    Stub stub;
    TessOcrUtils *m_tessOCrUtils = nullptr;
    virtual void SetUp() override{
        m_tessOCrUtils = TessOcrUtils::instance();
        std::cout << "start TessOcrUtilsTest" << std::endl;
    }

    virtual void TearDown() override{
        std::cout << "end TessOcrUtilsTest" << std::endl;
    }
};

ACCESS_PRIVATE_FUN(TessOcrUtils, bool(const QString), setLanguagesPath);
ACCESS_PRIVATE_FUN(TessOcrUtils, bool(ResultType), isExistsResultType);
ACCESS_PRIVATE_FUN(TessOcrUtils, QString(Languages), getLangStr);
ACCESS_PRIVATE_FUN(TessOcrUtils, Languages(), getSystemLang);
ACCESS_PRIVATE_FUN(TessOcrUtils, QString(), getLanguages);
ACCESS_PRIVATE_FUN(TessOcrUtils, RecognitionResult(Pix*, ResultType), getRecognizeResult);
ACCESS_PRIVATE_FUN(TessOcrUtils, void(ErrorCode, const QString,const ResultType,RecognitionResult &), setResult);



TEST_F(TessOcrUtilsTest, tessOcrUtils)
{
    EXPECT_NE(nullptr, m_tessOCrUtils);
}


TEST_F(TessOcrUtilsTest, getRecogitionResultImagePathAndResultType)
{
    //QString imagePath = "/media/wangcong/workspace/wangcong/workspace/qt_workspace/project/deepin-ocr/assets/testocr.png";
    QString imagePath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + "/Wallpapers/luca-micheli-ruWkmt3nU58-unsplash.jpg";
    ResultType resultType = ResultType::RESULT_STRING;
    RecognitionResult recognitionResult = m_tessOCrUtils->getRecogitionResult(imagePath,resultType);
    EXPECT_EQ(true, recognitionResult.flag);
    EXPECT_EQ("", recognitionResult.message);
    EXPECT_EQ(ErrorCode::OK, recognitionResult.errorCode);
    EXPECT_EQ(resultType, recognitionResult.resultType);
    //EXPECT_NE("", recognitionResult.result);
}

TEST_F(TessOcrUtilsTest, getRecogitionResult2ImagePath)
{
    QString imagePath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + "/Wallpapers/luca-micheli-ruWkmt3nU58-unsplash.jpg";
    RecognitionResult recognitionResult = m_tessOCrUtils->getRecogitionResult(imagePath);
    recognitionResult = m_tessOCrUtils->getRecogitionResult(imagePath);
    EXPECT_EQ(true, recognitionResult.flag);
    EXPECT_EQ("", recognitionResult.message);
    EXPECT_EQ(ErrorCode::OK, recognitionResult.errorCode);
    EXPECT_EQ(ResultType::RESULT_STRING, recognitionResult.resultType);
    //EXPECT_NE("", recognitionResult.result);
}

TEST_F(TessOcrUtilsTest, getRecogitionResultImageAndResultType)
{
    QString imagePath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + "/Wallpapers/luca-micheli-ruWkmt3nU58-unsplash.jpg";
    ResultType resultType = ResultType::RESULT_STRING;
    QImage* t_image = new QImage(imagePath);
    RecognitionResult recognitionResult = TessOcrUtils::instance()->getRecogitionResult(t_image,resultType);
    EXPECT_EQ(true, recognitionResult.flag);
    EXPECT_EQ("", recognitionResult.message);
    EXPECT_EQ(ErrorCode::OK, recognitionResult.errorCode);
    EXPECT_EQ(resultType, recognitionResult.resultType);
    //EXPECT_NE("", recognitionResult.result);
}

TEST_F(TessOcrUtilsTest, getRecogitionResultImage)
{
    QString imagePath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + "/Wallpapers/luca-micheli-ruWkmt3nU58-unsplash.jpg";
    QImage* t_image = new QImage(imagePath);
    RecognitionResult recognitionResult = TessOcrUtils::instance()->getRecogitionResult(t_image);
    EXPECT_EQ(true, recognitionResult.flag);
    EXPECT_EQ("", recognitionResult.message);
    EXPECT_EQ(ErrorCode::OK, recognitionResult.errorCode);
    EXPECT_EQ(ResultType::RESULT_STRING, recognitionResult.resultType);
    //EXPECT_NE("", recognitionResult.result);

}

TEST_F(TessOcrUtilsTest, isRunning)
{
    EXPECT_EQ(false, m_tessOCrUtils->isRunning());
    QString imagePath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + "/Wallpapers/luca-micheli-ruWkmt3nU58-unsplash.jpg";
    ResultType resultType = ResultType::RESULT_STRING;
    RecognitionResult recognitionResult = m_tessOCrUtils->getRecogitionResult(imagePath,resultType);
    EXPECT_EQ(false, m_tessOCrUtils->isRunning());
}

TEST_F(TessOcrUtilsTest, getRecognizeResult)
{
    QString imagePath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + "/Wallpapers/luca-micheli-ruWkmt3nU58-unsplash.jpg";
    QImage* image = new QImage(imagePath);
    ResultType resultType = ResultType::RESULT_STRING;
    Pix *p_image;
    p_image = pixCreate(image->width(), image->height(), image->depth());
    p_image->w = static_cast<l_uint32>(image->width());
    p_image->h = static_cast<l_uint32>(image->height());
    p_image->d = static_cast<l_uint32>(image->depth());
    p_image->spp = 3;
    p_image->wpl = static_cast<l_uint32>(image->width());
    p_image->refcount = 1;
    p_image->xres = 0;
    p_image->yres = 0;
    p_image->informat = 0;
    p_image->special = 0;
    p_image->text = nullptr;
    p_image->colormap = nullptr;
    p_image->data = reinterpret_cast<l_uint32*>(image->bits());
    RecognitionResult recognitionResult = call_private_fun::TessOcrUtilsgetRecognizeResult(*m_tessOCrUtils,p_image,resultType);
    EXPECT_EQ(true, recognitionResult.flag);
    EXPECT_EQ("", recognitionResult.message);
    EXPECT_EQ(ErrorCode::OK, recognitionResult.errorCode);
    EXPECT_EQ(ResultType::RESULT_STRING, recognitionResult.resultType);
}

TEST_F(TessOcrUtilsTest, setLanguagesPath)
{
    const QString TessDataPath = "/usr/share/deepin-ocr/tesslangs";
    bool flag = call_private_fun::TessOcrUtilssetLanguagesPath(*m_tessOCrUtils,TessDataPath);
    //EXPECT_EQ(true, flag);
}

TEST_F(TessOcrUtilsTest, isExistsResultType)
{
    const QString TessDataPath = "/usr/share/deepin-ocr/tesslangs";
    bool flag = call_private_fun::TessOcrUtilsisExistsResultType(*m_tessOCrUtils,ResultType::RESULT_STRING);
    //EXPECT_EQ(true, flag);
    flag = call_private_fun::TessOcrUtilsisExistsResultType(*m_tessOCrUtils,ResultType::RESULT_HTML);
    //EXPECT_EQ(true, flag);
    flag = call_private_fun::TessOcrUtilsisExistsResultType(*m_tessOCrUtils,ResultType::UNKNOWN_TYPE);
    //EXPECT_EQ(false, flag);
}

TEST_F(TessOcrUtilsTest, getLangStr)
{
    QString lang = call_private_fun::TessOcrUtilsgetLangStr(*m_tessOCrUtils,Languages::ENG);
    EXPECT_EQ("eng", lang);
    lang = call_private_fun::TessOcrUtilsgetLangStr(*m_tessOCrUtils,Languages::CHI_SIM);
    EXPECT_EQ("chi_sim", lang);
    lang = call_private_fun::TessOcrUtilsgetLangStr(*m_tessOCrUtils,Languages::CHI_TRA);
    EXPECT_EQ("chi_tra", lang);
}

TEST_F(TessOcrUtilsTest, getSystemLang)
{
    Languages lang = call_private_fun::TessOcrUtilsgetSystemLang(*m_tessOCrUtils);
    EXPECT_NE(Languages::UNKNOWN_LAN, lang);
}

TEST_F(TessOcrUtilsTest, getLanguages)
{
    QString lang = call_private_fun::TessOcrUtilsgetLanguages(*m_tessOCrUtils);
    EXPECT_NE("", lang);
}

TEST_F(TessOcrUtilsTest, setResult)
{
    RecognitionResult result ;
    call_private_fun::TessOcrUtilssetResult(*m_tessOCrUtils,ErrorCode::UNKNOWN,"test",ResultType::RESULT_STRING,result);
    EXPECT_EQ(false, result.flag);
    EXPECT_EQ(ErrorCode::UNKNOWN, result.errorCode);
    EXPECT_EQ("test", result.message);
    EXPECT_EQ(ResultType::RESULT_STRING, result.resultType);
    EXPECT_EQ("", result.result);
}
