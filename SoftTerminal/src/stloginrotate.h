#ifndef _STCONTACTITEM_H_
#define _STCONTACTITEM_H_

#include <QWidget>
#include <QStackedWidget>
#include <QPainter>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include "ui_STLogin.h"
#include "ui_STRegist.h"
#include "ui_STServerConfig.h"
#include "xmppclient.h"
#include "xmppregister.h"
#include "stconfig.h"
#include "stmain.h"
#include "stload.h"

using namespace tahiti;

namespace tahiti
{
	class STLogin : public QWidget
	{
		Q_OBJECT

	public:
		STLogin(XmppClient* client);
		~STLogin();
		void initLoginData();
		static void* loadProc(void* args);
		void setLoadStatus(bool status);
		public Q_SLOTS:
		void onUserNameChanged();
		void on_pbLogin_clicked();
		void on_pb2Config_clicked();
		void on_pb2Regist_clicked();
		void on_pbMinimum_clicked();
		void on_pbClose_clicked();
		private Q_SLOTS:
		void handleLoginResult(bool result);
	protected:
		virtual void mouseMoveEvent(QMouseEvent* event);
		virtual void mousePressEvent(QMouseEvent* event);
		virtual void mouseReleaseEvent(QMouseEvent* event);
		bool eventFilter(QObject *obj, QEvent *e);
	Q_SIGNALS:
		void rotateWindow(int index);
		void changeMainWindow();
	private:
		Ui::STLoginClass ui;
		bool m_isPressed;
		QPoint m_startMovePos;
		XmppClient* m_xmppClient;
		STLoad* m_load;
		pthread_t m_tidLoad;
		QString m_currentUser;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////
	class STRegist : public QWidget
	{
		Q_OBJECT

	public:
		STRegist(QWidget* parent = 0);
		~STRegist();
		void initRegistData();
		public Q_SLOTS:
		void on_pb2Login_clicked();
		void on_pbRegist_clicked();
		void on_pbMinimum_clicked();
		void on_pbClose_clicked();
		private Q_SLOTS:
		void handleRegistResult(bool result);
	protected:
		virtual void mouseMoveEvent(QMouseEvent* event);
		virtual void mousePressEvent(QMouseEvent* event);
		virtual void mouseReleaseEvent(QMouseEvent* event);
		bool eventFilter(QObject *obj, QEvent *e);
	Q_SIGNALS:
		void rotateWindow(int index);
	private:
		Ui::STRegistClass ui;
		bool m_isPressed;
		QPoint m_startMovePos;
		XmppRegister* m_xmppRegister;

	};

	////////////////////////////////////////////////////////////////////////////////////////////////
	class STServerConfig : public QWidget
	{
		Q_OBJECT

	public:
		STServerConfig(QWidget* parent = 0);
		~STServerConfig();
		void initServerConfigData();
		public Q_SLOTS:
		void on_pb2Login_clicked();
		void on_pbConfirm_clicked();
		void on_pbMinimum_clicked();
		void on_pbClose_clicked();
	protected:
		virtual void mouseMoveEvent(QMouseEvent* event);
		virtual void mousePressEvent(QMouseEvent* event);
		virtual void mouseReleaseEvent(QMouseEvent* event);
		bool eventFilter(QObject *obj, QEvent *e);
	Q_SIGNALS:
		void rotateWindow(int index);
	private:
		Ui::STServerConfigClass ui;
		bool m_isPressed;
		QPoint m_startMovePos;

	};

	////////////////////////////////////////////////////////////////////////////////////////////////
	class STLoginRotate : public QStackedWidget
	{
		Q_OBJECT

	public:
		STLoginRotate(XmppClient* client);
		~STLoginRotate();

	private:
		// 初始化旋转的窗口;
		void initRotateWindow();
		// 绘制旋转效果;
		void paintEvent(QPaintEvent* event);

		private Q_SLOTS:
		void onChangeMainWindow();
		void onChangeLoginWindow();
		// 开始旋转窗口;
		void onRotateWindow(int index);
		// 窗口旋转结束;
		void onRotateFinished();

	private:
		// 当前窗口是否正在旋转;
		bool m_isRoratingWindow;
		// 登录窗口
		STLogin* m_loginWindow;
		// 注册窗口
		STRegist* m_registWindow;
		// 服务器配置窗口
		STServerConfig* m_serverConfigWindow;
		int m_nextPageIndex;

		STMain* m_mainWindow;
		XmppClient* m_xmppClient;
	};
}
#endif
