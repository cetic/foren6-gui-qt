#include "SnifferDialog.h"
#include "ui_SnifferDialog.h"

#include "OpenedSniffersModel.h"

#include <QFileDialog>
#include <QTableWidgetItem>
#include <QUrl>
#include <QMessageBox>
#include <QDir>

SnifferDialog::SnifferDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SnifferDialog)
{
	ui->setupUi(this);

	openedSniffersModel = new OpenedSniffersModel;
	ui->snifferTable->setModel(openedSniffersModel);

	connect(ui->targetEdit, SIGNAL(returnPressed()), ui->addButton, SLOT(click()));
	connect(ui->addButton, SIGNAL(clicked()), this, SLOT(onAddSniffer()));
	connect(ui->removeButton, SIGNAL(clicked()), this, SLOT(onRemoveSniffer()));
	connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(onBrowseSniffer()));
	connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(accept()));

	QDir captureDir("../capture/bin/");
	QFileInfoList captureInterfaces = captureDir.entryInfoList(QStringList("*.so") << QString("*.dylib"), QDir::Files, QDir::Name);
	QFileInfo captureInterface;

	foreach(captureInterface, captureInterfaces) {
		qDebug("Loading %s", captureInterface.absoluteFilePath().toLatin1().constData());
		loadInterface(captureInterface.absoluteFilePath().toLatin1().constData());
	}
}

SnifferDialog::~SnifferDialog()
{
	delete ui;
}

void SnifferDialog::onAddSniffer() {
	QString snifferType = ui->typeCombo->currentText();
	QString snifferTarget = ui->targetEdit->text();
	int channel = ui->channelSpin->value();

	interface_t *interface;
	ifreader_t sniffer_handle;
	QUrl snifferUrl(QString("%1://%2?channel=%3").arg(snifferType).arg(snifferTarget).arg(channel));

	QString interfaceType = snifferUrl.scheme();
	QString interfacePath = snifferUrl.path();
	int interfaceChanned = snifferUrl.queryItemValue("channel").toInt();

	if(interfaceType.isEmpty())
		interfaceType = interfacePath.section('.', -1, -1);

	interface = (interface_t *) ui->typeCombo->itemData(ui->typeCombo->currentIndex()).value<void*>();

	if(interface == 0) {
		qWarning("Could not get interface !");
		QMessageBox::warning(this, QCoreApplication::applicationName(), QString("Cannot open interface type \"%1\". Known interfaces are pcap and snif.").arg(interfaceType));
		return;
	}

	sniffer_handle = interface->open(interfacePath.toLatin1().constData(), interfaceChanned);

	if(sniffer_handle == 0) {
		qWarning("Could not open target interface !");
		QMessageBox::warning(this, QCoreApplication::applicationName(), QString("Cannot open interface \"%1\" with channel %2. Maybe the file does not exist ?").arg(snifferUrl.path()).arg(interfaceChanned));
		return;
	}

	openedSniffersModel->append(snifferUrl, interface, sniffer_handle);
	ui->snifferTable->resizeColumnsToContents();
}

void SnifferDialog::onRemoveSniffer() {
	QModelIndexList selectedSniffers = ui->snifferTable->selectionModel()->selectedRows();
	QModelIndex sniffer;

	foreach(sniffer, selectedSniffers) {
		openedSniffersModel->remove(sniffer.row());
	}
}

void SnifferDialog::onBrowseSniffer() {
	QString target = QFileDialog::getOpenFileName(this, "Select a sniffer device or a file");

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


void SnifferDialog::onStartSniffer() {
	QList<OpenedSniffersModel::SnifferInterface> openedSniffers = openedSniffersModel->getOpenedSniffers();
	OpenedSniffersModel::SnifferInterface openedSniffer;

	foreach(openedSniffer, openedSniffers) {
		openedSniffer.interface->start(openedSniffer.sniffer_handle);
	}
}

void SnifferDialog::onStopSniffer() {
	QList<OpenedSniffersModel::SnifferInterface> openedSniffers = openedSniffersModel->getOpenedSniffers();
	OpenedSniffersModel::SnifferInterface openedSniffer;

	foreach(openedSniffer, openedSniffers) {
		openedSniffer.interface->stop(openedSniffer.sniffer_handle);
	}
}

bool SnifferDialog::loadInterface(const QString& soLocation) {
	interface_t *currentInterface  = rpl_tool_get_interface(soLocation.toLatin1().constData());

	if(currentInterface == 0)
		return false;

	currentInterface->init();

	ui->typeCombo->insertItem(-1, QString(currentInterface->interface_name), QVariant::fromValue((void*)currentInterface));

	return true;
}
