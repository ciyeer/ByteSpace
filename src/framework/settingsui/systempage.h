#ifndef SYSTEMPAGE_H
#define SYSTEMPAGE_H

#include <QWidget>

class SystemPage : public QWidget {
    Q_OBJECT

public:
    explicit SystemPage(QWidget *parent = nullptr);
    ~SystemPage();
};

#endif // SYSTEMPAGE_H
