#ifndef SNIFFERDIALOG_H
#define SNIFFERDIALOG_H

#include <QDialog>
#include <QUrl>
#include "rpl_packet_parser.h"

namespace Ui {
class SnifferDialog;
}

class OpenedSniffersModel;

class SnifferDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit SnifferDialog(QWidget *parent = 0);
	~SnifferDialog();

	void onStartSniffer();
	void onStopSniffer();
	bool loadInterface(const QString& soLocation);
    bool activeSniffersCount();

protected slots:
	void onAddSniffer();
	void onRemoveSniffer();
	void onBrowseSniffer();
	void onCloseDialog();

protected:
	void doAddSniffer(QUrl snifferUrl);

private:
	Ui::SnifferDialog *ui;
	OpenedSniffersModel *openedSniffersModel;
};

#endif // SNIFFERDIALOG_H
