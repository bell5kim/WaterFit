#include <qmainwindow.h>
#include <qwt_plot.h>

#include <QStatusBar>
#include <QPoint>

#define MAXDATA 5000

// class Q3PointArray;
class QwtPlotCurve;
class QwtPlotMarker;

class MyPlot2D: public QwtPlot {
    Q_OBJECT

public:
    explicit MyPlot2D( QWidget *parent = 0 );
    // MyPlot2D(QWidget *parent = 0, const char *name = 0);

    char fname[256];
    // void showToolBar(QMainWindow *mWin);
    // void readDataFile(char *fname);
    void err_Exit(char *location, char *reason, int exit_code);
    void usrDataFile(char *fname);
    // QString readSettingString(QString keyWord);

    double z[MAXDATA];    // z position
    double dc[MAXDATA];	  // calculated dair
    double dm[MAXDATA];	  // measured dair
    double dd[MAXDATA];	  // (dc-dm)/dm*100
    int    nData;         // number of data points

    QString ddFile;

    void enableLegend(bool value);
 	 
private slots:
	 	     
    // void toggleCurve(long);
	 
private:
    // QStatusBar *statusBar;
    // void showInfo(QString text = QString::null);
	 
    // long crv[100];
    // long mrk[100];
    QwtPlotCurve  *(crv[100]);
    QwtPlotMarker *mrk;
};
