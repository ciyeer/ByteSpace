#ifndef SENDWIDGET_H
#define SENDWIDGET_H

#include <QWidget>

namespace Ui {
class SendWidget;
}

class SendWidget : public QWidget {
    Q_OBJECT

public:
    explicit SendWidget(QWidget *parent = nullptr);
    ~SendWidget();

private:
    Ui::SendWidget *ui;
};

#endif // SENDWIDGET_H

