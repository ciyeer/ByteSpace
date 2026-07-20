#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include <QObject>
#include <QMutex>
#include <QMap>
#include <QString>

/**
 * @enum AppState
 * @brief 应用程序状态枚举
 */
enum class AppState {
    Idle,           // 空闲状态
    Connecting,     // 正在连接
    Connected,      // 已连接
    Disconnecting,  // 正在断开连接
    Processing,     // 处理中
    Error           // 错误状态
};

/**
 * @class StateManager
 * @brief 应用程序状态管理器
 *
 * StateManager负责管理应用程序的状态转换，确保状态变化符合预定义的规则。
 * 使用单例模式实现，保证全局只有一个状态管理实例。
 *
 * @author ByteSpace团队
 * @date 2024-11-11
 */
class StateManager : public QObject {
    Q_OBJECT
public:
    // 使用Meyers单例模式
    static StateManager& instance();
    
    // 状态管理
    /**
     * @brief 改变应用程序状态
     * @param newState 目标状态
     * @return 状态转换是否成功
     *
     * 尝试将应用程序状态从当前状态转换为目标状态。
     * 只有当目标状态在当前状态的允许转换列表中时，转换才会成功。
     * 状态转换成功后会发出stateChanged信号。
     */
    bool changeState(AppState newState);
    
    /**
     * @brief 获取当前应用程序状态
     * @return 当前状态
     *
     * 线程安全的获取当前应用程序状态。
     */
    AppState currentState() const;
    QString currentStateString() const;
    bool canTransitionTo(AppState targetState) const;
    
    // 通用状态查询方法，替代多个单独的查询方法
    bool isInState(AppState state) const;
    
    // 设置错误状态 - 现在只是改变状态并通过ErrorHandler处理错误
    bool setErrorState(const QString& errorMessage);
    
signals:
    /**
     * @brief 状态变化信号
     * @param oldState 旧状态
     * @param newState 新状态
     *
     * 当应用程序状态发生变化时发出此信号。
     */
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

    // 内部无锁版本（调用方必须持有 m_mutex）
    bool canTransitionToLocked(AppState targetState) const;
    
    // 成员变量
    AppState m_currentState;
    QMap<AppState, QList<AppState>> m_allowedTransitions;
    QMap<AppState, QString> m_stateStrings;
    mutable QMutex m_mutex;
};

#endif // STATEMANAGER_H
