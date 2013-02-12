#ifndef MENU_H
#define MENU_H

#include <QString>
#include <QDateTime>
#include "ui_ut.h"

class Ut : public QDialog
{
	Q_OBJECT
	public:
		Ut(QWidget *parent = 0);
	private:
		Ui::ut ui;
		QPalette red, white;
		QString filename;

		QStringList obss;
		int base;
		double longitude;
		int timezone;
		bool read_cat();

		QString zero(int);
		QString next_day(int, int, int);
		double hours(int, double, double);
		QString h2hhmmss(double);
		double JulianDate(QDateTime);
		double ST2UT(QDateTime);
		QString conv(QString);
	
		class Dstime {
		public:	
			QDateTime begin;
			QDateTime end;
			int tz, stz;
	
			Dstime(QDateTime b, QDateTime e, int t, int st)
			{
				begin = b; end = e; tz = t; stz = st;
			}
			int isInto(QDateTime d )
			{
				if ((d > begin) && (d < end))	return stz;
				else 				return tz;
			}
		};
		QList <Dstime> dst;

		int dsavingtime(QDateTime);

		bool read_dst();
		bool read_cfg(int&);
		void write_cfg();
	public slots:
		void read_write();
		void newfile();
		void inpfilename();
		void newcat();
		void observatory(int);
		void processing();
};
#endif
