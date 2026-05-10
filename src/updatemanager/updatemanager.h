/**
 * @file updatemanager.h
 * @brief 应用程序更新管理器
 *
 * 检查远程更新、下载安装包并验证完整性后执行安装。
 *
 * @author ByteSpace团队
 * @date 2024
 */
#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProgressDialog>

class UpdateManager : public QObject {
    Q_OBJECT

public:
    explicit UpdateManager(QSystemTrayIcon* trayIcon, QObject* parent = nullptr);
    void checkForUpdate();

private:
    QSystemTrayIcon* trayIcon;
    QNetworkAccessManager* networkManager;
    QProgressDialog* progressDialog;
    QString updateUrl;

    void showUpdateAvailable(const QString& version, const QString& url, const QString& changelog, const QString& md5);
    void startDownload(const QString& url, const QString& expectedMd5);
    bool verifyFileMd5(const QString& filePath, const QString& expectedMd5);
    void executeInstaller(const QString& installerPath);

private slots:
    void onUpdateCheckFinished(QNetworkReply* reply);
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
};

#endif // UPDATEMANAGER_H

