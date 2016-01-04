#include <QDebug>
#include <QFileInfo>

#include "ut.h"
 
Ut::Ut(QWidget *parent) : QDialog(parent)
{
qDebug() << "start Ut";
	ui.setupUi(this);
	connect(ui.convert, SIGNAL(clicked()), this, SLOT(read_write()));
	connect(ui.file, SIGNAL(textChanged(QString)), this, SLOT(newfile()));
	connect(ui.cat, SIGNAL(textChanged(QString)), this, SLOT(newcat()));
	connect(ui.observatory, SIGNAL(currentIndexChanged(int)), this, SLOT(observatory(int)));
	connect(ui.dir, SIGNAL(textChanged(QString)), this, SLOT(newdir()));
	connect(ui.convert, SIGNAL(pressed()), this, SLOT(processing()));

	connect(ui.LT2UT, SIGNAL(clicked()), this, SLOT(inpfilename()));	
	connect(ui.ST2UT, SIGNAL(clicked()), this, SLOT(inpfilename()));
    connect(ui.JD2UT, SIGNAL(clicked()), this, SLOT(inpfilename()));

	red.setColor(QPalette::Base,QColor(Qt::red));
	white.setColor(QPalette::Base,QColor(Qt::white));

//	filename = "..\\data\\" + ui.file->text() + "maindata_lt.txt";

	int index = 0;
	read_cfg(index);
	newfile();

	read_cat();
	observatory(index);

	read_dst();
	inpfilename();
qDebug() << "stop Ut";
}

//POT015			Potsdam	Germany	AIP Telegrafenberg	Potsdam	Germany	042	1	13 04.0	52 22.9	107

bool Ut::read_cat()
{
qDebug() << "begin read_cat";
	ui.observatory->clear();
	QString catname(ui.dir->text() + ui.cat->text() + ".txt");
qDebug() << catname;
	QFile inp(catname);
	if (!inp.open(QFile::ReadOnly))
	{
qDebug() << "ERROR!";
		ui.cat->setPalette(red);
		return false;		
	}
	ui.cat->setPalette(white);
	QTextStream text(&inp);

	QStringList row;
	bool found = false;
	int i = 0;
	while (!text.atEnd())
	{
		QString line(text.readLine());
		obss << line;
		row = line.split('\t');
// qDebug() << row[0] << row[9] << row[10] << row[11];
		if (row[0] == ui.file->text()) 
		{
			if (!found)
			{
				base = i;
				found = true;
			}
			ui.observatory->addItem(row[0] + "," + 
				row[6]); //+ ", " + row[12] + ", " + row[13]);
		}
		i++;
	}
	inp.close();
	if(!found)	
	{
qDebug() << "not found";
	 	return false;		
	}

qDebug() << "end read_cat";
	return true;
}

bool Ut::read_dst()
{
	QString name(ui.file->text() + ".dst");
qDebug() << "begin read_dst" << name;
	QFile inp(name);
	if (!inp.open(QFile::ReadOnly))
	{
qDebug() << "no " << name << " file!";
		ui.dst->setText("   ");
		return false;
	}
	QTextStream text(&inp);

	QStringList row;
	while (!text.atEnd())
	{
		QString line(text.readLine());
		row = line.split(' ');
qDebug() << row;
		Dstime d(QDateTime(QDate::fromString(row[0], "yyyy-MM-dd"), 
				QTime::fromString(row[1], "hh:mm:ss")), 
			QDateTime(QDate::fromString(row[2], "yyyy-MM-dd"), 
				QTime::fromString(row[3], "hh:mm:ss")),
			row[4].toInt(), row[5].toInt());
		dst.append(d);
qDebug() << d.begin << d.end;
	}
	ui.dst->setText("DST");;
	inp.close();
qDebug() << "end read_dst";
	return true;
}

bool Ut::read_cfg(int &index)
{
qDebug() << "begin read_cfg";
	index = 0;
	QFile inp("timetool.cfg");
	if (!inp.open(QFile::ReadOnly))
	{
qDebug() << "no timetool.cfg file!";
		return false;
	}
	QTextStream text(&inp);
	int num;
	text >> num;
	QString fname;
	text >> fname;
	QString catalog;
	text >> catalog;
	text >> index;
	QString dir;
	text >> dir;

	ui.cat->setText(catalog);

	ui.file->setText(fname);

	ui.dir->setText(dir);

	if (num == 1) 
	{
		ui.LT2UT->setChecked(true);
		ui.ST2UT->setChecked(false);
        ui.JD2UT->setChecked(false);

	}
	else if (num == 2)
	{
		ui.LT2UT->setChecked(false);
		ui.ST2UT->setChecked(true);
        ui.JD2UT->setChecked(false);
	}
    else // if (num == 3)
    {
		ui.LT2UT->setChecked(false);
		ui.ST2UT->setChecked(false);
        ui.JD2UT->setChecked(true);
	}

	ui.observatory->setCurrentIndex(index);

	inp.close();
qDebug() << "end read_cfg";	
	return true;
}

