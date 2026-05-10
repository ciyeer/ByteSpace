/**
 * @file framework.h
 * @brief 主窗口框架
 *
 * 应用程序主界面，组合 TitleBar、BytetraceBase、SettingsUI、StatusBar。
 *
 * @author ByteSpace团队
 * @date 2024
 */
#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include <QWidget>
#include "baseframework.h"
#include "bytetracebase.h"
#include "settingsui/settingsui.h"
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
    StatusBar *m_pStatusBar;
    QStackedWidget *m_pStackedWidget; // 添加 QStackedWidget 成员
    
private slots:
    void switchWidget(); // 添加切换界面的槽函数
};
#endif // FRAMEWORK_H
