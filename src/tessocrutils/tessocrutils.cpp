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

TessOcrUtils::TessOcrUtils()
{


}
TessOcrUtils *TessOcrUtils::m_tessOcrUtils = nullptr;

TessOcrUtils *TessOcrUtils::instance()
{
    if (!m_tessOcrUtils) {
        m_tessOcrUtils = new TessOcrUtils();
    }

    return m_tessOcrUtils;
}

RecognitionResult TessOcrUtils::getRecogitionResult(const QString imagePath,const ResultType resultType)
{
    QString errorMessage = "";
    ErrorCode errorCode = ErrorCode::UKNOW;
    RecognitionResult t_result;
    Pix *p_image;

    if(imagePath.isNull() || imagePath.isEmpty())
    {
        //errorMesage = "不能传递空的图片路径！";
        errorMessage = "Can't pass an empty image path!";
        errorCode = ErrorCode::OCRPNULL;
        setResult(errorCode,errorMessage,resultType,t_result);
        return t_result;
    }

    QFileInfo qFile(imagePath);
    //界面传递时最好做个限定，只能传图片
    if(!qFile.isFile())
    {
        //errorMesage = "图片不存在！imagePath: " + imagePath;
        errorMessage = "Image does not exist! imagePath: " + imagePath;
        errorCode = ErrorCode::OCRPNULL;
        setResult(errorCode,errorMessage,resultType,t_result);
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
        errorCode = ErrorCode::OCRLIF;
        setResult(errorCode,errorMessage,resultType,t_result);
        return t_result;
    }
    //获取识别结果
    return  getRecogitionResult(p_image,resultType);
}

RecognitionResult TessOcrUtils::getRecogitionResult(const QString imagePath)
{
    return getRecogitionResult(imagePath,ResultType::ResultString);
}

RecognitionResult TessOcrUtils::getRecogitionResult(QImage *image, const ResultType resultType)
{
    QString errorMessage = "";
    ErrorCode errorCode = ErrorCode::UKNOW;
    RecognitionResult t_result;

    if(image->isNull())
    {
        //errorMesage = "不能传递空的图片路径！";
        errorMessage = "Can't pass an empty image!";
        errorCode = ErrorCode::OCRPNULL;
        setResult(errorCode,errorMessage,resultType,t_result);
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
    return  getRecogitionResult(p_image,resultType);
}

RecognitionResult TessOcrUtils::getRecogitionResult(QImage *image)
{
    return getRecogitionResult(image,ResultType::ResultString);
}

bool TessOcrUtils::setLanguagesPath(const QString langsPath)
{
    if(langsPath.isNull() || langsPath.isEmpty())
    {
        return false;
    }
    m_sTessdataPath = langsPath;
    return true;
}

RecognitionResult TessOcrUtils::getRecogitionResult(Pix * image,ResultType resultType)
{
    QString errorMessage = "";
    ErrorCode errorCode = ErrorCode::UKNOW;
    QString result = "";
    RecognitionResult t_result;
    if(!isExistsResultType(resultType))
    {
        //errorMesage = "结果类型不存在 resultType: " + QString::number(resultType);
        errorMessage = "The result type does not exist! resultType: " + QString::number(resultType);
        errorCode = ErrorCode::OCRRTNULL;
        setResult(errorCode,errorMessage,resultType,t_result);
        return t_result;
    }

    tesseract::TessBaseAPI *t_Tesseract = new tesseract::TessBaseAPI();


    try{
        //实例化tesseract
        t_Tesseract = new tesseract::TessBaseAPI();
        //初始化语言包
        if (t_Tesseract->Init(m_sTessdataPath.toLatin1().data(), m_sLangs.toLatin1().data()))
        {
            //errorMesage = "初始化 Tesseract 失败！" + QString::number(resultType);
            errorMessage = "Could not initialize tesseract! Tesseract couldn't load any languages!";
            errorCode = ErrorCode::OCRINIF;
            setResult(errorCode,errorMessage,resultType,t_result);
            return t_result;
        }
    }catch(const std::logic_error &e){
        //errorMesage = "初始化 Tesseract 失败！Tesseract 内部错误！" + QString(qExc.what());
        errorMessage = "Could not initialize tesseract! Tesseract internal error! " + QString(e.what());
        errorCode = ErrorCode::OCRINIF;
        setResult(errorCode,errorMessage,resultType,t_result);
        return t_result;
    }

    //为Tesseract设置待识别图片
    t_Tesseract->SetImage(image);

    try {
        //识别图片
        switch (resultType) {
        case ResultType::ResultXML:
            //result = QString(t_Tesseract->GetAltoText(0));
            break;
        case ResultType::ResultHTML:
            result = QString(t_Tesseract->GetHOCRText(0));
            break;
        case ResultType::ResultString:
            result = QString(t_Tesseract->GetUTF8Text());
            break;
        }
    } catch (const std::logic_error &e) {
        //errorMesage = "识别图片失败！" + QString(qExc.what());
        errorMessage = "Image recognition failed! " + QString(e.what());
        errorCode = ErrorCode::OCRLIF;
        setResult(errorCode,errorMessage,resultType,t_result);
        return t_result;
    }
    t_Tesseract->End();
    delete t_Tesseract;
    pixDestroy(&image);

    t_result.flag = true;
    t_result.message = errorMessage;
    t_result.errorCode = ErrorCode::OK;
    t_result.resultType = resultType;
    t_result.result = result;
    return t_result;
}

void TessOcrUtils::setResult(ErrorCode errCode, const QString errMessage,const ResultType resultType,RecognitionResult &result)
{

    result.flag = false;
    result.message = errMessage;
    result.errorCode = errCode;
    result.resultType = resultType;
    result.result = "";
}

bool TessOcrUtils::setLanguages(const QList<Languages> langs)
{
    QString t_langs = "";
    if(langs.isEmpty())
    {
        return false;
    }

    int i;
    for ( i = 0;i<langs.length();i++) {
        Languages lang = langs[i];
        if(!isExistsLanguage(lang))
        {
            return false;
        }
        if((langs.length()-1) == i){
            t_langs+=getLangStr(lang);

        }else{
            t_langs+=getLangStr(lang) + "+";
        }
    }

    m_sLangs = t_langs;
    return  true;

}

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
    }
    return langStr;
}

bool TessOcrUtils::isExistsResultType(ResultType resultType)
{
    bool flag = false;
    switch (resultType)
    {
    case ResultType::ResultString:
        flag = true;
        break;
    case ResultType::ResultHTML:
        flag = true;
        break;
    case ResultType::ResultXML:
        flag = true;
        break;
    }
    return  flag;
}

bool TessOcrUtils::isExistsLanguage(Languages lang)
{
    bool flag = false;
    QString str = getLangStr(lang);
    if(!str.isNull() && !str.isEmpty()){
        flag = true;
    }
    return  flag;
}

