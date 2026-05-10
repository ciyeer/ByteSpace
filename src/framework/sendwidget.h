#ifndef SENDWIDGET_H
#define SENDWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>

namespace Ui {
class SendWidget;
}

class SendWidget : public QWidget {
    Q_OBJECT

public:
    explicit SendWidget(QWidget *parent = nullptr);
    ~SendWidget();

    QTextEdit* sendArea() const;
    QPushButton* dataSendBtn() const;
    QPushButton* clearSendBtn() const;
    QPushButton* clearRecvBtn() const;

private:
    Ui::SendWidget *ui;
};

#endif // SENDWIDGET_H

