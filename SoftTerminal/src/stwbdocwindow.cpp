#include "stwbdocwindow.h"
#include "stwbscene.h"

using namespace tahiti;

STWBDocWindow::STWBDocWindow(QString path, int index, QWidget * parent)
	: m_path(path), m_index(index), QDialog(parent)
{
	ui.setupUi(this);
	//setWindowFlags(Qt::FramelessWindowHint);
	setWindowFlags(Qt::SubWindow);
	ui.widTitle->installEventFilter(this);
	ui.teText->installEventFilter(this);
	ui.widBottom->installEventFilter(this);
	ui.scrollArea->installEventFilter(this);
	m_isMaximum = false;

	QString lowerPath = m_path.toLower();
	if (lowerPath.endsWith(".pptx") || lowerPath.endsWith(".ppt") || lowerPath.endsWith(".pptm")
		|| lowerPath.endsWith(".docx") || lowerPath.endsWith(".doc") || lowerPath.endsWith(".pdf"))
	{
		ui.widBottom->setVisible(true);
		ui.scrollArea->setVisible(true);
		ui.teText->setVisible(false);
	}
	else if (lowerPath.endsWith(".jpeg") || lowerPath.endsWith(".jpg") || lowerPath.endsWith(".png")
		|| lowerPath.endsWith(".bmp"))
	{
		ui.widBottom->setVisible(false);
		ui.scrollArea->setVisible(true);
		ui.teText->setVisible(false);
	}
	else if (lowerPath.endsWith(".txt"))
	{
		ui.widBottom->setVisible(false);
		ui.scrollArea->setVisible(false);
		ui.teText->setVisible(true);
	}
	setMaximumSize(QSize(640, 410));

	QVBoxLayout* layout = (QVBoxLayout*)ui.widDoc->layout();
	m_view = new STWBView(true);
	layout->addWidget(m_view);
	m_view->installEventFilter(this);

	ui.pbNormal->setVisible(false);
	ui.pbMaximum->setVisible(true);
	ui.pbMaximum->setAttribute(Qt::WA_UnderMouse, false);

	m_docWindowPath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation)
		+ DATA_ROOT_PATH + TMP_PATH + QString::number(m_index);
	QDir* dir = new QDir;
	deleteDirectory(m_docWindowPath);
	dir->mkpath(m_docWindowPath);

	m_needUnzip = false;
	QStringList filePaths = lowerPath.split("/");
	QString fileName = filePaths.at(filePaths.size() - 1);
	QString downloadPath = m_docWindowPath + QString("/") + fileName;
	if (lowerPath.endsWith(".pptx") || lowerPath.endsWith(".ppt") || lowerPath.endsWith(".pptm")
		|| lowerPath.endsWith(".docx") || lowerPath.endsWith(".doc") || lowerPath.endsWith(".pdf"))
	{
		m_needUnzip = true;
		downloadPath = downloadPath + ".zip";
	}
	m_downloadPath = downloadPath;

	int x = 50 * (m_index + 1);
	int y = 35 * (m_index + 1);
	move(QPoint(x, y));
	m_normalRect = this->geometry();

	m_downloadClient = new STFileClient();
	connect(m_downloadClient, SIGNAL(onDownloadFinished()), this, SLOT(onDownloadFinished()));
	m_downloadClient->downloadFile(m_path, m_downloadPath);
}

STWBDocWindow::~STWBDocWindow()
{

}

void STWBDocWindow::onDownloadFinished()
{
	if (m_needUnzip)
	{
		unzip();
	}
	loadFile();
}

void STWBDocWindow::unzip()
{
	// unzip
	JlCompress::extractDir(m_downloadPath, m_docWindowPath);

	QFile file(m_downloadPath);
	if (file.exists())
	{
		file.remove();
	}
}

