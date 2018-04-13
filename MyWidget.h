#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>

class MyWidget : public QWidget
{
	Q_OBJECT
private:
	QImage source_image;
	QImage image1;
	QImage image2;
	void updateImage();
public:
	explicit MyWidget(QWidget *parent = nullptr);

	void loadImage(const QString &path);
signals:

public slots:

	// QWidget interface
protected:
	void paintEvent(QPaintEvent *event);

	// QWidget interface
protected:
	void resizeEvent(QResizeEvent *event);
};

#endif // MYWIDGET_H
