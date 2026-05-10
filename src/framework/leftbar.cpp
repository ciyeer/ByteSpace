#include "leftbar.h"
#include "ui_leftbar.h"

LeftBar::LeftBar(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LeftBar)
{
    ui->setupUi(this);
}

LeftBar::~LeftBar()
{
    delete ui;
}

QComboBox* LeftBar::portComboBox() const { return ui->portComboBox; }
QComboBox* LeftBar::baudrateComboBox() const { return ui->baudrateComboBox; }
QComboBox* LeftBar::dataBitsComboBox() const { return ui->dataBitsComboBox; }
QComboBox* LeftBar::parityComboBox() const { return ui->parityComboBox; }
QComboBox* LeftBar::stopBitsComboBox() const { return ui->stopBitsComboBox; }
QComboBox* LeftBar::flowControlComboBox() const { return ui->flowControlComboBox; }
QPushButton* LeftBar::openCloseButton() const { return ui->openCloseButton; }
QLabel* LeftBar::transferLabel() const { return ui->transferLabel; }
QRadioButton* LeftBar::recvHexBtn() const { return ui->recvHexBtn; }
