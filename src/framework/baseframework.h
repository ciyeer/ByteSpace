#ifndef BASEFRAMEWORK_H
#define BASEFRAMEWORK_H

#include <QWidget>
#include <QMouseEvent>
#include <QHoverEvent>
#include <QPoint>
#include <QRect>
#include <QDebug>

class BaseFramework : public QWidget {
    Q_OBJECT
public:
    explicit BaseFramework(QWidget *parent = nullptr);
    
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    //void mouseDoubleClickEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override; // Add this line

private:
    enum class Region {
        None,
        Top,
        Bottom, 
        Left,
        Right,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight,
        Central
    };

    bool m_isPressed = false;
    bool m_isResizing = false;
    QPoint m_dragStartPos;       // 拖拽开始的全局位置
    QRect m_startGeometry;       // 拖拽开始时的窗口几何信息
    const int BORDER_WIDTH = 5;  // 边框宽度
    const QSize MIN_SIZE{200, 150}; // 最小窗口尺寸
    Region m_currentRegion = Region::None; // 当前操作区域
    
    Region getRegion(const QPoint &pos);
    void updateCursor(const QPoint &pos);
    void handleResize(const QPoint &globalPos);
};

#endif // BASEFRAMEWORK_H