void STWBDocWindow::loadFile()
{
	QString lowerPath = m_path.toLower();
	QStringList filePaths = lowerPath.split("/");
	QString fileName = filePaths.at(filePaths.size() - 1);
	QString shortName = fileName.mid(0, fileName.lastIndexOf("."));
	m_shortName = shortName;
	QString extension = fileName;
	extension.replace(shortName, "");
	if (lowerPath.endsWith(".pptx") || lowerPath.endsWith(".ppt") || lowerPath.endsWith(".pptm")
		|| lowerPath.endsWith(".docx") || lowerPath.endsWith(".doc") || lowerPath.endsWith(".pdf"))
	{
		QDir *dir = new QDir(m_docWindowPath);
		QStringList filter;
		filter << "*.png";
		dir->setNameFilters(filter);
		QList<QFileInfo>* fileInfo = new QList<QFileInfo>(dir->entryInfoList(filter));
		m_totalPage = fileInfo->count();
		ui.widBottom->setVisible(true);
		ui.scrollArea->setVisible(true);
		ui.teText->setVisible(false);
		m_currentPage = 1;

		ui.lblPage->setText(QString("%1 / %2").arg(QString::number(m_currentPage), QString::number(m_totalPage)));
		QString firstPicName;
		if (m_totalPage == 1)
		{
			firstPicName = QString("%1.png").arg(m_shortName);
		}
		else
		{
			firstPicName = QString("%1-%2.png").arg(m_shortName, QString::number(m_currentPage - 1));
		}
		showBefittingPic(firstPicName);
	}
	else if (lowerPath.endsWith(".jpeg") || lowerPath.endsWith(".jpg") || lowerPath.endsWith(".png")
		|| lowerPath.endsWith(".bmp"))
	{
		ui.widBottom->setVisible(false);
		ui.scrollArea->setVisible(true);
		ui.teText->setVisible(false);
		showBefittingPic(fileName);
	}
	else if (lowerPath.endsWith(".txt"))
	{
		ui.widBottom->setVisible(false);
		ui.scrollArea->setVisible(false);
		ui.teText->setVisible(true);

		QString sourceTxtPath = m_docWindowPath + QString("/") + fileName;
		QFile file(sourceTxtPath);
		if (!file.open(QIODevice::ReadWrite))
			return;
		QTextStream out(&file);
		while (!file.atEnd())
		{
			ui.teText->setText(out.readAll());
		}
	}
}

void STWBDocWindow::showBefittingPic(QString picName)
{
	m_pngName = picName;
	QString sourcePicPath = m_docWindowPath + QString("/") + picName;
	QPixmap sourceImage = QPixmap(sourcePicPath);
	Qt::AspectRatioMode mode = Qt::KeepAspectRatio;
	ui.scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui.scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	if (sourceImage.height() > sourceImage.width())
	{
		mode = Qt::KeepAspectRatioByExpanding;
		ui.scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	}
	ui.scrollArea->verticalScrollBar()->setValue(0);

	QString tmpPicPath = m_docWindowPath + QString("/tmp_") + picName;
	QPixmap tmpImage = QPixmap(sourcePicPath).scaled(
		ui.scrollArea->size(),
		mode,
		Qt::SmoothTransformation);
	tmpImage.save(tmpPicPath);

	QSize size = tmpImage.size();
	//m_view->setFixedSize(size);
	//ui.widDoc->setFixedSize(size);
	ui.widShow->setFixedSize(size);
	QPixmap image = QPixmap(tmpPicPath);
	m_view->setBackground(image, size);
}

void STWBDocWindow::on_pbPre_clicked()
{
	if (m_currentPage <= 1)
	{
		return;
	}
	m_currentPage--;

	ui.lblPage->setText(QString("%1 / %2").arg(QString::number(m_currentPage), QString::number(m_totalPage)));
	QString pngName = QString("%1-%2.png").arg(m_shortName, QString::number(m_currentPage - 1));
	showBefittingPic(pngName);
}

void STWBDocWindow::on_pbNext_clicked()
{
	if (m_currentPage >= m_totalPage)
	{
		return;
	}
	m_currentPage++;

	ui.lblPage->setText(QString("%1 / %2").arg(QString::number(m_currentPage), QString::number(m_totalPage)));
	QString pngName = QString("%1-%2.png").arg(m_shortName, QString::number(m_currentPage - 1));
	showBefittingPic(pngName);
}

void STWBDocWindow::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_isPressed = true;
		m_startMovePos = event->globalPos();
	}
}

void STWBDocWindow::mouseMoveEvent(QMouseEvent* event)
{
	if (m_isPressed)
	{
		QPoint movePoint = event->globalPos() - m_startMovePos;
		QPoint pos = this->pos() + movePoint;
		m_startMovePos = event->globalPos();
		this->move(pos.x(), pos.y());
	}
}

