#include "qimage2matdialog.h"
#include "ui_qimage2matdialog.h"

QImage2MatDialog::QImage2MatDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QImage2MatDialog)
{
    ui->setupUi(this);
    _imageFilePath = "C:\\";

    ui->lineEdit_imagePath->setText(_imageFilePath);
}

QImage2MatDialog::~QImage2MatDialog()
{
    delete ui;
}


void QImage2MatDialog::on_pushButton_clicked()
{
    _imageFilePath = QFileDialog::getOpenFileName(
                this,
                tr("Load Image File"),
                "C:\\",
                tr("JPEG(*.jpg);;BMP(*.bmp);;PNG(*.png);;All File(*.*)"));

    ui->lineEdit_imagePath->setText(_imageFilePath);

	if ( !isPicture(_imageFilePath) )
	{
		QMessageBox::information(
			NULL,
			"Error",
			"Load Image Failed!",
			QMessageBox::Yes | QMessageBox::No,
			QMessageBox::Yes);

		return;
	}

	QImage qImage;
	qImage.load(_imageFilePath);

	qimageShow(qImage, ui->graphicsView_qimg);

	mat2qImageShow(_imageFilePath);

	qImage2MatShow(qImage);

	qImage2MatPtrShow(qImage);
}

void QImage2MatDialog::qimageShow(QImage& qImage, QGraphicsView*& graphView)
{
	int imageWidth  = qImage.width();
	int imageHeight = qImage.height();
	int viewWidth   = ui->graphicsView_qimg->width();
	int viewHight   = ui->graphicsView_qimg->height();

	QPixmap pixMap;
	pixMap = pixMap.fromImage(qImage.scaled(viewWidth, 
		viewHight, 
		Qt::KeepAspectRatioByExpanding));

#if 0
	qDebug() << viewWidth << " " << viewHight;
#endif

	QGraphicsScene *graphicsViewScene = new QGraphicsScene();
	graphicsViewScene->setSceneRect(0, 0, viewWidth, viewHight);
	graphicsViewScene->clear();
	graphicsViewScene->addPixmap(pixMap);

	graphView->setScene(graphicsViewScene);
	graphView->show();
}

void QImage2MatDialog::mat2qImageShow(QString& imagePath)
{
	cv::Mat mat = cv::imread(_imageFilePath.toStdString(), IMREAD_UNCHANGED);

	qimageShow(mat2QImage(mat), ui->graphicsView_mat);
}

void QImage2MatDialog::qImage2MatShow(QImage& qImage)
{
	int imageWidth  = qImage.width();
	int imageHeight = qImage.height();

	cv::Mat qImg2Mat = qImage2Mat(qImage);

#if 1
	if ( qImg2Mat.data )
	{
		cv::imshow("QImage to Mat 1", qImg2Mat);
//		cv::waitKey(0);
//		cv::imwrite("E:\\lena.png", qImg2Mat);
	}
#endif
}

void QImage2MatDialog::qImage2MatPtrShow(QImage& qImage)
{
	int imageWidth  = qImage.width();
	int imageHeight = qImage.height();

	cv::Mat qImg2Mat = qImage2MatPtr(qImage);

#if 1
	if ( qImg2Mat.data )
	{
		cv::imshow("QImage to Mat 2", qImg2Mat);
//		cv::waitKey(0);
//		cv::imwrite("E:\\lena2.png", qImg2Mat);
	}
#endif
}

QImage QImage2MatDialog::mat2QImage(cv::Mat& mat)
{
	// 8-bits unsigned, NO. OF CHANNELS = 1  
	if(mat.type() == CV_8UC1)  
	{  
#if 0
		qDebug() << "CV_8UC1";
#endif  
		QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);  
		// Set the color table (used to translate colour indexes to qRgb values)  
		image.setColorCount(256);  
		for(int i = 0; i < 256; i++)  
		{  
			image.setColor(i, qRgb(i, i, i));  
		}  
		// Copy input Mat  
		uchar *pSrc = mat.data;  
		for(int row = 0; row < mat.rows; row ++)  
		{  
			uchar *pDest = image.scanLine(row);  
			memcpy(pDest, pSrc, mat.cols);  
			pSrc += mat.step;  
		}  
		return image;  
	}  
	// 8-bits unsigned, NO. OF CHANNELS = 3  
	else if(mat.type() == CV_8UC3)  
	{  
		// Copy input Mat  
		const uchar *pSrc = (const uchar*)mat.data;  
		// Create QImage with same dimensions as input Mat  
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);  
		return image.rgbSwapped();  
	}  
	else if(mat.type() == CV_8UC4)  
	{  
#if 0
		qDebug() << "CV_8UC4";
#endif  
		// Copy input Mat  
		const uchar *pSrc = (const uchar*)mat.data;  
		// Create QImage with same dimensions as input Mat  
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);  
		return image.copy();  
	}  
	else  
	{  
#if 0
		qDebug() << "ERROR: Mat could not be converted to QImage.";
#endif
		QMessageBox::information(
			NULL,
			"Error",
			"Image Format Unknown!",
			QMessageBox::Yes | QMessageBox::No,
			QMessageBox::Yes);
		return QImage();  
	}  
}

