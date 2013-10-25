#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();
    void applySettings();
    void restoreSettings();
    QString getRplRootRank();
    QString get6lowpanContext();
    
private:
    Ui::SettingsDialog *ui;
    QString settings_6lowpanContext;
    QString settings_RplRootRank;
};

#endif // SETTINGSDIALOG_H
