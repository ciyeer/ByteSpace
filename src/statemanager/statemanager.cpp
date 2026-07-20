#include "statemanager.h"
#include "utils/errorhandler.h"

StateManager& StateManager::instance() {
    static StateManager instance;
    return instance;
}

StateManager::StateManager(QObject* parent) : QObject(parent), 
    m_currentState(AppState::Idle) {
    
    initializeTransitions();
    initializeStateStrings();
}

void StateManager::initializeTransitions() {
    // 设置允许的状态转换
    m_allowedTransitions[AppState::Idle] = { AppState::Connecting, AppState::Error };
    m_allowedTransitions[AppState::Connecting] = { AppState::Connected, AppState::Idle, AppState::Error };
    m_allowedTransitions[AppState::Connected] = { AppState::Disconnecting, AppState::Processing, AppState::Error };
    m_allowedTransitions[AppState::Disconnecting] = { AppState::Idle, AppState::Error };
    m_allowedTransitions[AppState::Processing] = { AppState::Connected, AppState::Error };
    m_allowedTransitions[AppState::Error] = { AppState::Idle };
}

void StateManager::initializeStateStrings() {
    // 设置状态字符串
    m_stateStrings[AppState::Idle] = "空闲";
    m_stateStrings[AppState::Connecting] = "正在连接";
    m_stateStrings[AppState::Connected] = "已连接";
    m_stateStrings[AppState::Disconnecting] = "正在断开连接";
    m_stateStrings[AppState::Processing] = "处理中";
    m_stateStrings[AppState::Error] = "错误";
}

bool StateManager::changeState(AppState newState) {
    AppState oldState = AppState::Idle;
    bool canTransition;

    {
        QMutexLocker locker(&m_mutex);
        canTransition = canTransitionToLocked(newState);
        if (canTransition) {
            oldState = m_currentState;
            m_currentState = newState;
        }
    }

    if (canTransition) {
        emit stateChanged(oldState, newState);
    }

    return canTransition;
}

AppState StateManager::currentState() const {
    QMutexLocker locker(&m_mutex);
    return m_currentState;
}

QString StateManager::currentStateString() const {
    QMutexLocker locker(&m_mutex);
    return m_stateStrings.value(m_currentState, "未知状态");
}

bool StateManager::canTransitionTo(AppState targetState) const {
    QMutexLocker locker(&m_mutex);
    return canTransitionToLocked(targetState);
}

bool StateManager::canTransitionToLocked(AppState targetState) const {
    // 如果目标状态与当前状态相同，则允许转换
    if (targetState == m_currentState) {
        return true;
    }

    // 检查目标状态是否在允许的转换列表中
    return m_allowedTransitions.value(m_currentState).contains(targetState);
}

bool StateManager::isInState(AppState state) const {
    QMutexLocker locker(&m_mutex);
    return m_currentState == state;
}

bool StateManager::setErrorState(const QString& errorMessage) {
    // 使用ErrorHandler处理错误
    ErrorHandler::instance().handleError(ErrorType::Application, errorMessage);
    
    // 将状态更改为错误状态
    return changeState(AppState::Error);
}
