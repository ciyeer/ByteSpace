#ifndef ACCOUNTPAGE_H
#define ACCOUNTPAGE_H

#include <QWidget>

namespace Ui {
class AccountPage;
}

class AccountPage : public QWidget {
    Q_OBJECT

public:
    explicit AccountPage(QWidget *parent = nullptr);
    ~AccountPage();

private:
    Ui::AccountPage *ui;
};

#endif // ACCOUNTPAGE_H
