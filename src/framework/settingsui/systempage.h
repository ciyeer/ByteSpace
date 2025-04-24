#ifndef SYSTEMPAGE_H
#define SYSTEMPAGE_H

#include <QWidget>

namespace Ui {
class SystemPage;
}

class SystemPage : public QWidget {
    Q_OBJECT

public:
    explicit SystemPage(QWidget *parent = nullptr);
    ~SystemPage();

private:
    Ui::SystemPage *ui;
};

#endif // SYSTEMPAGE_H
