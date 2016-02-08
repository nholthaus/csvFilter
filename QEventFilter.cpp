#include "QEventFilter.h"

// --------------------------------------------------------------------------------
// 	FUNCTION: QEventFilter (private )
// --------------------------------------------------------------------------------
QEventFilter::QEventFilter(std::function<bool(QObject*, QEvent*)>&& callback) : QObject(), m_callback(std::move(callback))
{

}

// --------------------------------------------------------------------------------
// 	FUNCTION: QEventFilter (private )
// --------------------------------------------------------------------------------
QEventFilter::QEventFilter(const std::function<bool(QObject*, QEvent*)>& callback) : QObject(), m_callback(callback)
{

}
// --------------------------------------------------------------------------------
// 	FUNCTION: ~QEventFilter (public )
// --------------------------------------------------------------------------------
QEventFilter::~QEventFilter()
{

}
// --------------------------------------------------------------------------------
// 	FUNCTION: eventFilter (public )
// --------------------------------------------------------------------------------
bool QEventFilter::eventFilter(QObject* watched, QEvent* event)
{
	return m_callback(watched, event);
}