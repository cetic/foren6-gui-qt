#include <QSettings>

#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"
#include "rpl_packet_parser.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    char ipv6string[INET6_ADDRSTRLEN];
    //Get default config
    analyser_config_t config = *(rpl_tool_get_analyser_config());
    inet_ntop(AF_INET6, (const char*)&config.context0, ipv6string, INET6_ADDRSTRLEN);

    //Update config from user preferences
    QSettings settings;
    settings.beginGroup("rpl");
    config.root_rank = settings.value("root_rank", config.root_rank).toInt();
    QString context0 = settings.value("context0", ipv6string).toString();
    inet_pton(AF_INET6, qPrintable(context0), (void *)&config.context0);
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
    valid = valid && inet_pton(AF_INET6, qPrintable(ui->settings_6lowpanContextEdit->text()), (void *)&config.context0);

    if (valid) {
        rpl_tool_set_analyser_config(&config);
        //Update user preferences
        QSettings settings;
        settings.beginGroup("rpl");
        settings.setValue("root_rank", config.root_rank);
        settings.setValue("context0", ui->settings_6lowpanContextEdit->text());
        settings.endGroup();
    }


}

void SettingsDialog::restoreSettings()
{
    char ipv6string[INET6_ADDRSTRLEN];
    const analyser_config_t *config = rpl_tool_get_analyser_config();
    ui->settings_RplRootRankEdit->setText(QString::number(config->root_rank));
    inet_ntop(AF_INET6, (const char*)&config->context0, ipv6string, INET6_ADDRSTRLEN);
    ui->settings_6lowpanContextEdit->setText(ipv6string);
}
