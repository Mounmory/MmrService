#ifndef IPTCVIEWWIDGET_H
#define IPTCVIEWWIDGET_H
#include <QWidget>

class IPTCViewWidget :public QWidget
{
	Q_OBJECT
public:
	IPTCViewWidget(QWidget* parent)
		:QWidget(parent) 
	{
	};
	virtual ~IPTCViewWidget() {};

	//virtual void setThumbnailView(const QString& value) = 0;
	//virtual void setRenderatstartup(const QString& value) = 0;
	virtual void setSourceUrl(const QString& value) = 0;
};

//获取PTCView对象指针
typedef IPTCViewWidget* (*getPTCViewFuc)(QWidget* parent);

#endif //IPTCVIEWWIDGET_H


