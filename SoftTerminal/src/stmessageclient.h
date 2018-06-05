#ifndef _STNETWORKCLIENT_H
#define _STNETWORKCLIENT_H

#include <QObject>
#include <QThread>
#include <pthread.h>
#include "zmq.hpp"
#include "mdcliapi.hpp"
#include "zhelpers.hpp"
#include "stcommon.h"
#include "stconfig.h"

using namespace std;
using namespace zmq;

namespace tahiti
{
	class STMessageClient : public QObject
	{
		Q_OBJECT

	public:
		STMessageClient(QObject * parent = 0);
		~STMessageClient();
		QString sendMessage(QString data);
		void subscribeMessage(QString filter = "");
		void unsubscribeMessage();
		static void* connectProc(void* args);
	private:
		void processMessage(QString message);
		void readMessage();
	Q_SIGNALS:
		void cloudFileMessageSignal(QString message);
		void whiteBoardMessageSignal(QString message);
	private:
		socket_t* m_subscriber;
		pthread_t m_tidConnect;
		QString m_filter;
	};
}

#endif
