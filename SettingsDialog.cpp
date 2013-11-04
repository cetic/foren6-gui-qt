#include <QSettings>

#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"
#include "rpl_packet_parser.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    //Get default config
    analyser_config_t config = *(rpl_tool_get_analyser_config());

    //Update config from user preferences
    QSettings settings;
    settings.beginGroup("rpl");
    config.root_rank = settings.value("root_rank", config.root_rank).toInt();
    //context
    settings.endGroup();

    rpl_tool_set_analyser_config(&config);

    ui->setupUi(this);
    restoreSettings();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::applySettings()
{
    bool valid = true;
    //Get current config
    analyser_config_t config = *(rpl_tool_get_analyser_config());

    config.root_rank = ui->settings_RplRootRankEdit->text().toInt(&valid, 10);
    //config. = ui->settings_6lowpanContextEdit->text();

    if (valid) {
        rpl_tool_set_analyser_config(&config);
        //Update user preferences
        QSettings settings;
        settings.beginGroup("rpl");
        settings.setValue("root_rank", config.root_rank);
        settings.endGroup();
    }


}

void SettingsDialog::restoreSettings()
{
    const analyser_config_t *config = rpl_tool_get_analyser_config();
    ui->settings_RplRootRankEdit->setText(QString::number(config->root_rank));
    //ui->settings_6lowpanContextEdit->setText(this->settings_6lowpanContext);
}
