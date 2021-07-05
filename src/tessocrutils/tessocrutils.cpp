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
#include "tessocrutils.h"

/**
 * @brief 语言包路径
 */
const QString TESSDATA_PATH = "/usr/share/deepin-ocr/tesslangs";

/**
 * @brief TessOcrUtils::m_tessOcrUtils
 */
TessOcrUtils *TessOcrUtils::m_tessOcrUtils = nullptr;

/**
 * @brief TessOcrUtils::t_Tesseract 三方库实例化
 */
tesseract::TessBaseAPI *TessOcrUtils::t_Tesseract   = new tesseract::TessBaseAPI();


TessOcrUtils::TessOcrUtils()
{

    //设置语言包路径
    setLanguagesPath(TESSDATA_PATH);
    //设置当前默认使用的识别语言系统语言+英语
    m_sLangs = getLanguages();
    //是否正在识别中
    m_isRunning = false;
}

TessOcrUtils::~TessOcrUtils()
{

}

TessOcrUtils *TessOcrUtils::instance()
{
    if (!m_tessOcrUtils) {
        m_tessOcrUtils = new TessOcrUtils();
    }

    return m_tessOcrUtils;
}
//传入待识别图片的路径和想得到的返回结果类型，获取识别结果
RecognitionResult TessOcrUtils::getRecogitionResult(const QString &imagePath,const ResultType &resultType)
{
    m_isRunning = true;
    QString errorMessage = "";
    ErrorCode errorCode = ErrorCode::UNKNOWN;
    RecognitionResult t_result;
    Pix *p_image;

    if(imagePath.isNull() || imagePath.isEmpty())
    {
        //errorMesage = "不能传递空的图片路径！";
        errorMessage = "Can't pass an empty image path!";
        errorCode = ErrorCode::OCR_P_NULL;
        setResult(errorCode,errorMessage,resultType,t_result);
        m_isRunning = false;
        return t_result;
    }

    QFileInfo qFile(imagePath);
    //界面传递时最好做个限定，只能传图片
    if(!qFile.isFile())
    {
        //errorMesage = "图片不存在！imagePath: " + imagePath;
        errorMessage = "Image does not exist! imagePath: " + imagePath;
        errorCode = ErrorCode::OCR_P_NULL;
        setResult(errorCode,errorMessage,resultType,t_result);
        m_isRunning = false;
        return t_result;
    }
    try {
        //加载图片
         p_image = pixRead(imagePath.toLocal8Bit().data()); //absolute path of file
         if(p_image == nullptr)
         {
             throw new QException();
         }
    } catch (const std::logic_error &e) {
        //errorMesage = "加载图片失败！" + QString(qExc.what());
        errorMessage = "Failed to load picture! " + QString(e.what());
        errorCode = ErrorCode::OCR_LI_F;
        setResult(errorCode,errorMessage,resultType,t_result);
        m_isRunning = false;
        return t_result;
    }
    //获取识别结果
    return  getRecognizeResult(p_image,resultType);
}

//传入待识别图片的路径，获取纯字符串的识别结果
RecognitionResult TessOcrUtils::getRecogitionResult(const QString &imagePath)
{
    return getRecogitionResult(imagePath,ResultType::RESULT_STRING);
}

//传入待识别图片和想得到的返回结果类型，获取识别结果
RecognitionResult TessOcrUtils::getRecogitionResult(QImage *image, const ResultType &resultType)
{
    m_isRunning = true;
    QString errorMessage = "";
    ErrorCode errorCode = ErrorCode::UNKNOWN;
    RecognitionResult t_result;

    if(image->isNull())
    {
        //errorMesage = "不能传递空的图片路径！";
        errorMessage = "Can't pass an empty image!";
        errorCode = ErrorCode::OCR_P_NULL;
        setResult(errorCode,errorMessage,resultType,t_result);
        m_isRunning = false;
        return t_result;
    }
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
    //p_image->colormap->array;
    //p_image->colormap->depth;
    //p_image->colormap->nalloc;
    //p_image->colormap->n;
    p_image->data = reinterpret_cast<l_uint32*>(image->bits());
    //获取识别结果
    return  getRecognizeResult(p_image,resultType);
}

//传入待识别图片，获取纯字符串的识别结果
RecognitionResult TessOcrUtils::getRecogitionResult(QImage *image)
{
    return getRecogitionResult(image,ResultType::RESULT_STRING);
}

bool TessOcrUtils::isRunning()
{
    return m_isRunning;
}

//获取系统当前的语言
Languages TessOcrUtils::getSystemLang()
{
    QLocale locale;
    if(locale.language() == QLocale::Language::Chinese){
        //查询当前国别代码
        QLocale::Country t_countryId = locale.country();
        if(t_countryId == QLocale::Country::China){
            return Languages::CHI_SIM;
        }else {
            return Languages::CHI_TRA;
        }
    }else {
        return Languages::ENG;
    }
}

//获取识别需要的使用的语言包
QString TessOcrUtils::getLanguages()
{
    QString t_langs = "";

    //当前系统语言
    Languages t_systemLang = getSystemLang();

    //当前系统语言如果不为中文，而是任何国家的语言则只返回英文
    if(t_systemLang == Languages::ENG)
    {
        t_langs=getLangStr(t_systemLang);
    }else{
        t_langs=getLangStr(t_systemLang)+"+"+getLangStr(Languages::ENG);
    }
    return  t_langs;
}

