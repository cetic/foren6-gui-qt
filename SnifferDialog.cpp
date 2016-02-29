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
 *         Sources Selection Dialog & Associated Information
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#include "SnifferDialog.h"
#include "ui_SnifferDialog.h"

#include "OpenedSniffersModel.h"

#include <QFileDialog>
#include <QTableWidgetItem>
#include <QUrl>
#include <QMessageBox>
#include <QDir>
#include <QSettings>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QUrlQuery>
#endif

SnifferDialog::SnifferDialog(QWidget * parent):
QDialog(parent), ui(new Ui::SnifferDialog)
{
    ui->setupUi(this);

    openedSniffersModel = new OpenedSniffersModel;
    ui->snifferTable->setModel(openedSniffersModel);

    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(onAddSniffer()));
    connect(ui->removeButton, SIGNAL(clicked()), this, SLOT(onRemoveSniffer()));
    connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(onBrowseSniffer()));
    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(onCloseDialog()));
    connect(ui->typeCombo, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(onSelectType(const QString &)));

    QFileInfoList captureInterfaces;
    QDir searchPath;
    QStringList extensions;

    extensions << QString("*.so") << QString("*.dylib");

    searchPath = QDir("/usr/lib/foren6/interfaces");
    if(!searchPath.exists())
        searchPath = QDir(QApplication::applicationDirPath() + "/capture");
    if(!searchPath.exists())
        searchPath = QDir(QApplication::applicationDirPath());

    captureInterfaces.append(searchPath.entryInfoList(extensions, QDir::Files, QDir::Name));

    QFileInfo captureInterface;

    foreach(captureInterface, captureInterfaces) {
        qDebug("Loading %s", captureInterface.absoluteFilePath().toLatin1().constData());
        loadInterface(captureInterface.absoluteFilePath().toLatin1().constData());
    }

    QSettings settings;

    settings.beginGroup("Sniffers");
    int size = settings.beginReadArray("sources");

    for(int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        doAddSniffer(QUrl(settings.value("url").toString()));
    }
    settings.endArray();
    settings.endGroup();
}

SnifferDialog::~SnifferDialog()
{
    delete ui;
}

