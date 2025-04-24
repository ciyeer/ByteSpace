#ifndef RECVWIDGET_H
#define RECVWIDGET_H

#include <QWidget>

namespace Ui {
class RecvWidget;
}

class RecvWidget : public QWidget {
    Q_OBJECT

public:
    explicit RecvWidget(QWidget *parent = nullptr);
    ~RecvWidget();

private:
    Ui::RecvWidget *ui;
};

#endif // RECVWIDGET_H
