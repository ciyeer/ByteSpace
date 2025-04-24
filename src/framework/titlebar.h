#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QWidget>

namespace Ui {
class TitleBar;
}

class TitleBar : public QWidget {
    Q_OBJECT

public:
    explicit TitleBar(QWidget *parent = nullptr);
    ~TitleBar();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override; // 添加这一行

private:
    Ui::TitleBar *ui;

signals:
    void settingsButtonClicked(); // 添加设置按钮点击信号
};

#endif // TITLEBAR_H
