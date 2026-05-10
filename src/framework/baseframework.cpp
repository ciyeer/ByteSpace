#include "baseframework.h"
#include <QMouseEvent>
#include <QShowEvent>
#include <QScreen>
#include <QGuiApplication>

#ifdef Q_OS_WIN
#include <windows.h>
#include <windowsx.h>
#endif

BaseFramework::BaseFramework(QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_Hover);
    setMouseTracking(true);
    installEventFilter(this);
}

void BaseFramework::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
#ifdef Q_OS_WIN
    HWND hwnd = reinterpret_cast<HWND>(winId());
    SetWindowLongPtr(hwnd, GWL_STYLE,
        GetWindowLongPtr(hwnd, GWL_STYLE) | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
#endif
}

void BaseFramework::toggleMaximize() {
    if (isMaximized() || m_isCustomMaximized) {
        m_isCustomMaximized = false;
        if (m_normalGeometry.isValid()) {
            setGeometry(m_normalGeometry);
        } else {
            showNormal();
        }
    } else {
        m_normalGeometry = geometry();
        m_isCustomMaximized = true;
        QScreen *screen = QGuiApplication::primaryScreen();
        if (screen) {
            setGeometry(screen->availableGeometry());
        } else {
            showMaximized();
        }
    }
}

void BaseFramework::changeEvent(QEvent *event) {
    if (event->type() == QEvent::WindowStateChange) {
        if (isMaximized()) {
            m_isCustomMaximized = false;
        }
        m_normalGeometry = normalGeometry();
    }
    QWidget::changeEvent(event);
}

BaseFramework::Region BaseFramework::getRegion(const QPoint &pos) {
    int x = pos.x();
    int y = pos.y();
    int w = width();
    int h = height();
    
    if(x <= BORDER_WIDTH && y <= BORDER_WIDTH) return Region::TopLeft;
    if(x >= w - BORDER_WIDTH && y <= BORDER_WIDTH) return Region::TopRight;
    if(x <= BORDER_WIDTH && y >= h - BORDER_WIDTH) return Region::BottomLeft;
    if(x >= w - BORDER_WIDTH && y >= h - BORDER_WIDTH) return Region::BottomRight;
    if(y <= BORDER_WIDTH) return Region::Top;
    if(y >= h - BORDER_WIDTH) return Region::Bottom;
    if(x <= BORDER_WIDTH) return Region::Left;
    if(x >= w - BORDER_WIDTH) return Region::Right;
    
    return Region::Central;
}

void BaseFramework::updateCursor(const QPoint &pos) {
    if(m_isPressed) return;
    
    switch(getRegion(pos)) {
        case Region::Top:
        case Region::Bottom:
            setCursor(Qt::SizeVerCursor);
            break;
        case Region::Left:
        case Region::Right:
            setCursor(Qt::SizeHorCursor);
            break;
        case Region::TopLeft:
        case Region::BottomRight:
            setCursor(Qt::SizeFDiagCursor);
            break;
        case Region::TopRight:
        case Region::BottomLeft:
            setCursor(Qt::SizeBDiagCursor);
            break;
        default:
            setCursor(Qt::ArrowCursor);
            break;
    }
}

void BaseFramework::mousePressEvent(QMouseEvent *event) {
    if(event->button() == Qt::LeftButton) {
        m_isPressed = true;
        m_currentRegion = getRegion(event->pos());
        m_isResizing = (m_currentRegion != Region::Central);
        
        // 保存起始位置和几何信息
        m_dragStartPos = event->globalPosition().toPoint();
        m_startGeometry = geometry();
    }
}

