#ifndef RECVWIDGET_H
#define RECVWIDGET_H

#include <QWidget>
#include <QTextBrowser>

namespace Ui {
class RecvWidget;
}

class RecvWidget : public QWidget {
    Q_OBJECT

public:
    explicit RecvWidget(QWidget *parent = nullptr);
    ~RecvWidget();

    QTextBrowser* recvArea() const;

private:
    Ui::RecvWidget *ui;
};

#endif // RECVWIDGET_H
