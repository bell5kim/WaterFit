#include <qwt_math.h>
#include <qwt_scale_engine.h>
#include <qwt_symbol.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <qwt_text.h>
#include <qwt_plot_canvas.h>
#include <qmath.h>
#include "myplot2d.h"

#include <QTextStream>
#include <QSettings>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

// MyPlot2D::MyPlot2D(QWidget *parent, const char *name)
MyPlot2D::MyPlot2D( QWidget *parent ):
        QwtPlot(parent) {
    setAutoReplot( false );

    setTitle( "Comparison of WATER FIT against Measurements" );

    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    canvas->setBorderRadius( 10 );

    setCanvas( canvas );
    setCanvasBackground( QColor( "LightGray" ) );

    // legend
    // QwtLegend *legend = new QwtLegend;
    // insertLegend( legend, QwtPlot::BottomLegend );

    // grid
    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->enableXMin( true );
    grid->setMajorPen( Qt::white, 0, Qt::DotLine );
    grid->setMinorPen( Qt::gray, 0 , Qt::DotLine );
    grid->attach( this );

    // axes
    enableAxis( QwtPlot::yRight );
    setAxisTitle( QwtPlot::xBottom, "Distance from CAX (cm)" );
    setAxisTitle( QwtPlot::yLeft, "Relative Output Factor" );
    // setAxisTitle( QwtPlot::yRight, "Phase [deg]" );

    // setAxisMaxMajor( QwtPlot::xBottom, 6 );
    // setAxisMaxMinor( QwtPlot::xBottom, 9 );
    // setAxisScaleEngine( QwtPlot::xBottom, new QwtLogScaleEngine );

    setAutoReplot( true );
}
/*
void MyPlot2D::showToolBar(QMainWindow *mWin) {

    QToolBar    *toolBar = new QToolBar(mWin);
    
    QToolButton *btnZoom = new QToolButton(toolBar);
    btnZoom->setTextLabel("Zoom");
    btnZoom->setPixmap(zoom_xpm);
    btnZoom->setToggleButton(true);
    btnZoom->setUsesTextLabel(true);

#ifndef QT_NO_PRINTER
    QToolButton *btnPrint = new QToolButton(toolBar);
    btnPrint->setTextLabel("Print");
    btnPrint->setPixmap(print_xpm);
    btnPrint->setUsesTextLabel(true);
#endif

  QToolButton *btnTable = new QToolButton(toolBar);
    btnTable->setTextLabel("Table");
    btnTable->setPixmap(table_xpm);
    btnTable->setUsesTextLabel(true);

  QToolButton *btnClose = new QToolButton(toolBar);
    btnClose->setTextLabel("Close");
    btnClose->setPixmap(close_xpm);
    btnClose->setUsesTextLabel(true);

#ifndef QT_NO_PRINTER
    connect(btnPrint, SIGNAL(clicked()), SLOT(printIt()));
#endif
    connect(btnZoom, SIGNAL(toggled(bool)), SLOT(zoom(bool)));

    // connect(btnClose, SIGNAL(clicked()), SLOT(close()));
   
    connect(qwtPicker, SIGNAL(moved(const QPoint &)),
            SLOT(moved(const QPoint &)));
  
    connect(qwtPicker, SIGNAL(selected(const Q3PointArray &)),
            SLOT(selected(const Q3PointArray &)));

    connect(btnTable, SIGNAL(clicked()), this,
      SLOT( runGammaTable() ) );   
}

void MyPlot2D::zoom(bool on){  
    qwtZoomer[0]->setEnabled(on);
    qwtZoomer[0]->zoom(0);

    qwtZoomer[1]->setEnabled(on);
    qwtZoomer[1]->zoom(0);

    qwtPicker->setRubberBand(
        on ? QwtPicker::NoRubberBand: QwtPicker::CrossRubberBand);
   
    showInfo();
}


void MyPlot2D::showInfo(QString text){
    if ( text == QString::null )
    {
        if ( qwtPicker->rubberBand() )
            text = "Cursor Pos: Press left mouse button in plot region";
        else
            text = "Zoom: Press mouse button and drag";
    }

    statusBar->message(text);
}
*/

