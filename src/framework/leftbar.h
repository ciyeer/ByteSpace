#ifndef LEFTBAR_H
#define LEFTBAR_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QRadioButton>

namespace Ui {
class LeftBar;
}

class LeftBar : public QWidget {
    Q_OBJECT

public:
    explicit LeftBar(QWidget *parent = nullptr);
    ~LeftBar();

    QComboBox* portComboBox() const;
    QComboBox* baudrateComboBox() const;
    QComboBox* dataBitsComboBox() const;
    QComboBox* parityComboBox() const;
    QComboBox* stopBitsComboBox() const;
    QComboBox* flowControlComboBox() const;
    QPushButton* openCloseButton() const;
    QLabel* transferLabel() const;
    QRadioButton* recvHexBtn() const;

private:
    Ui::LeftBar *ui;
};

#endif // LEFTBAR_H
