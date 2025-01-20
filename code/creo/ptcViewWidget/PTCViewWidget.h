#ifndef PTCVIEWWIDGET_H
#define PTCVIEWWIDGET_H

#include "creo/creoCommon/interface/IPTCViewWidget.h"
#include "creo/ptcViewWidget/pviewlib.h"
#include <QVBoxLayout>
#include <memory>

class PTCViewWidget : public IPTCViewWidget
{
	Q_OBJECT
public:
	PTCViewWidget(QWidget* parent = nullptr);
	~PTCViewWidget();
	//virtual void setThumbnailView(const QString& value) override;
	//virtual void setRenderatstartup(const QString& value) override;
	virtual void setSourceUrl(const QString& value) override;
private:
	pviewLib::pview*			m_pView;
	QVBoxLayout*				m_layOut;
};

#endif




