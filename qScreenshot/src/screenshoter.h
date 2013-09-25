#ifndef SCREENSHOTER_H
#define SCREENSHOTER_H

#include <QObject>
#include <QPixmap>

class GrabAreaWidget;

class Screenshoter : public QObject
{
	Q_OBJECT
public:
	explicit Screenshoter(QObject *parent = 0);
	~Screenshoter();
	
signals:
	void refreshWindow();
	void pixmapReady(const QPixmap&);
	
public slots:
	void captureDesktop(int delay = 0);
	void captureWindow(int delay = 0);
	void captureArea(int delay = 0);

private slots:
	void shootDesktop();
	void shootWindow();
	void shootArea();

private:
	void shoot(WId id);

private:
	GrabAreaWidget* grabAreaWidget_;
};

#endif // SCREENSHOTER_H
