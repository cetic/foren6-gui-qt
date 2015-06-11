/*
 * This file is part of Foren6, a 6LoWPAN Diagnosis Tool
 * Copyright (C) 2013, CETIC
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * \file
 *         User Settings Dialog
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#include <QSettings>

#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"
#include "rpl_packet_parser.h"

SettingsDialog::SettingsDialog(QWidget * parent):
QDialog(parent), ui(new Ui::SettingsDialog)
{
    char ipv6string[INET6_ADDRSTRLEN];

    //Get default config
    analyser_config_t config = *(rpl_tool_get_analyser_config());

    inet_ntop(AF_INET6, (const char *)&config.context0, ipv6string, INET6_ADDRSTRLEN);

    //Update config from user preferences
    QSettings settings;

    settings.beginGroup("general");
    config.old_tshark = settings.value("old_tshark", config.old_tshark).toBool();
    settings.endGroup();
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
    config.sender_rank_inverted = settings.value("sender_rank_inverted", config.sender_rank_inverted).toBool();
    settings.endGroup();

    rpl_tool_set_analyser_config(&config);

    ui->setupUi(this);
    restoreSettings();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void
SettingsDialog::applySettings()
{
    bool valid = true;

    //Get current config
    analyser_config_t config = *(rpl_tool_get_analyser_config());

    config.root_rank = ui->settings_RplRootRankEdit->text().toInt(&valid, 10);
    valid = valid && inet_pton(AF_INET6, qPrintable(ui->settings_6lowpanContextEdit->text()), (void *)&config.context0);
    config.old_tshark = ui->settings_OldTsharkCbx->isChecked();
    config.address_autconf_only = ui->settings_AutoconfAddrsCbx->isChecked();
    config.one_preferred_parent = ui->settings_SinglePreferredParentCbx->isChecked();
    config.sender_rank_inverted = ui->settings_SenderRankInvertedCbx->isChecked();
    if(valid) {
        rpl_tool_set_analyser_config(&config);
        //Update user preferences
        QSettings settings;

        settings.beginGroup("general");
        settings.setValue("old_tshark", config.old_tshark);
        settings.endGroup();
        settings.beginGroup("6lowpan");
        settings.setValue("context0", ui->settings_6lowpanContextEdit->text());
        settings.endGroup();
        settings.beginGroup("ipv6");
        settings.setValue("address_autconf_only", config.address_autconf_only);
        settings.endGroup();
        settings.beginGroup("rpl");
        settings.setValue("one_preferred_parent", config.one_preferred_parent);
        settings.setValue("root_rank", config.root_rank);
        settings.setValue("sender_rank_inverted", config.sender_rank_inverted);
        settings.endGroup();
    }
}

void
SettingsDialog::restoreSettings()
{
    char ipv6string[INET6_ADDRSTRLEN];
    const analyser_config_t *config = rpl_tool_get_analyser_config();

    ui->settings_OldTsharkCbx->setChecked(config->old_tshark);
    ui->settings_RplRootRankEdit->setText(QString::number(config->root_rank));
    inet_ntop(AF_INET6, (const char *)&config->context0, ipv6string, INET6_ADDRSTRLEN);
    ui->settings_6lowpanContextEdit->setText(ipv6string);
    ui->settings_AutoconfAddrsCbx->setChecked(config->address_autconf_only);
    ui->settings_SinglePreferredParentCbx->setChecked(config->one_preferred_parent);
    ui->settings_SenderRankInvertedCbx->setChecked(config->sender_rank_inverted);
}
