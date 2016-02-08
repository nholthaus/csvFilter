//--------------------------------------------------------------------------------------------------
//	
//	Passive Correlator
//	Group 108 - Tactical Defense Systems
//	MIT Lincoln Laboratory
//	244 Wood St.
//	Lexington, MA 02420-9108
//	
//--------------------------------------------------------------------------------------------------
//
///	@file			QEventFilter.h
///	@brief			A generic event filter which takes a functor as a callback
/// @details		Can be installed on Qt classes as a general event filter so that a new class 
///					doesn't have to be derived for every new filter.
//
//--------------------------------------------------------------------------------------------------
//
///	@author			Nic Holthaus
/// @date			September 17, 2015
//
//--------------------------------------------------------------------------------------------------

#ifndef QEventFilter_h__
#define QEventFilter_h__

//------------------------
//	INCLUDES
//------------------------
#include <QEvent>
#include <QObject>

#include <functional>

//	----------------------------------------------------------------------------
//	CLASS		QEventFilter
//  ----------------------------------------------------------------------------
///	@brief		A generic event filter which takes a functor as a callback
///	@details	
//  ----------------------------------------------------------------------------
class QEventFilter : public QObject
{
	Q_OBJECT

public:

	explicit QEventFilter(const std::function<bool(QObject*, QEvent*)>& callback);
	explicit QEventFilter(std::function<bool(QObject*, QEvent*)>&& callback);
	virtual ~QEventFilter();

	virtual bool eventFilter(QObject* watched, QEvent* event) override;
	
private:

	std::function<bool(QObject*,QEvent*)>		m_callback;											///< The callback functor to invoke when the event occurs												

};

#endif // QEventFilter_h__