void BaseFramework::handleResize(const QPoint &globalPos) {
    if(!m_isResizing) return;
    
    // 计算鼠标移动的距离
    int dx = globalPos.x() - m_dragStartPos.x();
    int dy = globalPos.y() - m_dragStartPos.y();
    
    // 新的几何信息
    QRect newGeometry = m_startGeometry;
    
    // 根据不同区域处理拉伸
    switch(m_currentRegion) {
        case Region::Top: {
            int newHeight = m_startGeometry.height() - dy;
            if(newHeight >= MIN_SIZE.height()) {
                newGeometry.setTop(m_startGeometry.top() + dy);
            } 
            else {
                // 限制最小高度
                newGeometry.setTop(m_startGeometry.bottom() - MIN_SIZE.height());
            }
            break;
        }
        case Region::Bottom: {
            int newHeight = m_startGeometry.height() + dy;
            if(newHeight >= MIN_SIZE.height()) {
                newGeometry.setBottom(m_startGeometry.bottom() + dy);
            } else {
                // 限制最小高度
                newGeometry.setBottom(m_startGeometry.top() + MIN_SIZE.height());
            }
            break;
        }
        case Region::Left: {
            int newWidth = m_startGeometry.width() - dx;
            if(newWidth >= MIN_SIZE.width()) {
                newGeometry.setLeft(m_startGeometry.left() + dx);
            } else {
                // 限制最小宽度
                newGeometry.setLeft(m_startGeometry.right() - MIN_SIZE.width());
            }
            break;
        }
        case Region::Right: {
            int newWidth = m_startGeometry.width() + dx;
            if(newWidth >= MIN_SIZE.width()) {
                newGeometry.setRight(m_startGeometry.right() + dx);
            } else {
                // 限制最小宽度
                newGeometry.setRight(m_startGeometry.left() + MIN_SIZE.width());
            }
            break;
        }
        case Region::TopLeft: {
            // 处理宽度
            int newWidth = m_startGeometry.width() - dx;
            if(newWidth >= MIN_SIZE.width()) {
                newGeometry.setLeft(m_startGeometry.left() + dx);
            } 
            else {
                newGeometry.setLeft(m_startGeometry.right() - MIN_SIZE.width());
            }
            
            // 处理高度
            int newHeight = m_startGeometry.height() - dy;
            if(newHeight >= MIN_SIZE.height()) {
                newGeometry.setTop(m_startGeometry.top() + dy);
            }
            else {
                newGeometry.setTop(m_startGeometry.bottom() - MIN_SIZE.height());
            }
            break;
        }
        case Region::TopRight: {
            // 处理宽度
            int newWidth = m_startGeometry.width() + dx;
            if(newWidth >= MIN_SIZE.width()) {
                newGeometry.setRight(m_startGeometry.right() + dx);
            } 
            else {
                newGeometry.setRight(m_startGeometry.left() + MIN_SIZE.width());
            }
            
            // 处理高度
            int newHeight = m_startGeometry.height() - dy;
            if(newHeight >= MIN_SIZE.height()) {
                newGeometry.setTop(m_startGeometry.top() + dy);
            } 
            else {
                newGeometry.setTop(m_startGeometry.bottom() - MIN_SIZE.height());
            }
            break;
        }
        case Region::BottomLeft: {
            // 处理宽度
            int newWidth = m_startGeometry.width() - dx;
            if(newWidth >= MIN_SIZE.width()) {
                newGeometry.setLeft(m_startGeometry.left() + dx);
            } 
            else {
                newGeometry.setLeft(m_startGeometry.right() - MIN_SIZE.width());
            }
            
            // 处理高度
            int newHeight = m_startGeometry.height() + dy;
            if(newHeight >= MIN_SIZE.height()) {
                newGeometry.setBottom(m_startGeometry.bottom() + dy);
            } 
            else {
                newGeometry.setBottom(m_startGeometry.top() + MIN_SIZE.height());
            }
            break;
        }
        case Region::BottomRight: {
            // 处理宽度
            int newWidth = m_startGeometry.width() + dx;
            if(newWidth >= MIN_SIZE.width()) {
                newGeometry.setRight(m_startGeometry.right() + dx);
            } 
            else {
                newGeometry.setRight(m_startGeometry.left() + MIN_SIZE.width());
            }
            
            // 处理高度
            int newHeight = m_startGeometry.height() + dy;
            if(newHeight >= MIN_SIZE.height()) {
                newGeometry.setBottom(m_startGeometry.bottom() + dy);
            } 
            else {
                newGeometry.setBottom(m_startGeometry.top() + MIN_SIZE.height());
            }
            break;
        }
        default:
            break;
    }
    
    // 应用新的几何信息
    if(newGeometry.isValid()) {
        setGeometry(newGeometry);
    }
}

void BaseFramework::mouseReleaseEvent(QMouseEvent *event) {
    if(event->button() == Qt::LeftButton) {
        m_isPressed = false;
        m_isResizing = false;
        updateCursor(event->pos());
    }
}

bool BaseFramework::eventFilter(QObject *watched, QEvent *event) {
    if(watched == this) {
        switch(event->type()) {
            case QEvent::MouseMove: {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                if(m_isPressed) {
                    if(m_isResizing) {
                        handleResize(mouseEvent->globalPosition().toPoint());
                    } 
                    else {                       
                        move(mouseEvent->globalPosition().toPoint() - (m_dragStartPos - m_startGeometry.topLeft()));    // 移动窗口
                    }
                } 
                else {
                    updateCursor(mouseEvent->pos());
                }
                break;
            }
            case QEvent::HoverMove: {
                QHoverEvent *hoverEvent = static_cast<QHoverEvent*>(event);
                updateCursor(hoverEvent->position().toPoint());
                break;
            }
            default:
                break;
        }
    }
    return QWidget::eventFilter(watched, event);
}

void BaseFramework::mouseMoveEvent(QMouseEvent *event) {
    if(m_isPressed) {
        if(m_isResizing) {
            handleResize(event->globalPosition().toPoint());
        } 
        else {           
            move(event->globalPosition().toPoint() - (m_dragStartPos - m_startGeometry.topLeft()));     // 移动窗口
        }
    }
    updateCursor(event->pos());
}

void BaseFramework::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
}

bool BaseFramework::nativeEvent(const QByteArray &eventType, void *message, qintptr *result) {
#ifdef Q_OS_WIN
    if (eventType == "windows_generic_MSG") {
        MSG *msg = static_cast<MSG *>(message);
        if (msg->message == WM_SYSCOMMAND) {
            if (msg->wParam == SC_MINIMIZE) {
                showMinimized();
                *result = 0;
                return true;
            }
        }
    }
#else
    Q_UNUSED(eventType)
    Q_UNUSED(message)
    Q_UNUSED(result)
#endif
    return QWidget::nativeEvent(eventType, message, result);
}
