/**
 * @file configmanager.h
 * @brief 应用程序配置管理器
 *
 * 使用 QSettings 管理应用程序配置。支持缓存、事务批处理、
 * 串口配置、UI 配置、日志配置和线程池配置。
 *
 * @author ByteSpace团队
 * @date 2024
 */
#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QSettings>
#include <QMutex>
#include <QString>
#include <QVariant>
#include <QHash>

class ConfigManager : public QObject {
    Q_OBJECT
public:
    static ConfigManager& instance();

    // 串口配置
    QString portName() const;
    void setPortName(const QString& name);
    int baudRate() const;
    void setBaudRate(int rate);
    int dataBits() const;
    void setDataBits(int bits);
    int stopBits() const;
    void setStopBits(int bits);
    QString parity() const;
    void setParity(const QString& parity);
    QString flowControl() const;
    void setFlowControl(const QString& flowControl);

    // 界面配置
    bool rememberWindowGeometry() const;
    void setRememberWindowGeometry(bool remember);
    QByteArray windowGeometry() const;
    void setWindowGeometry(const QByteArray& geometry);

    // 日志配置
    bool autoSaveLog() const;
    void setAutoSaveLog(bool autoSave);
    QString logPath() const;
    void setLogPath(const QString& path);
    int maxLogSize() const;
    void setMaxLogSize(int size);

    // 缓存控制
    void enableCache(bool enable);
    void clearCache();

    // 批量操作
    void beginTransaction();
    void commitTransaction();
    void rollbackTransaction();

    // 线程池配置
    int maxThreadCount() const;
    void setMaxThreadCount(int count);

public slots:
    void saveSettings();
    void loadSettings();
    void resetSettings();
    void sync();

signals:
    void configChanged(const QString& key);
    void saveError(const QString& error);
    void transactionCompleted();

private:
    explicit ConfigManager(QObject *parent = nullptr);
    ~ConfigManager();

    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    void initializeDefaultValues();
    QString configFilePath() const;
    QVariant getCachedValue(const QString& key, const QVariant& defaultValue) const;
    void setCachedValue(const QString& key, const QVariant& value);
    
    QSettings* m_settings;
    mutable QMutex m_mutex;
    mutable QHash<QString, QVariant> m_cache;
    QHash<QString, QVariant> m_transactionCache;
    bool m_cacheEnabled;
    bool m_inTransaction;

    // 默认值
    static const QString DEFAULT_PORT_NAME;
    static const int DEFAULT_BAUD_RATE = 115200;
    static const int DEFAULT_DATA_BITS = 8;
    static const int DEFAULT_STOP_BITS = 1;
    static const QString DEFAULT_PARITY;
    static const QString DEFAULT_FLOW_CONTROL;
    static const int DEFAULT_MAX_LOG_SIZE = 10;
    static const QString DEFAULT_LOG_PATH;
    static const int DEFAULT_MAX_THREAD_COUNT = 4;  // 添加线程池默认值
};

#endif // CONFIGMANAGER_H
