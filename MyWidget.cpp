#include "MyWidget.h"
#include <QPainter>
#include <QTime>
#include "antialias.h"

MyWidget::MyWidget(QWidget *parent)
	: QWidget(parent)
{

}

void MyWidget::updateImage()
{
	if (source_image.isNull()) {
		int w = width() / 2;
		int h = height();
		if (image1.width() != w || image1.height() != h) {
			if (w > 0 && h > 0) {
				image1 = QImage(w, h, QImage::Format_RGB888);
				image1.fill(Qt::black);
				QPainter pr(&image1);
				pr.setPen(Qt::white);
				pr.setBrush(Qt::white);
				pr.drawEllipse(5, 5, w - 10, h - 10);
			}
		}
	} else {
		image1 = source_image.convertToFormat(QImage::Format_RGB888);
		image2 = QImage();
	}
}

void MyWidget::loadImage(QString const &path)
{
	source_image = QImage(path);
	image2 = QImage();
	updateImage();
}

void MyWidget::paintEvent(QPaintEvent *event)
{
	updateImage();
	if (image2.isNull()) {
		image2 = image1;
		QTime t;
		t.start();
		image::antialias(&image2);
		int e = t.elapsed();
		static_cast<QWidget *>(parent())->setWindowTitle(QString::number(e));
	}

	QPainter pr(this);
	int w = width();
	int h = height();
	pr.fillRect(0, 0, w, h, Qt::black);
	pr.setClipRect(0, 0, w / 2, h);
	pr.drawImage((w / 2 - image1.width()) / 2, (h - image1.height()) / 2, image1);
	pr.translate(w - w / 2, 0);
	pr.setClipRect(0, 0, w / 2, h);
	pr.drawImage((w / 2 - image1.width()) / 2, (h - image1.height()) / 2, image2);
}

void MyWidget::resizeEvent(QResizeEvent *event)
{
	image2 = QImage();
}
