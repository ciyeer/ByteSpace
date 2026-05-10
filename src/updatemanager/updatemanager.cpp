/**
 * @file updatemanager.cpp
 * @brief 应用程序更新管理器实现
 *
 * @author ByteSpace团队
 * @date 2024
 */
#include "updatemanager.h"
#include "buildconfig.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QStandardPaths>
#include <QFile>
#include <QCryptographicHash>
#include <QProcess>
#include <QCoreApplication>

UpdateManager::UpdateManager(QSystemTrayIcon* trayIcon, QObject* parent)
    : QObject(parent), trayIcon(trayIcon) {
    networkManager = new QNetworkAccessManager(this);
    progressDialog = new QProgressDialog("Downloading update...", "Cancel", 0, 100, nullptr);
    progressDialog->setWindowModality(Qt::WindowModal);
    updateUrl = QString("https://ciyeer.com/bytespace/update.json");
}

void UpdateManager::checkForUpdate() {
    QUrl url(updateUrl);
    QNetworkRequest request(url);

    connect(networkManager, &QNetworkAccessManager::finished, this, &UpdateManager::onUpdateCheckFinished);
    networkManager->get(request);
}

void UpdateManager::onUpdateCheckFinished(QNetworkReply* reply) {
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Failed to fetch update info:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray response = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
    QJsonObject jsonObj = jsonDoc.object();

    QString latestVersion = jsonObj.value("version").toString();
    QString downloadUrl = jsonObj.value("url").toString();
    QString changelog = jsonObj.value("changelog").toString();
    QString md5 = jsonObj.value("md5").toString();

    if (latestVersion > QString(BYTESPACE_VERSION)) {
        showUpdateAvailable(latestVersion, downloadUrl, changelog, md5);
    }
    else {
        QMessageBox::information(nullptr, "No Update", "You are using the latest version.");
    }
    reply->deleteLater();
}

void UpdateManager::showUpdateAvailable(const QString& version, const QString& url, const QString& changelog, const QString& md5) {
    QMessageBox::StandardButton reply = QMessageBox::question(
        nullptr,
        "Update Available",
        QString("A new version (%1) is available!\n\n%2\n\nDo you want to update now?").arg(version, changelog),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        startDownload(url, md5);
    }
}

void UpdateManager::startDownload(const QString& url, const QString& expectedMd5) {
    QUrl qurl(url);  // 创建 QUrl 对象
    QNetworkRequest request(qurl);  // 使用 QUrl 创建 QNetworkRequest 对象

    // 确保传递正确的参数
    QNetworkReply* reply = networkManager->get(request);  // 调用 get() 方法

    connect(reply, &QNetworkReply::downloadProgress, this, &UpdateManager::onDownloadProgress);
    connect(reply, &QNetworkReply::finished, this, [=]() {
        QString savePath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/update_installer.exe";
        QFile file(savePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(reply->readAll());
            file.close();

            if (verifyFileMd5(savePath, expectedMd5)) {
                executeInstaller(savePath);
            }
            else {
                QMessageBox::critical(nullptr, "Update Error", "File integrity check failed.");
            }
        }
        else {
            QMessageBox::critical(nullptr, "Update Error", "Failed to save the update file.");
        }
        reply->deleteLater();
    });
}


void UpdateManager::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    progressDialog->setValue(static_cast<int>((bytesReceived * 100) / bytesTotal));
    if (progressDialog->wasCanceled()) {
        qDebug() << "Download canceled!";
    }
}

bool UpdateManager::verifyFileMd5(const QString& filePath, const QString& expectedMd5) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QCryptographicHash hash(QCryptographicHash::Md5);
    if (!hash.addData(&file))
        return false;

    QString fileMd5 = hash.result().toHex();
    return fileMd5 == expectedMd5;
}

void UpdateManager::executeInstaller(const QString& installerPath) {
    QProcess::startDetached(installerPath, QStringList());
    qApp->quit();
}
