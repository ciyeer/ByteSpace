#include "configmanager.h"
#include "logmanager/logutils.h"
#include <QDir>
#include <QStandardPaths>

const QString ConfigManager::DEFAULT_PORT_NAME = "COM1";
const QString ConfigManager::DEFAULT_PARITY = "None";
const QString ConfigManager::DEFAULT_LOG_PATH = 
    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Termnite/logs";

ConfigManager& ConfigManager::instance() {
    static ConfigManager instance;
    return instance;
}

ConfigManager::ConfigManager(QObject *parent) : QObject(parent), 
    m_settings(nullptr), 
    m_cacheEnabled(true), 
    m_inTransaction(false) {
    
    QString configPath = configFilePath();
    QFile configFile(configPath);
    
    // 检查配置文件是否存在，如果不存在则创建默认配置
    if (!configFile.exists()) {
        // 创建目录
        QDir dir = QFileInfo(configPath).dir();
        if (!dir.exists()) {
            dir.mkpath(".");
        }
        
        // 创建默认配置文件
        if (configFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&configFile);
            out << "[General]\n";
            out << "FirstRun=true\n";
            
            // 添加默认串口配置
            out << "[SerialPort]\n";
            out << "Name=" << DEFAULT_PORT_NAME << "\n";
            out << "BaudRate=" << DEFAULT_BAUD_RATE << "\n";
            out << "DataBits=" << DEFAULT_DATA_BITS << "\n";
            out << "StopBits=" << DEFAULT_STOP_BITS << "\n";
            out << "Parity=" << DEFAULT_PARITY << "\n";
            
            // 添加默认UI配置
            out << "[UI]\n";
            out << "RememberWindowGeometry=true\n";
            
            // 添加默认日志配置
            out << "[Log]\n";
            out << "AutoSave=true\n";
            out << "Path=" << DEFAULT_LOG_PATH << "\n";
            out << "MaxSize=" << DEFAULT_MAX_LOG_SIZE << "\n";
            
            configFile.close();
        }
    }
    
    // 初始化设置
    m_settings = new QSettings(configPath, QSettings::IniFormat, this);
    
    // 确保所有默认值都已设置
    initializeDefaultValues();
    
    // 加载设置
    loadSettings();
}

ConfigManager::~ConfigManager() {
    saveSettings();
}

void ConfigManager::initializeDefaultValues() {
    if (!m_settings->contains("SerialPort/Name")) {
        m_settings->setValue("SerialPort/Name", DEFAULT_PORT_NAME);
    }
    if (!m_settings->contains("SerialPort/BaudRate")) {
        m_settings->setValue("SerialPort/BaudRate", DEFAULT_BAUD_RATE);
    }
    if (!m_settings->contains("SerialPort/DataBits")) {
        m_settings->setValue("SerialPort/DataBits", DEFAULT_DATA_BITS);
    }
    if (!m_settings->contains("SerialPort/StopBits")) {
        m_settings->setValue("SerialPort/StopBits", DEFAULT_STOP_BITS);
    }
    if (!m_settings->contains("SerialPort/Parity")) {
        m_settings->setValue("SerialPort/Parity", DEFAULT_PARITY);
    }
    
    // 界面配置
    if (!m_settings->contains("UI/RememberWindowGeometry")) {
        m_settings->setValue("UI/RememberWindowGeometry", true);
    }
    

    // 日志配置
    if (!m_settings->contains("Log/AutoSave")) {
        m_settings->setValue("Log/AutoSave", true);
    }
    if (!m_settings->contains("Log/Path")) {
        m_settings->setValue("Log/Path", DEFAULT_LOG_PATH);
    }
    if (!m_settings->contains("Log/MaxSize")) {
        m_settings->setValue("Log/MaxSize", DEFAULT_MAX_LOG_SIZE);
    }
}

QVariant ConfigManager::getCachedValue(const QString& key, const QVariant& defaultValue) const {
    QMutexLocker locker(&m_mutex);
    
    if (m_inTransaction && m_transactionCache.contains(key)) {
        return m_transactionCache.value(key);
    }
    
    if (m_cacheEnabled && m_cache.contains(key)) {
        return m_cache.value(key);
    }
    
    QVariant value = m_settings->value(key, defaultValue);
    if (m_cacheEnabled) {
        m_cache.insert(key, value);
    }
    return value;
}

void ConfigManager::setCachedValue(const QString& key, const QVariant& value) {
    QMutexLocker locker(&m_mutex);
    
    if (m_inTransaction) {
        m_transactionCache.insert(key, value);
    } 
    else {
        m_settings->setValue(key, value);
        if (m_cacheEnabled) {
            m_cache.insert(key, value);
        }
        emit configChanged(key);
    }
}

