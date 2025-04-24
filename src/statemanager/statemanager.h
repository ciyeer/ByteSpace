#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include <QObject>
#include <QMutex>
#include <QMap>
#include <QString>

// 应用程序状态枚举
enum class AppState {
    Idle,           // 空闲状态
    Connecting,     // 正在连接
    Connected,      // 已连接
    Disconnecting,  // 正在断开连接
    Processing,     // 处理中
    Error           // 错误状态
};

class StateManager : public QObject {
    Q_OBJECT
public:
    // 使用Meyers单例模式
    static StateManager& instance();
    
    // 状态管理
    bool changeState(AppState newState);
    AppState currentState() const;
    QString currentStateString() const;
    bool canTransitionTo(AppState targetState) const;
    
    // 通用状态查询方法，替代多个单独的查询方法
    bool isInState(AppState state) const;
    
    // 设置错误状态 - 现在只是改变状态并通过ErrorHandler处理错误
    bool setErrorState(const QString& errorMessage);
    
signals:
    void stateChanged(AppState oldState, AppState newState);
    
private:
    // 私有构造函数和析构函数
    explicit StateManager(QObject* parent = nullptr);
    ~StateManager() = default;
    
    // 禁止复制
    StateManager(const StateManager&) = delete;
    StateManager& operator=(const StateManager&) = delete;
    
    // 初始化函数
    void initializeTransitions();
    void initializeStateStrings();
    
    // 成员变量
    AppState m_currentState;
    QMap<AppState, QList<AppState>> m_allowedTransitions;
    QMap<AppState, QString> m_stateStrings;
    mutable QMutex m_mutex;
};

#endif // STATEMANAGER_H
