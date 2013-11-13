/*
 * Copyright (c) 2013, CETIC.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
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
    QString snifferType = ui->typeCombo->currentText();
    QString snifferTarget = ui->targetEdit->text();
    int channel = ui->channelSpin->value();

    QUrl snifferUrl(QString("%1://%2?channel=%3").arg(snifferType).arg(snifferTarget).arg(channel));

    doAddSniffer(snifferUrl);
}

void
SnifferDialog::doAddSniffer(QUrl snifferUrl)
{
    interface_t *interface;
    ifreader_t sniffer_handle;

    QString interfaceType = snifferUrl.scheme();
    QString interfacePath = snifferUrl.path();

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    int interfaceChanned = QUrlQuery(snifferUrl).queryItemValue("channel").toInt();
#else
    int interfaceChanned = snifferUrl.queryItemValue("channel").toInt();
#endif

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

    sniffer_handle = interface->open(interfacePath.toLatin1().constData(), interfaceChanned);

    if(sniffer_handle == 0) {
        qWarning("Could not open target interface !");
        QMessageBox::warning(this, QCoreApplication::applicationName(),
                             QString("Cannot open interface \"%1\" with channel %2. Maybe the file does not exist ?").arg(snifferUrl.path()).
                             arg(interfaceChanned));
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
    QString target = QFileDialog::getOpenFileName(this, "Select a sniffer device or a file");

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