void STWBDocWindow::mouseReleaseEvent(QMouseEvent* event)
{
	m_isPressed = false;
}

void STWBDocWindow::on_pbMaximum_clicked()
{
	ui.pbNormal->setVisible(true);
	ui.pbNormal->setAttribute(Qt::WA_UnderMouse, false);
	ui.pbMaximum->setVisible(false);

	setFixedSize(parentWidget()->size());
	move(QPoint(0, 0));
	m_isMaximum = true;

	QPixmap image = QPixmap(":/SoftTerminal/images/camera.png");
	m_view->setBackground(image, ui.scrollArea->size());

	if (ui.teText->isVisible())
	{
		QFont font;
		font.setFamily(QStringLiteral("微软雅黑"));
		font.setPointSize(18);
		ui.teText->setFont(font);
	}
	else
	{
		showBefittingPic(m_pngName);
	}
}

void STWBDocWindow::on_pbNormal_clicked()
{
	ui.pbNormal->setVisible(false);
	ui.pbMaximum->setVisible(true);
	ui.pbMaximum->setAttribute(Qt::WA_UnderMouse, false);

	setFixedSize(QSize(m_normalRect.width(), m_normalRect.height()));
	move(QPoint(m_normalRect.x(), m_normalRect.y()));
	m_isMaximum = false;

	if (ui.teText->isVisible())
	{
		QFont font;
		font.setFamily(QStringLiteral("微软雅黑"));
		font.setPointSize(12);
		ui.teText->setFont(font);
	}
	else
	{
		showBefittingPic(m_pngName);
	}
}

void STWBDocWindow::on_pbClose_clicked()
{
	Q_EMIT closeCloudFile(m_index);
	QString filePath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation)
		+ DATA_ROOT_PATH + TMP_PATH + QString::number(m_index);
	deleteDirectory(filePath);
	close();
}

bool STWBDocWindow::deleteDirectory(QString path)
{
	if (path.isEmpty())
	{
		return false;
	}

	QDir dir(path);
	if (!dir.exists())
	{
		return true;
	}

	dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
	QFileInfoList fileList = dir.entryInfoList();
	QFileInfo fileInfo;
	for (int i = 0; i != fileList.size(); i++)
	{
		fileInfo = fileList.at(i);
		if (fileInfo.isFile())
		{
			fileInfo.dir().remove(fileInfo.fileName());
		}
		else
		{
			deleteDirectory(fileInfo.absoluteFilePath());
		}
	}
	return dir.rmpath(dir.absolutePath());
}

void STWBDocWindow::setActionMode(int mode)
{
	m_view->clearSelection();
	m_view->setMode((ActionType)mode);
	if (mode == ActionType::AT_Select)
	{
		m_view->setDragMode(QGraphicsView::RubberBandDrag);
	}
	else
	{
		m_view->setDragMode(QGraphicsView::NoDrag);
	}
}

void STWBDocWindow::deleteAction()
{
	m_view->deleteSelectedItem();
}

void STWBDocWindow::setPenThickness(int thickness)
{
	m_view->setPenThickness(thickness);
}

void STWBDocWindow::setPenColor(QString color)
{
	m_view->setPenColor(color);
}

void STWBDocWindow::setTextSize(int size)
{
	m_view->setTextSize(size);
}

void STWBDocWindow::setTextColor(QString color)
{
	m_view->setTextColor(color);
}

bool STWBDocWindow::eventFilter(QObject *obj, QEvent *e)
{
	if (e->type() == QEvent::MouseButtonPress || e->type() == QEvent::FocusIn)
	{
		this->raise();
		m_view->setFocus();
	}
	else if (e->type() == QEvent::MouseButtonDblClick && ui.widTitle == obj)
	{
		if (ui.pbNormal->isVisible())
		{
			on_pbNormal_clicked();
		}
		else
		{
			on_pbMaximum_clicked();
		}
	}
	else if (e->type() == QEvent::KeyPress && ui.widBottom->isVisible())
	{
		QKeyEvent *event = static_cast<QKeyEvent*>(e);
		if (event->key() == Qt::Key_PageUp || event->key() == Qt::Key_Left)
		{
			on_pbPre_clicked();
		}
		else if (event->key() == Qt::Key_PageDown || event->key() == Qt::Key_Right)
		{
			on_pbNext_clicked();
		}
	}
	return false;
}