#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    this->settings_RplRootRank = "256";
    this->settings_6lowpanContext = "aaaa::";
    ui->setupUi(this);
    ui->settings_RplRootRankEdit->setText(this->settings_RplRootRank);
    ui->settings_6lowpanContextEdit->setText(this->settings_6lowpanContext);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::applySettings()
{
    this->settings_RplRootRank = ui->settings_RplRootRankEdit->text();
    this->settings_6lowpanContext = ui->settings_6lowpanContextEdit->text();
}

void SettingsDialog::restoreSettings()
{
    ui->settings_RplRootRankEdit->setText(this->settings_RplRootRank);
    ui->settings_6lowpanContextEdit->setText(this->settings_6lowpanContext);
}

QString SettingsDialog::getRplRootRank()
{
    return settings_RplRootRank;
}

QString SettingsDialog::get6lowpanContext()
{
    return settings_6lowpanContext;
}
