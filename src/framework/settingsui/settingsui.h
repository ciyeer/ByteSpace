#ifndef SETTINGSUI_H
#define SETTINGSUI_H

#include <QWidget>
#include "aboutpage.h"
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

    SystemPage* systemPage() const { return m_pSystemPage; }

private slots:
    void onCategoryChanged(int index);

private:
    Ui::SettingsUI *ui;
    AboutPage       *m_pAboutPage;
    AppearancePage  *m_pAppearancePage;
    HelpPage        *m_pHelpPage;
    SystemPage      *m_pSystemPage;
};

#endif // SETTINGSUI_H
