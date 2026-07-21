#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QWidget>

namespace Ui {
class StatusBar;
}

class StatusBar : public QWidget {
    Q_OBJECT

public:
    explicit StatusBar(QWidget *parent = nullptr);
    ~StatusBar();

public slots:
    void onRecvMessage(const QString &);
    void updateTxRx(qint64 tx, qint64 rx);

private:
    Ui::StatusBar *ui;
    QString m_strRecvMsg;
};

#endif // STATUSBAR_H
//