//设置识别需要的使用的语言包的路径
bool TessOcrUtils::setLanguagesPath(const QString langsPath)
{
    if(langsPath.isNull() || langsPath.isEmpty())
    {
        return false;
    }
    m_sTessdataPath = langsPath;
    return true;
}

//获取识别结果
RecognitionResult TessOcrUtils::getRecognizeResult(Pix * image,ResultType resultType)
{
    QString errorMessage = "";
    ErrorCode errorCode = ErrorCode::UNKNOWN;
    QString result = "";
    RecognitionResult t_result;
    if(!isExistsResultType(resultType))
    {
        //errorMesage = "结果类型不存在 resultType: " + QString::number(resultType);
        errorMessage = "The result type does not exist! resultType: " + QString::number(resultType);
        errorCode = ErrorCode::OCR_RT_NULL;
        setResult(errorCode,errorMessage,resultType,t_result);
        m_isRunning = false;
        return t_result;
    }
    //最新的tesseract库可以new出来，老版本的库无法在appcation后new出来
    //tesseract::TessBaseAPI *t_Tesseract = new tesseract::TessBaseAPI();
    try{
        //初始化语言包
        if (t_Tesseract->Init(m_sTessdataPath.toLatin1().data(), m_sLangs.toLatin1().data()))
        {
            //errorMesage = "初始化 Tesseract 失败！" + QString::number(resultType);
            errorMessage = "Could not initialize tesseract! Tesseract couldn't load any languages!";
            errorCode = ErrorCode::OCR_INI_F;
            setResult(errorCode,errorMessage,resultType,t_result);
            m_isRunning = false;
            return t_result;
        }
    }catch(const std::logic_error &e){
        //errorMesage = "初始化 Tesseract 失败！Tesseract 内部错误！" + QString(qExc.what());
        errorMessage = "Could not initialize tesseract! Tesseract internal error! " + QString(e.what());
        errorCode = ErrorCode::OCR_INI_F;
        setResult(errorCode,errorMessage,resultType,t_result);
        m_isRunning = false;
        return t_result;
    }
    //为Tesseract设置待识别图片
    t_Tesseract->SetImage(image);

    try {
        //识别图片
        switch (resultType) {
        case ResultType::RESULT_HTML:
            result = QString(t_Tesseract->GetHOCRText(0));
            break;
        case ResultType::RESULT_STRING:
            result = QString(t_Tesseract->GetUTF8Text());
            break;
        default:
            result = QString(t_Tesseract->GetUTF8Text());
            break;
        }
    } catch (const std::logic_error &e) {
        //errorMesage = "识别图片失败！" + QString(qExc.what());
        errorMessage = "Image recognition failed! " + QString(e.what());
        errorCode = ErrorCode::OCR_LI_F;
        setResult(errorCode,errorMessage,resultType,t_result);
        m_isRunning = false;
        return t_result;
    }
    t_Tesseract->End();
    pixDestroy(&image);
    t_result.flag = true;
    t_result.message = errorMessage;
    t_result.errorCode = ErrorCode::OK;
    t_result.resultType = resultType;
    t_result.result = result;
    m_isRunning = false;
    return t_result;
}

//设置返回结果,内部使用
void TessOcrUtils::setResult(ErrorCode errCode, const QString errMessage,const ResultType resultType,RecognitionResult &result)
{
    result.flag = false;
    result.message = errMessage;
    result.errorCode = errCode;
    result.resultType = resultType;
    result.result = "";
}

//设置当前使用的语言包
//bool TessOcrUtils::setLanguages(const QList<Languages> langs)
//{
//    QString t_langs = "";
//    if(langs.isEmpty())
//    {
//        return false;
//    }
//    int i;
//    for ( i = 0;i<langs.length();i++) {
//        Languages lang = langs[i];
//        if(!isExistsLanguage(lang))
//        {
//            return false;
//        }
//        if((langs.length()-1) == i){
//            t_langs+=getLangStr(lang);
//        }else{
//            t_langs+=getLangStr(lang) + "+";
//        }
//    }
//    m_sLangs = t_langs;
//    return  true;
//}

//根据传入的枚举，获取相应的语言包字符串
QString TessOcrUtils::getLangStr(Languages lang)
{
    QString langStr = "";
    switch (lang)
    {
    case Languages::CHI_SIM:
        langStr = "chi_sim";
        break;
    case Languages::CHI_TRA:
        langStr = "chi_tra";
        break;
    case Languages::ENG:
        langStr = "eng";
        break;
    default:
        langStr = "eng";
        break;
    }
    return langStr;
}

//判断指定的结果类型是否存在
bool TessOcrUtils::isExistsResultType(ResultType resultType)
{
    bool flag = false;
    switch (resultType)
    {
    case ResultType::RESULT_STRING:
        flag = true;
        break;
    case ResultType::RESULT_HTML:
        flag = true;
        break;
    default:
        flag = false;
    }
    return  flag;
}

//判断指定的语言包类型是否存在 （尚未使用，后面需支持多种语言时可以考虑是否启用）
//bool TessOcrUtils::isExistsLanguage(Languages lang)
//{
//    bool flag = false;
//    QString str = getLangStr(lang);
//    if(!str.isNull() && !str.isEmpty()){
//        flag = true;
//    }
//    return  flag;
//}

