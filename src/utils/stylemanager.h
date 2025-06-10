#ifndef STYLEMANAGER_H
#define STYLEMANAGER_H

#include <QApplication>
#include <QString>

#define DEFAULTSTYLEFILEPATH ":/styles/default.qss"
#define DEFAULTSTYLENAME "Fusion"

class StyleManager {
public:
    // 加载应用程序样式表
    static void loadStyleSheet(QApplication& app, const QString& styleFilePath = DEFAULTSTYLEFILEPATH);
    
    // 设置应用程序基础样式
    static void setupAppStyle(QApplication& app, const QString& styleName = DEFAULTSTYLENAME);
    
    // 初始化完整的应用程序样式（包括基础样式和样式表）
    static void initAppStyle(QApplication& app, 
                             const QString& styleName = DEFAULTSTYLENAME,
                             const QString& styleFilePath = DEFAULTSTYLEFILEPATH);

private:
    StyleManager() = delete; // 禁止实例化
    ~StyleManager() = delete;
    StyleManager(const StyleManager&) = delete;
    StyleManager& operator=(const StyleManager&) = delete;
};

#endif // STYLEMANAGER_H
