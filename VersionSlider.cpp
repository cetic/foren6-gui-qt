#include "VersionSlider.h"
#include "ui_VersionSlider.h"
#include <math.h>

#include "data_info/rpl_data.h"

VersionSlider::VersionSlider(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VersionSlider),
    showUpdate(false),
    incrTime(false),
    currentTimestamp(0),
    currentMaxTimestamp(0),
    maxTimestamp(0)
{
	ui->setupUi(this);

	settingCurrentVersion = false;

	connect(ui->versionSlider, SIGNAL(valueChanged(int)), this, SLOT(onVersionSliderChange(int)));
	connect(ui->versionSpin, SIGNAL(valueChanged(int)), this, SLOT(onVersionSpinChange(int)));

	setRealtime();
}

VersionSlider::~VersionSlider()
{
	delete ui;
}

void VersionSlider::updateTime(void) {
    if (showUpdate) {
        currentMaxTimestamp = maxTimestamp;
    }
    if(currentMaxTimestamp > 20000000)
        currentMaxTimestamp = 20000000;
    if(currentTimestamp > 20000000)
        currentTimestamp = 20000000;
    if ( realtime() ) {
        currentTimestamp = currentMaxTimestamp;
    }
    ui->versionSlider->setMaximum(ceil(currentMaxTimestamp*100));
    ui->versionSlider->setValue(ceil(currentTimestamp*100));
    QString text = realtime() ? "(R) " : "";
    text += QString::number(currentTimestamp);
    ui->currentTimeLabel->setText(text);
}

void VersionSlider::setMaximum(int max) {
	for(int i = 0; i < max; i++) {
		if(maxTimestamp < rpldata_wsn_version_get_timestamp(i))
			maxTimestamp = rpldata_wsn_version_get_timestamp(i);
	}

	settingCurrentVersion = true;
	ui->versionSpin->setMaximum(max + 1);
    updateTime();
	settingCurrentVersion = false;
	ui->versionSpin->setSuffix(QString("/%1").arg(max + 1));
}

void VersionSlider::setValue(int version) {
    if ( version > maximum() ) {
        version = maximum();
    }
	currentTimestamp = rpldata_wsn_version_get_timestamp(version);
	settingCurrentVersion = true;
	ui->versionSpin->setValue(version);
	updateTime();
	settingCurrentVersion = false;
}

void VersionSlider::setRealtime() {
    currentTimestamp = currentMaxTimestamp;
    settingCurrentVersion = true;
    ui->versionSpin->setValue(ui->versionSpin->maximum());
    updateTime();
    settingCurrentVersion = false;
}

int VersionSlider::maximum() {
	return ui->versionSpin->maximum() - 1;
}

int VersionSlider::value() {
    if ( realtime() ) {
        return ui->versionSpin->maximum() - 1;
    } else {
        return ui->versionSpin->value();
    }
}

bool VersionSlider::realtime() {
    return ui->versionSpin->value() == ui->versionSpin->maximum();
}
void VersionSlider::onChangeCurrentVersion(int newVersion) {
	if(newVersion == 0) {
		setRealtime();
	} else {
	    setValue(newVersion);
	}

	emit changeWsnVersion(value());
}

void VersionSlider::onUpdateVersionCount(int versionCount) {
	bool versionChange = false;
	bool inRealtime = realtime();

	if(value() > versionCount || inRealtime)
		versionChange = true;

	setMaximum(versionCount);
	if(inRealtime) {
		 setRealtime();
	}

	if(versionChange)
		emit changeWsnVersion(value());
}

void VersionSlider::onTimeTick(int msec) {
    if (!incrTime) return;
    maxTimestamp += msec / 1000.0;
    updateTime();
}

void VersionSlider::clearTime() {
    incrTime = false;
    maxTimestamp = 0;
    currentMaxTimestamp = 0;
    currentTimestamp = 0;
}

void VersionSlider::startTime() {
    showUpdate = true;
    incrTime = true;
}

void VersionSlider::stopTime() {
    showUpdate = false;
}

void VersionSlider::onVersionSliderChange(int newValue) {
	int version, i;
	double wantedTimestamp = (double)newValue / 100.0;

	if(settingCurrentVersion)
		return;

	//i < maximum(): if the timestamp is greater than any version before the last (maximum()) then we don't check for the last and will take that one.
	for(i = 1; i <= maximum(); i++) {
		if(rpldata_wsn_version_get_timestamp(i) > wantedTimestamp) {
			if(i > 1 && (wantedTimestamp - rpldata_wsn_version_get_timestamp(i-1)) < (rpldata_wsn_version_get_timestamp(i) - wantedTimestamp))
				i--;
			break;
		}
	}
	if ( i > maximum() ) {
	    setRealtime();
	} else {
	    version = i;
	    setValue(version);
	}

	emit changeWsnVersion(value());
}

void VersionSlider::onVersionSpinChange(int newValue) {
	int version = qMax(qMin(newValue, maximum() + 1), 1);

	if(settingCurrentVersion)
		return;

	if (version > maximum() ) {
	    setRealtime();
	} else {
        setValue(version);
	}
	emit changeWsnVersion(value());
}
