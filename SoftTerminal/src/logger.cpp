#include "logger.h"

using namespace tahiti;

Logger STLogger::_logger = Logger::getInstance(LOG4CPLUS_TEXT("main_log"));

STLogger::STLogger()
{
}

STLogger::~STLogger()
{
}

STLogger& STLogger::instance()
{
	static STLogger log;
	return log;
}

bool STLogger::open_log(const wchar_t* logfile)
{
	/* step 1: Instantiate an appender object */
	SharedAppenderPtr _append(new RollingFileAppender(logfile, 5 * 1024 * 1024, 3));
	_append->setName(LOG4CPLUS_TEXT("st log"));

	/* step 2: Instantiate a layout object */
	auto_ptr<Layout> _layout(new PatternLayout(LOG4CPLUS_TEXT("%D{%Y/%m/%d %H:%M:%S,%Q}[%l][%p]:%m%n")));
	//auto_ptr<Layout> _layout(new TTCCLayout());

	/* step 3: Attach the layout object to the appender */
	_append->setLayout(_layout);

	/* step 4: Instantiate a logger object */

	/* step 5: Attach the appender object to the logger  */
	STLogger::_logger.addAppender(_append);

	/* step 6: Set a priority for the logger  */
	STLogger::_logger.setLogLevel(NOT_SET_LOG_LEVEL);

	return true;
}
