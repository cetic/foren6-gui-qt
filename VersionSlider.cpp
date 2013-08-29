#include "VersionSlider.h"
#include "ui_VersionSlider.h"
#include <math.h>

#include "data_info/rpl_data.h"

VersionSlider::VersionSlider(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VersionSlider)
{
	ui->setupUi(this);

	settingCurrentVersion = false;

	connect(ui->versionSlider, SIGNAL(valueChanged(int)), this, SLOT(onVersionSliderChange(int)));
	connect(ui->versionSpin, SIGNAL(valueChanged(int)), this, SLOT(onVersionSpinChange(int)));
}

VersionSlider::~VersionSlider()
{
	delete ui;
}

void VersionSlider::setMaximum(int max) {
	maxTimestamp = 0;

	for(int i = 0; i < max; i++) {
		if(maxTimestamp < rpldata_wsn_version_get_timestamp(i))
			maxTimestamp = rpldata_wsn_version_get_timestamp(i);
	}

	settingCurrentVersion = true;
	ui->versionSlider->setMaximum(ceil(maxTimestamp*100));
	ui->versionSpin->setMaximum(max);
	settingCurrentVersion = false;
	ui->versionSpin->setSuffix(QString("/%1").arg(max));
}

void VersionSlider::setValue(int version) {
	double timestamp = rpldata_wsn_version_get_timestamp(version);

	settingCurrentVersion = true;
	ui->versionSlider->setValue(ceil(timestamp*100));
	ui->versionSpin->setValue(version);
	settingCurrentVersion = false;
	ui->currentTimeLabel->setText(QString::number(timestamp));
}

int VersionSlider::maximum() {
	return ui->versionSpin->maximum();
}

int VersionSlider::value() {
	return ui->versionSpin->value();
}

void VersionSlider::onChangeCurrentVersion(int newVersion) {
	if(newVersion == 0) {
		setValue(maximum());
	} else setValue(newVersion);
}

void VersionSlider::onUpdateVersionCount(int versionCount) {
	bool setRealtime, versionChange = false;

	if(value() == maximum())
		setRealtime = true;
	else setRealtime = false;

	if(value() > versionCount || setRealtime)
		versionChange = true;

	setMaximum(versionCount);
	if(setRealtime) {
		setValue(versionCount);
	}

	if(versionChange)
		emit changeWsnVersion(value());
}

void VersionSlider::onVersionSliderChange(int value) {
	int version, i;
	double wantedTimestamp = (double)value / 100.0;

	if(settingCurrentVersion)
		return;

	//i < maximum(): if the timestamp is greater than any version before the last (maximum()) then we don't check for the last and will take that one.
	for(i = 1; i < maximum(); i++) {
		if(rpldata_wsn_version_get_timestamp(i) > wantedTimestamp) {
			if(i > 1 && (wantedTimestamp - rpldata_wsn_version_get_timestamp(i-1)) < (rpldata_wsn_version_get_timestamp(i) - wantedTimestamp))
				i--;
			break;
		}
	}

	version = i;

	setValue(version);

	emit changeWsnVersion(version);
}

void VersionSlider::onVersionSpinChange(int value) {
	int version = qMax(qMin(value, maximum()), 1);

	if(settingCurrentVersion)
		return;

	if(version > maximum())
		version = 0;  //realtime mode
	if(version < 1)
		version = 1;

	if(version == 0)
		setValue(maximum());
	else
		setValue(version);

	emit changeWsnVersion(version);
}