cv::Mat QImage2MatDialog::qImage2Mat(QImage& qImg)
{
	cv::Mat mat;
	switch(qImg.format())  
	{  
	case QImage::Format_ARGB32:  
	case QImage::Format_RGB32:  
	case QImage::Format_ARGB32_Premultiplied:  
		cv::Mat(qImg.height(), qImg.width(), CV_8UC4, (void*)qImg.constBits(), qImg.bytesPerLine()).copyTo(mat);  
		break;  
	case QImage::Format_RGB888:  
		cv::Mat(qImg.height(), qImg.width(), CV_8UC3, (void*)qImg.constBits(), qImg.bytesPerLine()).copyTo(mat);  
		cv::cvtColor(mat, mat, CV_BGR2RGB);  
		break;  
	case QImage::Format_Indexed8:  
		cv::Mat(qImg.height(), qImg.width(), CV_8UC1, (void*)qImg.constBits(), qImg.bytesPerLine()).copyTo(mat);  
		break;
	default:
		qDebug() << "Image Format Unknown!\n";
	}  

	return mat;
}

cv::Mat QImage2MatDialog::qImage2MatPtr(QImage& qImg)
{
	int qImgWidth  = qImg.width();
	int qImgHeight = qImg.height();
	int qImgDepth = qImg.depth();
	int imgChannels = qImgDepth/8;
	int byterPerLine = qImg.bytesPerLine();

#if 0
	qDebug() << qImg.format() << " " << qImg.depth();
#endif

	cv::Mat mat;

	if ( qImg.format() == QImage::Format_ARGB32 || qImg.format() == QImage::Format_RGB888 || 
		qImg.format() == QImage::Format_Indexed8 || qImg.format() == QImage::Format_ARGB32_Premultiplied )
	{
		cv::Mat(qImg.height(), qImg.width(), CV_MAKETYPE(CV_8U, qImgDepth/8), cv::Scalar::all(0)).copyTo(mat);
		uchar* data_qimg = qImg.bits();
		uchar* data_mat  = (uchar*)mat.data;

		for ( int i=0; i<qImgHeight; i++ )
			for ( int j=0; j<qImgWidth; j++ )
				for ( int k=0; k<imgChannels; k++ )
					data_mat[(i*qImgWidth+j)*imgChannels + k] = data_qimg[i*byterPerLine+j*imgChannels + k];

		data_mat  = NULL;
		data_qimg = NULL;
	}
	else if ( qImg.format() == QImage::Format_RGB32 )
	{
		cv::Mat(qImg.height(), qImg.width(), CV_MAKETYPE(CV_8U, 3), cv::Scalar::all(0)).copyTo(mat);
		uchar* data_qimg = qImg.bits();
		uchar* data_mat  = (uchar*)mat.data;

		for ( int i=0; i<qImgHeight; i++ )
		{
			for ( int j=0; j<qImgWidth; j++ )
			{
				data_mat[(i*qImgWidth+j)*3]   = data_qimg[i*byterPerLine+j*4];   // B
				data_mat[(i*qImgWidth+j)*3+1] = data_qimg[i*byterPerLine+j*4+1]; // G
				data_mat[(i*qImgWidth+j)*3+2] = data_qimg[i*byterPerLine+j*4+2]; // R
			}
		}
	}
	else
	{
		qDebug() << "Image Format Unknown!\n";
	}

	return mat;
}


bool QImage2MatDialog::isPicture(QString& imagePath)
{
	if (_imageFilePath.contains(".jpg",  Qt::CaseInsensitive) || _imageFilePath.contains(".bmp",  Qt::CaseInsensitive) ||
		_imageFilePath.contains(".png",  Qt::CaseInsensitive) || _imageFilePath.contains(".gif",  Qt::CaseInsensitive) ||
		_imageFilePath.contains(".jpeg", Qt::CaseInsensitive) || _imageFilePath.contains(".pgm",  Qt::CaseInsensitive) ||
		_imageFilePath.contains(".ppm",  Qt::CaseInsensitive) || _imageFilePath.contains(".tif",  Qt::CaseInsensitive) ||
		_imageFilePath.contains(".tiff", Qt::CaseInsensitive) || _imageFilePath.contains(".xpm",  Qt::CaseInsensitive) || 
		_imageFilePath.contains(".pbm",  Qt::CaseInsensitive) || _imageFilePath.contains(".xbm",  Qt::CaseInsensitive) )
		return true;
	else 
		return false;	
}


