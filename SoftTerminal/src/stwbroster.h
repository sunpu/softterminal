#ifndef _STWBROSTER_H_
#define _STWBROSTER_H_

#include <QDialog>
#include <QMouseEvent>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>
#include "stmessageclient.h"
#include "ui_STWBRoster.h"

namespace tahiti
{
	typedef struct btnInfo
	{
		QString type;
		QString jid;
		bool flag;
		bool enable;
		QPushButton* operateBtn;
		QPushButton* cameraBtn;
		QPushButton* micBtn;
	} BtnInfo;

	class STWBRoster : public QDialog
	{
		Q_OBJECT

	public:
		STWBRoster(QWidget * parent = 0);
		~STWBRoster();
		void initRoster(QString courseID, QString admin, bool isAdmin);
		public Q_SLOTS:
		void on_pbClose_clicked();
		private Q_SLOTS:
		void clearSearchInput();
		void onTextChanged();
		void onBtnClicked();
		void updateAuthorityStatusSlot(QString subtype, QString jid, bool flag);
		void refreshRosterTable();
	private:
		void updateBtnImage(QPushButton* button, BtnInfo info);
	Q_SIGNALS:
		void closeRoster();
		void setAuthorityStatusSignal(QString subtype, QString jid, bool flag);
	protected:
		virtual void mouseMoveEvent(QMouseEvent* event);
		virtual void mousePressEvent(QMouseEvent* event);
		virtual void mouseReleaseEvent(QMouseEvent* event);
		virtual void keyPressEvent(QKeyEvent *event);
		virtual void resizeEvent(QResizeEvent *event);
		bool eventFilter(QObject* obj, QEvent* e);
	private:
		void resizeHeaders();
	private:
		Ui::STWBRosterClass ui;
		bool m_isPressed;
		QPoint m_startMovePos;
		QString m_courseID;
		bool m_isAdmin;
		QPushButton* m_clearBtn;
		STMessageClient* m_messageClient;
		QMap<QPushButton*, BtnInfo> m_btnInfoMap;
	};
}
#endif
