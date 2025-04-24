#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#include <QObject>
#include <QSystemTrayIcon> // 提前放置，优先加载
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProgressDialog>

class UpdateManager : public QObject {
    Q_OBJECT

public:
    explicit UpdateManager(QSystemTrayIcon* trayIcon, QObject* parent = nullptr);
    void checkForUpdate(); // 检查更新

private:
    QSystemTrayIcon* trayIcon;
    QNetworkAccessManager* networkManager;
    QProgressDialog* progressDialog;
    QString currentVersion{"1.0.0"};  // 添加版本号作为成员变量
    QString updateUrl{"https://example.com/update.json"};  // 添加更新URL作为成员变量

    void showUpdateAvailable(const QString& version, const QString& url, const QString& changelog, const QString& md5);
    void startDownload(const QString& url, const QString& expectedMd5);
    bool verifyFileMd5(const QString& filePath, const QString& expectedMd5);
    void executeInstaller(const QString& installerPath);

private slots:
    void onUpdateCheckFinished(QNetworkReply* reply);
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
};

#endif // UPDATEMANAGER_H
//