void Ut::write_cfg()
{
	QFile out("timetool.cfg");
	if (!out.open(QFile::WriteOnly))
	{
qDebug() << "ERROR222";
		return;
	}
	QTextStream text(&out);
//	text << ((ui.LT2UT->isChecked()) ? 1  : 2) << endl;
    if (ui.LT2UT->isChecked()) text << 1 << endl;
    else if (ui.ST2UT->isChecked()) text << 2 << endl;
    else if (ui.JD2UT->isChecked()) text << 3 << endl;
	text << ui.file->text() << endl;
	text << ui.cat->text() << endl;
	text << ui.observatory->currentIndex() << endl;
	text << ui.dir->text();
	out.close();
}	
/************************************************************/
int Ut::dsavingtime(QDateTime qdt)
{
qDebug() << "begin dsavingtime" << qdt;
	int out = 0;
	foreach(Dstime dt, dst)
	{
		if (dt.begin.date().year() == qdt.date().year())
						out = dt.isInto(qdt);
// qDebug() << dt.begin << qdt << dt.end;		
	}
qDebug() << "end dsavingtime" << out;
	return out;
}

QString Ut::zero(int k)
{
	QString s = QString::number(k);	
	if (k < 10) return "0" + s;
	else return s;   
}  

QString Ut::next_day(int y, int m, int d)
{
     const int M[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
     if (d < M[m]) d++;
     else if (m != 2 && !(m == 12 && d == 31)) { m++; d = 1; } 
     else if (m == 2 && !(y%4 == 0)) { m = 3; d = 1; }
     else if (m == 2 && y%4 == 0)  d = 29;
     else if (m == 12 && d == 31) { y++; m = 1; d = 1; } 
     else 
qDebug() << "next_day: ERROR";
     QString nd(QString::number(y) + zero(m) + zero(d));
qDebug() << "next_day:" << y << m << d << "->" << nd;
     return nd;
}

double Ut::hours(int hh, double mm, double ss)
{
       return 1.0*hh + mm/60.0 + ss/3600.0;      
} 

QString Ut::h2hhmmss(double h)
{
	int hh = int(h);
	int mm = int((h - hh)*60);
//	int ss = int((h - hh - mm/60.0)*3600);
	int ss = int(((h - hh)*60 - mm)*60);
	QString s(zero(hh) + zero(mm) + zero(ss));
qDebug() << "h2hhmmss" << s;
	return s;  
}    

// OK
double Ut::ST2UT(QDateTime q)
{
     double du  = q.date().toJulianDay() - 2451545;
qDebug() << "JD= " << QString::number(du + 2451545, 'g', 16);
     double T = du/36525;
     double GMST1 = 24110.54841 + T*(8640184.812866 + 0.093104*T - 6.2e-6*T*T);
     double LST = hours(q.time().hour(), q.time().minute(), q.time().second());
     double UT = LST - GMST1/3600.0 - longitude/15;
qDebug() << "UT=" << UT << longitude;
     UT = UT/24;
qDebug() << "UT/24" << UT;
     UT= 24*(UT - int(UT));
qDebug() << "UT=" << UT;
     return UT;           
}

QString Ut::conv(QString s)
{
	const int TIME = 28;  

    int year = s.mid(TIME+0,4).toInt();
	int month = s.mid(TIME+4, 2).toInt();
	int day = s.mid(TIME+6, 2).toInt();
	QDate qdate(year, month, day);

	int hour = s.mid(TIME+8, 2).toInt();
	int minute = s.mid(TIME+10, 2).toInt(); 
	int second = s.mid(TIME+12, 2).toInt(); 
	QTime qtime(hour, minute, second);

	QDateTime qdt(qdate, qtime);

qDebug() << "LST " << year << zero(month) << zero(day) 
          << zero(hour) << zero(minute) << zero(second) << endl; 
qDebug() << qdate << qtime << qdt;

//      double Longitude = hours(13, 3, 52.65); // Zeiss-Triplet

	QString hhmmss;
	if (ui.ST2UT->isChecked())
	{
		double UT = ST2UT(qdt);
		hhmmss = h2hhmmss(UT);
		qtime = QTime::fromString(hhmmss, "hhmmss");
		qdt = QDateTime(qdate, qtime);
		if (qtime.hour() < 12) qdt = qdt.addDays(1); 
/* предполагаме, че датата е началото на нощта и не е сменяна през цялата нощ */
/* предполагаме, че датата в началото на нощта е същата, както в часова зона 0 */	
	}
	else 
	{
		qdt = qdt.addSecs(-dsavingtime(qdt)*60*60); // LT -> UT
//		UT = hours(qdt.time().hour(), qdt.time().minute(), qdt.time().second());
	}
	QString sout = s.left(TIME) + qdt.toString("yyyyMMddhhmmss") + s.mid(TIME+14);            
qDebug() << s;
qDebug() << sout;
	return sout;
}

QString Ut::convJD(QString s)
{
    const int TIME = 28;
    QString ss = s.mid(TIME, 12);
    QStringList qss = ss.split('.');
    qss[1] = "0." + qss[1];
qDebug() << qss[0] << qss[1];
    qlonglong jd = qss[0].toLongLong();
    double part = qss[1].toDouble();
    if (part >= 0.5) jd++;
    QDate date = QDate::fromJulianDay(jd);
qDebug() << date << jd;
    QTime time(12,0);
    time = time.addSecs(int(part*60*60*24));
qDebug() << time;
    QString sout = s.left(TIME) + date.toString("yyyyMMdd")
        + time.toString("hhmmss") + s.mid(TIME+14);
qDebug() << sout << "convJD"; 
    return sout;
}

void Ut::read_write()
{
qDebug() << "begin read_write";
	QFile inp(filename);
	if (!inp.open(QFile::ReadOnly))
	{
qDebug() << "ERROR1";
		return;
	}
	QTextStream text1(&inp);
	
	QString filename2 = filename.left(filename.size() - 6) + "ut.txt";
qDebug() << filename << "-->" << filename2;
	QFile out(filename2);
	if (!out.open(QFile::WriteOnly))
	{
qDebug() << "ERROR2";
		return;
	}
	QTextStream text2(&out);

	while (!text1.atEnd())
	{
		QString line(text1.readLine());
		if (line.mid(42,1) != " ") text2 << line << endl;
		else if (ui.JD2UT->isChecked()) text2 << convJD(line) << endl;
        else text2 << conv(line) << endl;
	}
	inp.close();
	out.close();

	ui.convert->setText("DONE!");
	ui.convert->setDisabled(true);
	write_cfg();
qDebug() << "end read_write";	
}
/********************** slots *****************************************/
void Ut::newfile()
{
	QString s = ui.dir->text() + ui.file->text() + ui.label4->text();
qDebug() << "newfile:" << s;
	QFileInfo f(s);
	if (f.isFile()) 
	{
		ui.file->setPalette(white);
		if (read_cat())
		{
			ui.convert->setDisabled(false);
			filename = s;
			QString ss;
			if (ui.LT2UT->isChecked()) ss = "_lt";
			else ss = "_st";
			s = ui.file->text() + ui.label4->text();
			ui.outfile->setText(s.replace(ss, "_ut"));
			ui.outfile->update();
			
			read_dst();
		}
		else ui.convert->setDisabled(true);
	}
	else 
	{
		ui.file->setPalette(red);
		ui.convert->setDisabled(true);
	}
}

void Ut::inpfilename()
{
qDebug() << "inpfilename";
	if (ui.LT2UT->isChecked()) 
	{	
		ui.label4->setText("maindata_lt.txt");
		read_dst();
	}
	else if (ui.ST2UT->isChecked()) ui.label4->setText("maindata_st.txt");
    else ui.label4->setText("maindata_jd.txt");

	ui.label4->update();
	newfile();	
}

void Ut::newcat()
{
qDebug() << "newcat";
	if(read_cat())
	{
		newfile();
	}
	else
	{
		ui.convert->setDisabled(true);
		ui.convert->update();
	}
}

void Ut::newdir()
{
qDebug() << "newdir";
	if(read_cat())
	{
		newfile();
	}
	else
	{
		ui.convert->setDisabled(true);
		ui.convert->update();
	}
}
	
void Ut::observatory(int index)
{
qDebug() << "observatory" << index;
	QString line(obss[base + index]);
	QStringList row = line.split('\t');
	timezone = row[13].toInt();
	longitude = hours(row[13].left(row[13].indexOf(' ')).toInt(), 
			row[13].mid(row[13].indexOf(' ')).toDouble(), 0);
	ui.lon->setText(row[12] + "; " + row[13]);
}

void Ut::processing()
{
	ui.convert->setText("Processing...");
	ui.convert->update();
}
