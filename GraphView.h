#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

#include <QWidget>
#include "rpl/rplNetworkInfoManager.h"


namespace Ui {
class GraphView;
}

class GraphView : public QWidget
{
	Q_OBJECT
	
public:
	explicit GraphView(QWidget *parent = 0);
	~GraphView();
	void setNetworkManager(rpl::NetworkInfoManager *networkMgr);

protected slots:
	void onLayoutComboChange(int index);
	
private:
	Ui::GraphView *ui;
	rpl::NetworkInfoManager *networkMgr;
};

#endif // GRAPHVIEW_H
