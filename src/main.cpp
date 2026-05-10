/**
 * @file main.cpp
 * @brief ByteSpace 应用程序入口
 *
 * 初始化应用程序信息、配置管理器、样式管理器，创建并显示主窗口。
 *
 * @author ByteSpace团队
 * @date 2024
 */
#include <QApplication>
#include <QStandardPaths>
#include <QDebug>
#include <QDir>
#include "buildconfig.h"
#include "framework/framework.h"
#include "utils/configmanager.h"
#include "utils/stylemanager.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QApplication::setApplicationName("Bytespace");
    QApplication::setOrganizationName("Ciyeer");
    QApplication::setApplicationVersion(BYTESPACE_VERSION);
    
    // 初始化配置管理器
    ConfigManager::instance();

    // 初始化应用程序样式
    StyleManager::setupAppStyle(app);
    
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