void
SnifferDialog::onCloseDialog()
{
    QSettings settings;

    settings.beginGroup("Sniffers");
    settings.beginWriteArray("sources");
    for(int i = 0; i < openedSniffersModel->getOpenedSniffers().count(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("url", openedSniffersModel->getOpenedSniffers().at(i).url.toString());
    }
    settings.endArray();
    settings.endGroup();
    accept();
}

void
SnifferDialog::onAddSniffer()
{
    int parameters = 0;
    QString snifferType = ui->typeCombo->currentText();
    QString tmp = snifferType + "://";
    if (ui->targetEdit->isEnabled()) {
      QString snifferTarget = ui->targetEdit->text();
      tmp += snifferTarget;
    }
    if (ui->channelSpin->isEnabled()) {
      int channel = ui->channelSpin->value();
      if (parameters++ == 0) tmp += '?'; else tmp += '&';
      tmp += "channel=";
      tmp += QString::number(channel);
    }
    if (ui->baudrateCombo->isEnabled()) {
      int baudrate = ui->baudrateCombo->currentText().toInt();
      if (parameters++ == 0) tmp += '?'; else tmp += '&';
      tmp += "baudrate=";
      tmp += QString::number(baudrate);
    }
    QUrl snifferUrl(tmp);

    doAddSniffer(snifferUrl);
}

void
SnifferDialog::doAddSniffer(QUrl snifferUrl)
{
    interface_t *interface;
    ifreader_t sniffer_handle;

    QString interfaceType = snifferUrl.scheme();
    QString interfacePath = snifferUrl.path();

    int interfaceChannel;
    int interfaceBaudrate;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QUrlQuery query(snifferUrl);
#else
    QUrl &query = snifferUrl;
#endif
    if (query.hasQueryItem("channel")) {
        interfaceChannel = query.queryItemValue("channel").toInt();
    } else {
        interfaceChannel = 26;
    }
    if (query.hasQueryItem("baudrate")) {
        interfaceBaudrate = query.queryItemValue("baudrate").toInt();
    } else {
        interfaceBaudrate = 115200;
    }

    if(interfaceType.isEmpty())
        interfaceType = interfacePath.section('.', -1, -1);

    int index = ui->typeCombo->findText(interfaceType);
    interface = (interface_t *) ui->typeCombo->itemData(index).value < void *>();

    if(interface == 0) {
        qWarning("Could not get interface !");
        QMessageBox::warning(this, QCoreApplication::applicationName(),
                             QString("Cannot open interface type \"%1\". Choose an interface type before loading a file.").arg(interfaceType));
        return;
    }

    sniffer_handle = interface->open(interfacePath.toLatin1().constData(), interfaceChannel, interfaceBaudrate);

    if(sniffer_handle == 0) {
        qWarning("Could not open target interface !");
        QMessageBox::warning(this, QCoreApplication::applicationName(),
                             QString("Cannot open device \"%1\". Maybe the file does not exist ?").arg(snifferUrl.path()));
        return;
    }

    openedSniffersModel->append(snifferUrl, interface, sniffer_handle);
    ui->snifferTable->resizeColumnsToContents();
}

void
SnifferDialog::onRemoveSniffer()
{
    QModelIndexList selectedSniffers = ui->snifferTable->selectionModel()->selectedRows();
    QModelIndex sniffer;

    foreach(sniffer, selectedSniffers) {
        interface_t *iface = openedSniffersModel->getInterface(sniffer.row());
        ifreader_t sniffer_handle = openedSniffersModel->getHandle(sniffer.row());

        iface->close(sniffer_handle);
        openedSniffersModel->remove(sniffer.row());
    }
}

void
SnifferDialog::onBrowseSniffer()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setFilter(QDir::System | QDir::AllEntries);
    QString target;
    if (dialog.exec()) {
        QStringList selectedFiles = dialog.selectedFiles();
        target = selectedFiles.at(0);
    }

    if(target.isEmpty())
        return;

    ui->targetEdit->setText(target);

    int index = target.lastIndexOf('.');

    if(index != -1) {
        index = target.size() - index - 1;
        QString targetExt = target.right(index);
        int i;

        for(i = 0; i < ui->typeCombo->count(); i++) {
            if(ui->typeCombo->itemText(i) == targetExt) {
                ui->typeCombo->setCurrentIndex(i);
                break;
            }
        }
    }
}

void
SnifferDialog::onSelectType(const QString & text)
{
  int index = ui->typeCombo->findText(text);
  interface_t *interface = (interface_t *) ui->typeCombo->itemData(index).value < void *>();
  ui->targetEdit->setEnabled((interface->parameters & INTERFACE_DEVICE) != 0);
  ui->browseButton->setEnabled((interface->parameters & INTERFACE_DEVICE) != 0);
  ui->channelSpin->setEnabled((interface->parameters & INTERFACE_CHANNEL) != 0);
  ui->baudrateCombo->setEnabled((interface->parameters & INTERFACE_BAUDRATE) != 0);
}

void
SnifferDialog::onStartSniffer()
{
    QList < OpenedSniffersModel::SnifferInterface > openedSniffers = openedSniffersModel->getOpenedSniffers();
    OpenedSniffersModel::SnifferInterface openedSniffer;

    rpl_tool_start_capture();
    foreach(openedSniffer, openedSniffers) {
        openedSniffer.interface->start(openedSniffer.sniffer_handle);
    }
}

void
SnifferDialog::onStopSniffer()
{
    QList < OpenedSniffersModel::SnifferInterface > openedSniffers = openedSniffersModel->getOpenedSniffers();
    OpenedSniffersModel::SnifferInterface openedSniffer;

    foreach(openedSniffer, openedSniffers) {
        openedSniffer.interface->stop(openedSniffer.sniffer_handle);
    }
    rpl_tool_stop_capture();
}

bool
SnifferDialog::loadInterface(const QString & soLocation)
{
    interface_t *currentInterface = rpl_tool_get_interface(soLocation.toLatin1().constData());

    if(currentInterface == 0)
        return false;

    currentInterface->init();

    ui->typeCombo->insertItem(-1, QString(currentInterface->interface_name), QVariant::fromValue((void *)currentInterface));

    return true;
}

bool
SnifferDialog::activeSniffersCount()
{
    QList < OpenedSniffersModel::SnifferInterface > openedSniffers = openedSniffersModel->getOpenedSniffers();
    return openedSniffers.count();
}
