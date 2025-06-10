#include <QApplication>
#include <QStandardPaths>
#include <QDebug>
#include <QDir>
#include "framework/framework.h"
#include "utils/configmanager.h"
#include "utils/stylemanager.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 设置应用程序信息
    QApplication::setApplicationName("Bytespace");
    QApplication::setOrganizationName("Ciyeer");
    QApplication::setApplicationVersion("1.4.0"); // 更新为当前版本
    
    // 初始化配置管理器
    ConfigManager::instance();

    // 初始化应用程序样式
    StyleManager::initAppStyle(app);
    
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
