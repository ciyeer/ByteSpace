#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QSettings>
#include <QMutex>
#include <QString>
#include <QVariant>
#include <QHash>
//#include <optional>

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
    static const int DEFAULT_MAX_LOG_SIZE = 10;
    static const QString DEFAULT_LOG_PATH;
};

#endif // CONFIGMANAGER_H
