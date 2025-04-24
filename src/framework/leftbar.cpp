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