// error handler
void MyPlot2D::err_Exit(char *location, char *reason, int exit_code)
{
   cerr << endl;
   cerr << "RUN-TIME ERROR in " << location << endl;
   cerr << "Reason: " << reason << "!" << endl;
   cerr << "Exiting to system..." << endl;
   cerr << endl;
   exit(exit_code);
}
/*
QString MyPlot2D::readSettingString(QString keyWord) {
	
    QString HOSTNAME = getenv("HOSTNAME");
    QString gName = HOSTNAME.section('.',0,0);
	
    QString Entry = "/"+gName+"/" + keyWord;
    QSettings settings;
    // settings.setPath("localhost", gName);
    settings.insertSearchPath( QSettings::Unix, HOST_NAME);
    settings.beginGroup("/"+gName);
    QString keyValue = settings.readEntry(Entry, "");
    settings.endGroup();
    return (keyValue);
}
*/
void MyPlot2D::usrDataFile (char *fname) {
    ddFile = fname;
    ifstream inp_file;
    inp_file.open(fname,ios::in);
    if (inp_file.bad()) err_Exit("MyPlot2D::usrDataFile",
                              "cannot open input file",8);
	
	int i = 0;
	float zMax = 0.0;
    while (!inp_file.eof())
    {
        char line[256] = "";  // lines to read from file
        inp_file.getline(line,sizeof(line));
        istringstream line_stream(line);
		line_stream >> z[i] >> dc[i] >> dm[i] >> dd[i];
		if (z[i] > zMax) zMax = z[i];
		i++;
	}
	
	nData = i-1;
	// close file
    inp_file.close();

    QString titleText;
    setTitle("Comparison of In-water Fitting with Measurement");

//  setCanvasBackground(lightGray);
/*
    // legend
    setAutoLegend(true);
    enableLegend(true);
    setLegendPosition(QwtPlot::Bottom);
    // setLegendFrameStyle(QFrame::NoFrame|QFrame::Plain);

    // grid
    grid->enableGridXMin();
    grid->setGridMajPen(QPen(darkGray, 0, DotLine));
    grid->setGridMinPen(QPen(gray, 0, DotLine));

    QFont Helvetica9("Helvetica", 9, QFont::Normal);
*/
	QString PWD = getenv("PWD");
	
	QString FILENAME = fname;
	if (FILENAME.contains("spectrum.dat")) {
		if (FILENAME.contains("reference")) {
            QTextStream(&titleText) << "Spectrum: "
                                    << PWD.section('/',-2,-2);

            //setTitleFont(Helvetica9);
            // setAxisFont(QwtPlot::xBottom,Helvetica9);
            setAxisTitle(QwtPlot::xBottom, "E (MeV)");
            //setAxisFont(QwtPlot::yLeft,Helvetica9);
            setAxisTitle(QwtPlot::yLeft, "Rel. Prob.");
            setAxisScale(QwtPlot::yLeft, 0, 1.1, 0);
            // setAxisTitleFont(QwtPlot::xBottom,Helvetica9);
            // setAxisTitleFont(QwtPlot::yLeft,Helvetica9);

            // Curves
            crv[2] = new QwtPlotCurve("Reference");
            crv[2]->setRenderHint(QwtPlotItem::RenderAntialiased);
            crv[2]->setPen(QPen(Qt::darkBlue));
            crv[2]->setYAxis(QwtPlot::yLeft);
            crv[2]->attach(this);
            crv[2]->setSamples(z, dc, nData);

            // curves
 //           crv[2] = insertCurve("Reference");
 //           setCurvePen(crv[2], QPen(blue));
 //           setCurveYAxis(crv[2], QwtPlot::yLeft);
 //           setCurveData(crv[2], z, dc, nData);

//			mrk[2] = insertMarker();
//            setMarkerPos(mrk[2], zMax*1.5/2,1.1-1.1*0.2*1/10);
//			setMarkerLabelAlign(mrk[2], AlignRight|AlignBottom);
//			setMarkerPen(mrk[2], QPen(blue, 0, DashDotLine));
//			setMarkerFont(mrk[2], Helvetica9);
//			setMarkerLabelText(mrk[2],"Ref");

//			mrk[1] = insertMarker();
//            setMarkerPos(mrk[1], zMax*1.5/2,1.1-1.1*0.2*5/10);
//			setMarkerLabelAlign(mrk[1], AlignRight|AlignBottom);
//			setMarkerPen(mrk[1], QPen(red, 0, DashDotLine));
//			setMarkerFont(mrk[1], Helvetica9);
//			setMarkerLabelText(mrk[1],"Fitted");

            enableLegend(false);
       }
       else {
             //removeCurves();
             //removeMarkers();
             QwtPlotDict::detachItems(QwtPlotItem::Rtti_PlotCurve);
             QwtPlotDict::detachItems(QwtPlotItem::Rtti_PlotMarker);

             QTextStream(&titleText) << "Spectrum: "
			                  	<< PWD.section('/',-2,-2);		

             //setTitleFont(Helvetica9);
             //setAxisFont(QwtPlot::xBottom,Helvetica9);
             setAxisTitle(QwtPlot::xBottom, "E (MeV)");
             //setAxisFont(QwtPlot::yLeft,Helvetica9);
             setAxisTitle(QwtPlot::yLeft, "Rel. Prob.");
             setAxisScale(QwtPlot::yLeft, 0, 1.1, 0);
             //setAxisTitleFont(QwtPlot::xBottom,Helvetica9);
             //setAxisTitleFont(QwtPlot::yLeft,Helvetica9);

             crv[1] = new QwtPlotCurve("Calulated");
             crv[1]->setRenderHint(QwtPlotItem::RenderAntialiased);
             crv[1]->setPen(QPen(Qt::red));
             crv[1]->setYAxis(QwtPlot::yLeft);
             crv[1]->attach(this);
             crv[1]->setSamples(z, dc, nData);

             // curves
 //            crv[1] = insertCurve("Calulated");
 //            setCurvePen(crv[1], QPen(red));
 //            setCurveYAxis(crv[1], QwtPlot::yLeft);
 //            setCurveData(crv[1], z, dc, nData);
             enableLegend(false);
		}
    }
    else {
        //removeCurves();
        //removeMarkers();
        QwtPlotDict::detachItems(QwtPlotItem::Rtti_PlotCurve);
        QwtPlotDict::detachItems(QwtPlotItem::Rtti_PlotMarker);

        // axes
        enableAxis(QwtPlot::yRight);
        setAxisTitle(QwtPlot::xBottom, "Depth (cm)");
        setAxisTitle(QwtPlot::yLeft, "Percent Depth Dose (%)");
        setAxisTitle(QwtPlot::yRight, "Percent Dose Difference (%)");
        setAxisScale(QwtPlot::yLeft, 0, 110.0, 0);

        //    setAxisOptions(QwtPlot::xBottom, QwtAutoScale::Floating);
        //    setAxisMaxMajor(QwtPlot::xBottom, 6);
        //    setAxisMaxMinor(QwtPlot::xBottom, 10);

        // curves
        //crv[1] = insertCurve("Measured");
        //setCurvePen(crv[1], QPen(blue));
        //setCurveYAxis(crv[1], QwtPlot::yLeft);

        crv[1] = new QwtPlotCurve("Measured");
        crv[1]->setRenderHint(QwtPlotItem::RenderAntialiased);
        crv[1]->setPen(QPen(Qt::blue));
        crv[1]->setYAxis(QwtPlot::yLeft);
        crv[1]->attach(this);


        //crv[2] = insertCurve("Fitted");
        //setCurvePen(crv[2], QPen(red));
        //setCurveYAxis(crv[2], QwtPlot::yLeft);
        crv[2] = new QwtPlotCurve("Measured");
        crv[2]->setRenderHint(QwtPlotItem::RenderAntialiased);
        crv[2]->setPen(QPen(Qt::red));
        crv[2]->setYAxis(QwtPlot::yLeft);
        crv[2]->attach(this);

        //crv[3] = insertCurve("Diff");
        //setCurvePen(crv[3], QPen(darkYellow));
        //setCurveYAxis(crv[3], QwtPlot::yRight);

        crv[3] = new QwtPlotCurve("Diff");
        crv[3]->setRenderHint(QwtPlotItem::RenderAntialiased);
        crv[3]->setPen(QPen(Qt::darkYellow));
        crv[3]->setYAxis(QwtPlot::yLeft);
        crv[3]->attach(this);
        // toggleCurve(crv[3]);

        // marker
        mrk = new QwtPlotMarker();
        mrk->setLineStyle(QwtPlotMarker::VLine);
        mrk->setValue(0.0,0.0);
        mrk->setLabelAlignment(Qt::AlignRight|Qt::AlignBottom);
        mrk->setLinePen(QPen(Qt::black, 0, Qt::SolidLine));

        //mrk[1] = insertMarker();
        //setMarkerLineStyle(mrk[1], QwtMarker::VLine);
        //setMarkerPos(mrk[1], 0.0,0.0);
        //setMarkerLabelAlign(mrk[1], AlignRight|AlignBottom);
        //setMarkerPen(mrk[1], QPen(black, 0, SolidLine));
        //setMarkerFont(mrk[1], QFont("Helvetica", 10, QFont::Bold));
        /*
        mrk2 = insertLineMarker("", QwtPlot::yLeft);
        setMarkerLabelAlign(mrk2, AlignRight|AlignBottom);
        setMarkerPen(mrk2, QPen(QColor(200,150,0), 0, DashDotLine));
        setMarkerFont(mrk2, QFont("Helvetica", 10, QFont::Bold));
        setMarkerSymbol(mrk2,
        QwtSymbol(QwtSymbol::Diamond, yellow, green, QSize(7,7)));
        */
        //	 usrDataFile("tst.dat");
        QTextStream(&titleText) << "In-water Fitting: "
         //	 	                     << readSettingString("Machine/Machine")
                                     << PWD.section('/',-2,-2)
                                     << ": PDD of 10cm x 10cm at 100 cm";


         setAxisTitle(QwtPlot::xBottom, "Depth (cm)");

        // setCurveData(crv[1], z, dc, nData);
        // setCurveData(crv[2], z, dm, nData);
        // setCurveData(crv[3], z, dd, nData);

         crv[1]->setSamples(z, dc, nData);
         crv[2]->setSamples(z, dm, nData);
         crv[3]->setSamples(z, dd, nData);
	}
	
   setTitle(titleText);

   replot();
   
   //qwtZoomer[0]->setZoomBase();
   //qwtZoomer[1]->setZoomBase();

}
/*
void MyPlot2D::toggleCurve(long curveId) {
    QwtPlotCurve *c = curve(curveId);
    if ( c ) {
        c->setEnabled(!c->enabled());
        replot();
    }
}
*/
void MyPlot2D::enableLegend(bool value) {
    if (value) {
         // legend
         QwtLegend *legend = new QwtLegend;
         legend->setFrameStyle(QFrame::Box|QFrame::Sunken);
         this->insertLegend(legend, QwtPlot::RightLegend);
    }
    else {
         this->insertLegend(NULL);
    }
    replot();
}
