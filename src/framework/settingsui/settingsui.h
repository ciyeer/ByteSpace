#ifndef SETTINGSUI_H
#define SETTINGSUI_H

#include <QWidget>
#include "aboutpage.h"
#include "accountpage.h"
#include "appearancepage.h"
#include "helppage.h"
#include "systempage.h"

namespace Ui {
class SettingsUI;
}

class SettingsUI : public QWidget {
    Q_OBJECT

public:
    explicit SettingsUI(QWidget *parent = nullptr);
    ~SettingsUI();

private slots:
    void onCategoryChanged(int index);

private:
    Ui::SettingsUI *ui;
    AboutPage       *m_pAboutPage;
    AccountPage     *m_pAccountPage;
    AppearancePage  *m_pAppearancePage;
    HelpPage        *m_pHelpPage;
    SystemPage      *m_pSystemPage;
};

#endif // SETTINGSUI_H
