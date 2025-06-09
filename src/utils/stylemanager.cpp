#include "stylemanager.h"
#include <QFile>
#include <QStyleFactory>
#include <QDebug>

void StyleManager::loadStyleSheet(QApplication& app, const QString& styleFilePath)
{
    QFile styleFile(styleFilePath);
    if (styleFile.exists() && styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        app.setStyleSheet(styleSheet);
        styleFile.close();
        qDebug() << "已加载样式表:" << styleFilePath;
    } 
    else {
        qDebug() << "无法加载样式表文件:" << styleFilePath;
    }
}

void StyleManager::setupApplicationStyle(QApplication& app, const QString& styleName)
{
    app.setStyle(QStyleFactory::create(styleName));
    qDebug() << "已设置应用程序样式:" << styleName;
}

void StyleManager::initializeApplicationStyle(QApplication& app, 
                                            const QString& styleName,
                                            const QString& styleFilePath)
{
    // 设置基础样式
    setupApplicationStyle(app, styleName);
    
    // 加载样式表
    loadStyleSheet(app, styleFilePath);
}