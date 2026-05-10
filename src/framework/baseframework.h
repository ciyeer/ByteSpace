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
    
    void toggleMaximize();
    bool isCustomMaximized() const { return m_isCustomMaximized; }

protected:
    void showEvent(QShowEvent *event) override;
    void changeEvent(QEvent *event) override;
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

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
    bool m_isCustomMaximized = false;
    QPoint m_dragStartPos;
    QRect m_startGeometry;
    QRect m_normalGeometry;
    const int BORDER_WIDTH = 5;
    const QSize MIN_SIZE{200, 150};
    Region m_currentRegion = Region::None;
    
    Region getRegion(const QPoint &pos);
    void updateCursor(const QPoint &pos);
    void handleResize(const QPoint &globalPos);
};

#endif // BASEFRAMEWORK_H
