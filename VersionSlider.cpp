#include "VersionSlider.h"
#include "ui_VersionSlider.h"

#include "data_info/rpl_data.h"

VersionSlider::VersionSlider(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VersionSlider)
{
	ui->setupUi(this);

	connect(ui->versionSlider, SIGNAL(valueChanged(int)), this, SLOT(onVersionSliderMove(int)));
	connect(ui->versionSpin, SIGNAL(valueChanged(int)), this, SLOT(onVersionSliderMove(int)));
}

VersionSlider::~VersionSlider()
{
	delete ui;
}

void VersionSlider::setMaximum(int max) {
	ui->versionSlider->setMaximum(max);
	ui->versionSpin->setMaximum(max);
	ui->versionSpin->setSuffix(QString("/%1").arg(max));
}

void VersionSlider::setValue(int version) {
	ui->versionSlider->setValue(version);
	ui->versionSpin->setValue(version);
	double timestamp = rpldata_wsn_version_get_timestamp(version);
	ui->currentTimeLabel->setText(QString::number(timestamp));
}

int VersionSlider::maximum() {
	return ui->versionSlider->maximum();
}

int VersionSlider::value() {
	return ui->versionSlider->value();
}

void VersionSlider::onChangeCurrentVersion(int newVersion) {
	if(newVersion == 0) {
		setValue(maximum());
	} else setValue(newVersion);
}

void VersionSlider::onUpdateVersionCount(int versionCount) {
	bool setRealtime;

	if(value() == maximum())
		setRealtime = true;
	else setRealtime = false;

	setMaximum(versionCount);
	if(setRealtime) {
		setValue(versionCount);
	}
}

void VersionSlider::onVersionSliderMove(int value) {
	int version = qMax(qMin(value, maximum()), 1);

	if(version > maximum())
		version = 0;  //realtime mode
	if(version < 1)
		version = 1;

	setValue(version);

	emit changeWsnVersion(version);
}
