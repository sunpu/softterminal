#ifndef _STSCREENSHOT_H  
#define _STSCREENSHOT_H  

#include <memory>
#include <QRect>
#include <QWidget>
#include <QDesktopWidget>
#include <QApplication>
#include <QMouseEvent>
#include <QFileDialog>
#include <QClipboard>
#include <QDateTime>
#include <QPainter>
#include <QScreen>
#include <QCursor>
#include <QMutex>
#include <QMenu>
#include <QPen>
#include <QStandardPaths>
#include <windows.h>
#include <stcommon.h>
#include "ui_STScreenshotToolbar.h"

namespace tahiti
{
	const int PADDING = 6;
	enum DIRECTION
	{
		UPPER = 0,
		LOWER = 1,
		LEFT,
		RIGHT,
		LEFTUPPER,
		LEFTLOWER,
		RIGHTLOWER,
		RIGHTUPPER,
		NONE
	};

	class SSToolbar;
	class SSScreen;
	class SSSizeShow;
	class STScreenshot : public QWidget
	{
		Q_OBJECT
	public:
		explicit STScreenshot(QWidget *parent = 0);
		~STScreenshot(void);

	Q_SIGNALS:
		void cursorPosChange(int, int);
		void doubleClick(void);
		void findChildWind(QRect);
		void finishScreenshot(QString);
		void cancelScreenshot();
		public Q_SLOTS:
		void onFinishScreenshot(QString);
		void onCancelScreenshot();
	protected:
		virtual void hideEvent(QHideEvent *);
		virtual void closeEvent(QCloseEvent *);
		virtual void mouseDoubleClickEvent(QMouseEvent*);
		virtual void mousePressEvent(QMouseEvent *);
		virtual void mouseReleaseEvent(QMouseEvent *e);
		virtual void mouseMoveEvent(QMouseEvent *e);
		virtual void keyPressEvent(QKeyEvent *e);
		virtual void paintEvent(QPaintEvent *);
		void updateMouse(void);

	private:
		void initMeasureWidget(void);
		std::shared_ptr<QPixmap> initGlobalScreen(void);
		std::shared_ptr<SSScreen> createScreen(const QPoint &pos);
		void destroyScreen(void);
		const QRect& getScreenRect(void);
		std::shared_ptr<QPixmap> getGlobalScreen(void);

		private Q_SLOTS:
		void onEgoistic(void);

	private:
		bool m_isLeftPressed;
		QPoint m_startPoint;
		QRect m_desktopRect;
		std::shared_ptr<QPixmap> m_backgroundScreen;
		std::shared_ptr<QPixmap> m_originPainting;
		std::shared_ptr<SSScreen> m_screen;
		std::shared_ptr<SSSizeShow> m_sizeShow;
		QRect m_windowRect;
		static bool isActivity;
		SSToolbar* m_toolbar;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////
	class SSSizeShow : public QWidget
	{
		Q_OBJECT

	public:
		explicit SSSizeShow(QWidget *parent = 0);

	protected:
		void paintEvent(QPaintEvent *);
		public Q_SLOTS:
		void onPostionChange(int x, int y);
		void onSizeChange(int w, int h);

	private:
		QString m_info;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////
	class SSToolbar : public QWidget
	{
		Q_OBJECT

	public:
		explicit SSToolbar(QWidget *parent = 0);
		~SSToolbar();
	Q_SIGNALS:
		void finishScreenshot();
		void cancelScreenshot();

		public Q_SLOTS:
		void on_pbFinish_clicked();
		void on_pbCancel_clicked();
		void onPosChange(int x, int y);
	private:
		Ui::STScreenshotToolbarClass ui;
		SSScreen* screen;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////
	class SSScreen : public QWidget
	{

		Q_OBJECT

	public:
		explicit SSScreen(std::shared_ptr<QPixmap> originPainting, QPoint pos, SSToolbar* toolbar, QWidget *parent = 0);
		~SSScreen() { isInit = false; }
		bool state(void) { return isInit; }

	Q_SIGNALS:
		void posChange(int, int);
		void sizeChange(int, int);
		void postionChange(int, int);
		void doubleClick(void);
		void finishScreenshot(QString);
		void cancelScreenshot();
	protected:
		DIRECTION getRegion(const QPoint &cursor);
		virtual void contextMenuEvent(QContextMenuEvent *);
		virtual void mouseDoubleClickEvent(QMouseEvent *e);
		virtual void mousePressEvent(QMouseEvent *e);
		virtual void mouseReleaseEvent(QMouseEvent *e);
		virtual void mouseMoveEvent(QMouseEvent *e);
		virtual void moveEvent(QMoveEvent *);
		virtual void resizeEvent(QResizeEvent *);
		virtual void showEvent(QShowEvent *);
		virtual void hideEvent(QHideEvent *);
		virtual void enterEvent(QEvent *e);
		virtual void leaveEvent(QEvent *e);
		virtual void closeEvent(QCloseEvent *);
		virtual void paintEvent(QPaintEvent *);

	private:
		virtual const QString getFileName(void);
		QString getRandString();

		public Q_SLOTS:
		void onMouseChange(int x, int y);
		void onSaveScreen(void);
		void onSaveScreenOther(void);
		void quitScreenshot(void);

	private:
		static bool isInit;
		DIRECTION m_direction;
		QPoint m_originPoint;
		bool m_isPressed;
		QPoint m_movePos;
		std::shared_ptr<QPixmap> m_originPainting;
		QRect m_currentRect;
		SSToolbar* m_toolbar;
	};
}
#endif