void ConfigManager::beginTransaction() {
    QMutexLocker locker(&m_mutex);
    m_inTransaction = true;
    m_transactionCache.clear();
}

void ConfigManager::commitTransaction() {
    QMutexLocker locker(&m_mutex);
    if (!m_inTransaction) return;
    
    for (auto it = m_transactionCache.constBegin(); it != m_transactionCache.constEnd(); ++it) {
        m_settings->setValue(it.key(), it.value());
        if (m_cacheEnabled) {
            m_cache.insert(it.key(), it.value());
        }
        emit configChanged(it.key());
    }
    
    m_transactionCache.clear();
    m_inTransaction = false;
    emit transactionCompleted();
}

void ConfigManager::rollbackTransaction() {
    QMutexLocker locker(&m_mutex);
    m_transactionCache.clear();
    m_inTransaction = false;
}

// 串口配置
QString ConfigManager::portName() const {
    return getCachedValue("SerialPort/Name", DEFAULT_PORT_NAME).toString();
}

void ConfigManager::setPortName(const QString& name) {
    setCachedValue("SerialPort/Name", name);
}

int ConfigManager::baudRate() const {
    return getCachedValue("SerialPort/BaudRate", DEFAULT_BAUD_RATE).toInt();
}

void ConfigManager::setBaudRate(int rate) {
    setCachedValue("SerialPort/BaudRate", rate);
}

int ConfigManager::dataBits() const {
    return getCachedValue("SerialPort/DataBits", DEFAULT_DATA_BITS).toInt();
}

void ConfigManager::setDataBits(int bits) {
    setCachedValue("SerialPort/DataBits", bits);
}

int ConfigManager::stopBits() const {
    return getCachedValue("SerialPort/StopBits", DEFAULT_STOP_BITS).toInt();
}

void ConfigManager::setStopBits(int bits) {
    setCachedValue("SerialPort/StopBits", bits);
}

QString ConfigManager::parity() const {
    return getCachedValue("SerialPort/Parity", DEFAULT_PARITY).toString();
}

void ConfigManager::setParity(const QString& parity) {
    setCachedValue("SerialPort/Parity", parity);
}

// 界面配置
bool ConfigManager::rememberWindowGeometry() const {
    return getCachedValue("UI/RememberWindowGeometry", true).toBool();
}

void ConfigManager::setRememberWindowGeometry(bool remember) {
    setCachedValue("UI/RememberWindowGeometry", remember);
}

QByteArray ConfigManager::windowGeometry() const {
    return getCachedValue("UI/WindowGeometry", QByteArray()).toByteArray();
}

void ConfigManager::setWindowGeometry(const QByteArray& geometry) {
    setCachedValue("UI/WindowGeometry", geometry);
}

// 日志配置
bool ConfigManager::autoSaveLog() const {
    return getCachedValue("Log/AutoSave", true).toBool();
}

void ConfigManager::setAutoSaveLog(bool autoSave) {
    setCachedValue("Log/AutoSave", autoSave);
}

QString ConfigManager::logPath() const {
    return getCachedValue("Log/Path", DEFAULT_LOG_PATH).toString();
}

void ConfigManager::setLogPath(const QString& path) {
    setCachedValue("Log/Path", path);
}

int ConfigManager::maxLogSize() const {
    return getCachedValue("Log/MaxSize", DEFAULT_MAX_LOG_SIZE).toInt();
}

void ConfigManager::setMaxLogSize(int size) {
    setCachedValue("Log/MaxSize", size);
}

void ConfigManager::enableCache(bool enable) {
    QMutexLocker locker(&m_mutex);
    m_cacheEnabled = enable;
    if (!enable) {
        clearCache();
    }
}

void ConfigManager::clearCache() {
    m_cache.clear();
}

void ConfigManager::saveSettings() {
    QMutexLocker locker(&m_mutex);
    m_settings->sync();
    if (m_settings->status() != QSettings::NoError) {
        emit saveError("配置保存失败");
        LogUtils::logMessage("配置保存失败", LOG::LOG_ERROR);
    }
}

void ConfigManager::loadSettings() {
    QMutexLocker locker(&m_mutex);
    clearCache();
    m_settings->sync();
}

void ConfigManager::resetSettings() {
    QMutexLocker locker(&m_mutex);
    m_settings->clear();
    initializeDefaultValues();
    emit configChanged("ALL");
}

void ConfigManager::sync() {
    QMutexLocker locker(&m_mutex);
    if (m_inTransaction) {
        commitTransaction();
    }
    saveSettings();
}

QString ConfigManager::configFilePath() const {
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(appDataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return dir.filePath("config.ini");
}
