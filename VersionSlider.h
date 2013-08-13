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
	void setValue(int newValue);
	int maximum();
	int value();

public slots:
	void onChangeCurrentVersion(int newVersion);
	void onUpdateVersionCount(int versionCount);

signals:
	void changeWsnVersion(int version);

protected slots:
	void onVersionSliderMove(int value);

	
private:
	Ui::VersionSlider *ui;
};

#endif // VERSIONSLIDER_H
