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
    settings.beginGroup("6lowpan");
    QString context0 = settings.value("context0", ipv6string).toString();
    inet_pton(AF_INET6, qPrintable(context0), (void *)&config.context0);
    settings.endGroup();
    settings.beginGroup("ipv6");
    config.address_autconf_only = settings.value("address_autconf_only", config.address_autconf_only).toBool();
    settings.endGroup();
    settings.beginGroup("rpl");
    config.one_preferred_parent = settings.value("one_preferred_parent", config.one_preferred_parent).toBool();
    config.root_rank = settings.value("root_rank", config.root_rank).toInt();
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
    config.address_autconf_only = ui->settings_AutoconfAddrsCbx->isChecked();
    config.one_preferred_parent = ui->settings_SinglePreferredParentCbx->isChecked();
    if (valid) {
        rpl_tool_set_analyser_config(&config);
        //Update user preferences
        QSettings settings;
        settings.beginGroup("6lowpan");
        settings.setValue("context0", ui->settings_6lowpanContextEdit->text());
        settings.endGroup();
        settings.beginGroup("ipv6");
        settings.setValue("address_autconf_only", config.address_autconf_only);
        settings.endGroup();
        settings.beginGroup("rpl");
        settings.setValue("one_preferred_parent", config.one_preferred_parent);
        settings.setValue("root_rank", config.root_rank);
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
    ui->settings_AutoconfAddrsCbx->setChecked(config->address_autconf_only);
    ui->settings_SinglePreferredParentCbx->setChecked(config->one_preferred_parent);
}
