#ifndef VERSIONSLIDER_H
#define VERSIONSLIDER_H

#include <QWidget>

namespace Ui {
class VersionSlider;
}

class VersionSlider : public QWidget
{
	Q_OBJECT
	
public:
	explicit VersionSlider(QWidget *parent = 0);
	~VersionSlider();

	void setMaximum(int max);
	void setValue(int version);
	int maximum();
	int value();
	void clearTime();
	void startTime();
	void stopTime();

public slots:
	void onChangeCurrentVersion(int newVersion);
	void onUpdateVersionCount(int versionCount);
	void onTimeTick(int msec);

signals:
	void changeWsnVersion(int version);

protected:
	void updateTime(void);

protected slots:
	void onVersionSliderChange(int value);
	void onVersionSpinChange(int value);

private:
	Ui::VersionSlider *ui;
	bool showUpdate;
	bool incrTime;
	double currentTimestamp;
	double currentMaxTimestamp;
	double maxTimestamp;
	bool settingCurrentVersion;  //avoid recursive loop setValue -> onVersion*Change -> setValue ...
};

#endif // VERSIONSLIDER_H
