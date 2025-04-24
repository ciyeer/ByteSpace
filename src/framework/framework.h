#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include <QWidget>
#include "baseframework.h"
#include "bytetracebase.h"
#include "settingsui/settingsui.h"
#include "termnite.h"
#include "titlebar.h"
#include "statusbar.h"
#include <QStackedWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class Framework;
}
QT_END_NAMESPACE

class Framework : public BaseFramework {
    Q_OBJECT

public:
    Framework(QWidget *parent = nullptr);
    ~Framework();

protected:
    void closeEvent(QCloseEvent *) override;

// 在 Framework 类的私有成员中添加
private:
    Ui::Framework *ui;
    TitleBar *m_pTitleBar;
    BytetraceBase *m_pBytetraceBase;
    SettingsUI *m_pSettingsUI;
    Termnite *m_pTermnite;
    StatusBar *m_pStatusBar;
    QStackedWidget *m_pStackedWidget; // 添加 QStackedWidget 成员
    
private slots:
    void switchWidget(); // 添加切换界面的槽函数
};
#endif // FRAMEWORK_H
