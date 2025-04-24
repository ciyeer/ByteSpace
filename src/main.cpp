#include <QApplication>
#include <QStyleFactory>
#include <QStandardPaths>
#include <QDebug>
#include <QDir>
#include "framework/framework.h"
#include "utils/configmanager.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 设置应用程序信息
    QApplication::setApplicationName("Bytespace");
    QApplication::setOrganizationName("Ciyeer");
    QApplication::setApplicationVersion("1.4.0"); // 更新为当前版本
    
    // 初始化配置管理器
    ConfigManager::instance();

    // 设置应用程序样式
    app.setStyle(QStyleFactory::create("Fusion"));

    // 尝试从资源文件加载样式表
    QFile styleFile(":/styles/default.qss");
    if (styleFile.exists() && styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        app.setStyleSheet(styleSheet);
        styleFile.close();
        qDebug() << "已加载样式表";
    } 
    else {
        qDebug() << "无法加载样式表文件";
    }
    
    // 输出配置文件路径
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString configPath = QDir(appDataPath).filePath("config.ini");
    qDebug() << "配置文件路径:" << configPath;
    
    // 创建并显示主窗口
    Framework mainWindow;
    mainWindow.show();
    
    // 在应用程序退出前保存配置
    QObject::connect(&app, &QApplication::aboutToQuit, []() {
        ConfigManager::instance().sync();
    });
    
    return app.exec();
}
