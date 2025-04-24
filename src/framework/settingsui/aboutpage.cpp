#include "aboutpage.h"
#include "ui_aboutpage.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>

AboutPage::AboutPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AboutPage) {
    ui->setupUi(this);
    ui->verticalLayout_2->addStretch(1);
}

AboutPage::~AboutPage() {
    delete ui;
}
