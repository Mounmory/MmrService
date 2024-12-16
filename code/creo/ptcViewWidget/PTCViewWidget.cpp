#include "PTCViewWidget.h"
#include <iostream>

# if defined(PTCVIEWWIDGET_LIB)
#  define PTCVIEWWIDGET_EXPORT __declspec(dllexport)
# else
#  define PTCVIEWWIDGET_EXPORT __declspec(dllimport)
# endif

PTCViewWidget::PTCViewWidget(QWidget* parent):
	IPTCViewWidget(parent)
{
	m_pView = new pviewLib::pview;
	m_pView->setThumbnailView("true");
	m_pView->setRenderatstartup("true");

	m_layOut = new QVBoxLayout;
	m_layOut->addWidget(m_pView);
	m_layOut->setContentsMargins(0, 0, 0, 0);//无间隙
	this->setLayout(m_layOut);

	//std::cout << " PTCViewWidget construct！ " << std::endl;
}

PTCViewWidget::~PTCViewWidget()
{
	if (nullptr != m_pView)
	{
		delete m_pView;
		m_pView = nullptr;
	}
	if (nullptr != m_layOut)
	{
		delete m_layOut;
		m_layOut = nullptr;
	}
	//std::cout << " PTCViewWidget destruct！ " << std::endl;
}

//void PTCViewWidget::setThumbnailView(const QString& value)
//{
//	//std::cout << " setThumbnailView:value is " << value.toStdString().c_str() << std::endl;
//	m_pView->setThumbnailView(value);
//}
//
//void PTCViewWidget::setRenderatstartup(const QString& value)
//{
//	//std::cout << " setRenderatstartup:value is " << value.toStdString().c_str() << std::endl;
//	m_pView->setRenderatstartup(value);
//}

void PTCViewWidget::setSourceUrl(const QString& value)
{
	//std::cout << " setSourceUrl:value is " << value.toStdString().c_str() << std::endl;
	m_pView->setSourceUrl(value);
}


extern "C" PTCVIEWWIDGET_EXPORT IPTCViewWidget* getPTCView(QWidget* parent) 
{
	return new PTCViewWidget(parent);
}

