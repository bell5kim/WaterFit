#include <qregexp.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qstatusbar.h>
#include <qprinter.h>
#include <qpicture.h>
#include <qpainter.h>
#include <qprintdialog.h>

#include <qwt_counter.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_renderer.h>
#include <qwt_text.h>
#include <qwt_math.h>

#include <QDomElement>
#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QDateTime>
#include <QMessageBox>

#include "pixmaps.h"
// #include "myplot2d.h"
#include "waterfit.h"
#include "ui_waterfit.h"

using namespace std;
#include <iostream>
#include <cmath>
#include "WaterFitInput.h"

class Zoomer: public QwtPlotZoomer
{
public:
    Zoomer( int xAxis, int yAxis, QWidget *canvas ):
        QwtPlotZoomer( xAxis, yAxis, canvas )
    {
        setTrackerMode( QwtPicker::AlwaysOff );
        setRubberBand( QwtPicker::NoRubberBand );

        // RightButton: zoom out by 1
        // Ctrl+RightButton: zoom out to full size

        setMousePattern( QwtEventPattern::MouseSelect2,
            Qt::RightButton, Qt::ControlModifier );
        setMousePattern( QwtEventPattern::MouseSelect3,
            Qt::RightButton );
    }
};

WaterFitInput *wfit = new WaterFitInput;  // This should be declared at very beginning

WaterFit::WaterFit(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WaterFit)
{
    ui->setupUi(this);


    const int margin = 5;
    ui->qwtPlot->setContentsMargins( margin, margin, margin, 0 );

    setContextMenuPolicy( Qt::NoContextMenu );

    d_zoomer[0] = new Zoomer( QwtPlot::xBottom, QwtPlot::yLeft,
        ui->qwtPlot->canvas() );
    d_zoomer[0]->setRubberBand( QwtPicker::RectRubberBand );
    d_zoomer[0]->setRubberBandPen( QColor( Qt::green ) );
    d_zoomer[0]->setTrackerMode( QwtPicker::ActiveOnly );
    d_zoomer[0]->setTrackerPen( QColor( Qt::white ) );

    d_zoomer[1] = new Zoomer( QwtPlot::xTop, QwtPlot::yRight,
         ui->qwtPlot->canvas() );

    d_panner = new QwtPlotPanner( ui->qwtPlot->canvas() );
    d_panner->setMouseButton( Qt::MidButton );

    d_picker = new QwtPlotPicker( QwtPlot::xBottom, QwtPlot::yLeft,
        QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,
        ui->qwtPlot->canvas() );
    d_picker->setStateMachine( new QwtPickerDragPointMachine() );
    d_picker->setRubberBandPen( QColor( Qt::green ) );
    d_picker->setRubberBand( QwtPicker::CrossRubberBand );
    d_picker->setTrackerPen( QColor( Qt::white ) );

    // setCentralWidget( ui->qwtPlot );

    QToolBar *toolBar = new QToolBar( this );

    QToolButton *btnZoom = new QToolButton( toolBar );
    btnZoom->setText( "Zoom" );
    btnZoom->setIcon( QPixmap( zoom_xpm ) );
    btnZoom->setCheckable( true );
    btnZoom->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    toolBar->addWidget( btnZoom );
    connect( btnZoom, SIGNAL( toggled( bool ) ), SLOT( enableZoomMode( bool ) ) );

#ifndef QT_NO_PRINTER
    QToolButton *btnPrint = new QToolButton( toolBar );
    btnPrint->setText( "Print" );
    btnPrint->setIcon( QPixmap( print_xpm ) );
    btnPrint->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    toolBar->addWidget( btnPrint );
    connect( btnPrint, SIGNAL( clicked() ), SLOT( print() ) );
#endif

    QToolButton *btnExport = new QToolButton( toolBar );
    btnExport->setText( "Export" );
    btnExport->setIcon( QPixmap( print_xpm ) );
    btnExport->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    toolBar->addWidget( btnExport );
    connect( btnExport, SIGNAL( clicked() ), SLOT( exportDocument() ) );

    QToolButton *btnTable = new QToolButton( toolBar );
    btnTable->setText( "Table" );
    btnTable->setIcon( QPixmap( table_xpm ) );
    btnTable->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    toolBar->addWidget( btnTable );
    connect( btnTable, SIGNAL( clicked() ), SLOT( runGammaTable() ) );

/*
    toolBar->addSeparator();

    QWidget *hBox = new QWidget( toolBar );

    QHBoxLayout *layout = new QHBoxLayout( hBox );
    layout->setSpacing( 0 );
    layout->addWidget( new QWidget( hBox ), 10 ); // spacer
    layout->addWidget( new QLabel( "Damping Factor", hBox ), 0 );
    layout->addSpacing( 10 );

    QwtCounter *cntDamp = new QwtCounter( hBox );
    cntDamp->setRange( 0.0, 5.0 );
    cntDamp->setSingleStep( 0.01 );
    cntDamp->setValue( 0.0 );

    layout->addWidget( cntDamp, 0 );

    ( void )toolBar->addWidget( hBox );
*/

    addToolBar( toolBar );

#ifndef QT_NO_STATUSBAR
    ( void )statusBar();
#endif

    enableZoomMode( false );
    showInfo();

    //connect( cntDamp, SIGNAL( valueChanged( double ) ),
    //    ui->qwtPlot, SLOT( setDamp( double ) ) );

    // connect( cntDamp, &QwtCounter::valueChanged, damping, &Plot::damp );
    // connect( cntDamp, &QwtCounter::valueChanged, ui->qwtPlot, &Plot::damp );

    connect( d_picker, SIGNAL( moved( const QPoint & ) ),
        SLOT( moved( const QPoint & ) ) );

    connect( d_picker, SIGNAL( selected( const QPolygon & ) ),
        SLOT( selected( const QPolygon & ) ) );

    initApp();
/*
    setAirFitValues("afit.inp.tmp");
    // myPlot->showToolBar(this);
    ui->checkBoxAutoPlot->setChecked(true);
    iListBox = 3; // set to zListBox
    iListItem = 0; // set to first item
    plotAirFit();
    resetAirFitValues();
*/
}

WaterFit::~WaterFit()
{
    delete ui;
}

// CLASS Status ------------------------------------------------------
#define XML on
#ifdef XML
class Status {
  public:
     QString value, date;
};

QDomElement StatusToNode (QDomDocument &d, const Status &s, QString e){
   QDomElement elm = d.createElement(e);
   elm.setAttribute("value", s.value);
   elm.setAttribute("date", s.date);
   return elm;
};
#endif


void WaterFit::resetZoomer(){
    d_zoomer[0]->setZoomBase();
    d_zoomer[1]->setZoomBase();
}


#ifndef QT_NO_PRINTER

void WaterFit::print()
{
    QPrinter printer( QPrinter::HighResolution );

    QString docName = ui->qwtPlot->title().text();
    if ( !docName.isEmpty() )
    {
        docName.replace ( QRegExp ( QString::fromLatin1 ( "\n" ) ), tr ( " -- " ) );
        printer.setDocName ( docName );
    }

    printer.setCreator( "Water Fitting Report" );
    printer.setOrientation( QPrinter::Landscape );
    printer.setPageSize(QPrinter::Letter);
    printer.setColorMode(QPrinter::Color);

    QPrintDialog dialog( &printer );
    if ( dialog.exec() )
    {
        QwtPlotRenderer renderer;

        if ( printer.colorMode() == QPrinter::GrayScale )
        {
            renderer.setDiscardFlag( QwtPlotRenderer::DiscardBackground );
            renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasBackground );
            renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasFrame );
            renderer.setLayoutFlag( QwtPlotRenderer::FrameWithScales );
        }

        renderer.renderTo( ui->qwtPlot, printer );
    }
}

#endif

void WaterFit::exportDocument()
{
    QwtPlotRenderer renderer;
    renderer.exportTo( ui->qwtPlot, "WaterFit.pdf" );
}

void WaterFit::enableZoomMode( bool on )
{
    d_panner->setEnabled( on );

    d_zoomer[0]->setEnabled( on );
    d_zoomer[0]->zoom( 0 );

    d_zoomer[1]->setEnabled( on );
    d_zoomer[1]->zoom( 0 );

    //d_picker->setRubberBand(
    //    on ? QwtPicker::NoRubberBand: QwtPicker::CrossRubberBand);

    d_picker->setEnabled( !on );

    showInfo();
}

void WaterFit::showInfo( QString text )
{
    if ( text == QString::null )
    {
        if ( d_picker->rubberBand() )
            text = "Cursor Pos: Press left mouse button in plot region";
        else
            text = "Zoom: Press mouse button and drag";
    }

#ifndef QT_NO_STATUSBAR
    statusBar()->showMessage( text );
#endif
}

void WaterFit::moved( const QPoint &pos )
{
    QString info;
    float depth = ui->qwtPlot->invTransform(QwtPlot::xBottom, pos.x());
    int iDepth = 0;
    for (int i=0; i<ui->qwtPlot->nData-1; i++) {
      if ((ui->qwtPlot->z[i]-depth)*(ui->qwtPlot->z[i+1]-depth) <= 0) {
        iDepth = i;
        break;
      }
    }

    float mDose = 0.0;
    float cDose = 0.0;
    float dDose = 0.0;

    if ((ui->qwtPlot->z[iDepth+1]-ui->qwtPlot->z[iDepth]) != 0.0) {
      mDose = (ui->qwtPlot->dm[iDepth]*(ui->qwtPlot->z[iDepth+1]-depth) + ui->qwtPlot->dm[iDepth+1]
                *(depth-ui->qwtPlot->z[iDepth]))/(ui->qwtPlot->z[iDepth+1]-ui->qwtPlot->z[iDepth]);
      cDose = (ui->qwtPlot->dc[iDepth]*(ui->qwtPlot->z[iDepth+1]-depth) + ui->qwtPlot->dc[iDepth+1]
                *(depth-ui->qwtPlot->z[iDepth]))/(ui->qwtPlot->z[iDepth+1]-ui->qwtPlot->z[iDepth]);
    }
    if (mDose != 0.0) dDose = (cDose - mDose) / mDose * 100.0;

    info.sprintf("Depth=%.2f (cm), Dose(c)=%.2f (cGy), Dose(m)=%.2f (cGy), Diff=%.2f (%%)",
                  depth, cDose, mDose, dDose);

    showInfo( info );
}

void WaterFit::selected( const QPolygon & )
{
    showInfo();
}

// ----------------------------------------------------------

void WaterFit::initApp() {

    // Environment Settings -------------------------------------
    QString XVMC_WORK = getenv("XVMC_WORK");
    QString mName = XVMC_WORK.section('/',-1);
    ui->textLabelMachine->setText(mName);

#ifdef XVMC
    QString BDT_FILE = XVMC_WORK + "/dat/" + mName + "_GeoModel.bdt";
#else
    QString BDT_FILE = XVMC_WORK + "/dat/" + mName + ".bdt";
#endif
    QFile bdtFile(BDT_FILE);
     if (bdtFile.exists()) {
        readBDT(BDT_FILE); // Changed for mwfit 4.0
        QString Ee; Ee.sprintf("%6.3f",wfit->eEnergy);
        QString Re; Re.sprintf("%6.3f",wfit->FFRad);
        ui->lineEditEe->setText(Ee);
        ui->lineEditRe->setText(Re);
     } else {
        QMessageBox::critical( 0, "waterfit", "No "+BDT_FILE+" exists" );
        close();
     }
/*
     ui->qwtPlot->showToolBar(this);
#ifdef TOOLBAR
     ui->qwtPlotSpect->showToolBar(this);
#endif
*/
     int m=5;  // number of digits before decial point
     int n=2;  // number of digits after decial point
     int mFactor = 10;

     // Norm Group Setting
     ui->floatSpinBoxNorm->setRange(0,10);
     //ui->floatSpinBoxNorm->setValidator(m,n);
     ui->floatSpinBoxNorm->setDecimals(n);
     ui->floatSpinBoxNorm->setMaximum(5000.0);
     ui->floatSpinBoxNorm->setMinimum(0.0);
     ui->floatSpinBoxNorm->setValue(1500.0);
     //ui->floatSpinBoxNorm->setValidator(0);
     ui->sliderNorm->setRange(0*(int)pow(10.0,m+n-1),(int)(0.5*pow(10.0,m+n-1)));
     ui->sliderNorm->setTickInterval((int)(0.5*pow(10.0,m+n-2)));

     m=3;
     n=4;
     // Lval Group Setting
     ui->floatSpinBoxLval->setRange(0,10);
     //ui->floatSpinBoxLval->setValidator(m,n);
     ui->floatSpinBoxNorm->setDecimals(n);
     ui->floatSpinBoxLval->setMaximum(100.0);
     ui->floatSpinBoxLval->setMinimum(0.0);
     ui->floatSpinBoxLval->setValue(100.0);
     //ui->floatSpinBoxLval->setValidator(0);
     ui->sliderLval->setRange(0,1*(int)pow(10.0,m+n-1));
     ui->sliderLval->setTickInterval((int)(0.1*pow(10.0,m+n-1)));

     m=2;
     n=4;
     // Bval Group Setting
     ui->floatSpinBoxBval->setRange(0,10);
     //ui->floatSpinBoxBval->setValidator(m,n);
     ui->floatSpinBoxNorm->setDecimals(n);
     ui->floatSpinBoxBval->setMaximum(20.0);
     ui->floatSpinBoxBval->setMinimum(0.0);
     ui->floatSpinBoxBval->setValue(10.0);
     //ui->floatSpinBoxBval->setValidator(0);
     ui->sliderBval->setRange(0,2*(int)pow(10.0,m+n-1));
     ui->sliderBval->setTickInterval((int)(0.25*pow(10.0,m+n-1)));

     m=2;
     n=4;
      // Emin Group Setting
     ui->floatSpinBoxEmin->setRange(0,10);
     //ui->floatSpinBoxEmin->setValidator(m,n);
     ui->floatSpinBoxNorm->setDecimals(n);
     ui->floatSpinBoxEmin->setMaximum(2.0);
     ui->floatSpinBoxEmin->setMinimum(0.0);
     ui->floatSpinBoxEmin->setValue(0.25);
     //ui->floatSpinBoxEmin->setValidator(0);
     ui->sliderEmin->setRange(0,(int)(0.2*pow(10.0,m+n-1)));
     ui->sliderEmin->setTickInterval((int)(0.2*pow(10.0,m+n-2)));
     // sliderEmin->setValue(0);

     // Emax Group Setting
     ui->floatSpinBoxEmax->setRange(0,10);
     //ui->floatSpinBoxEmax->setValidator(m,n);
     ui->floatSpinBoxNorm->setDecimals(n);
     ui->floatSpinBoxEmax->setMaximum(30.0);
     ui->floatSpinBoxEmax->setMinimum(0.0);
     ui->floatSpinBoxEmax->setValue(10.0);
     //ui->floatSpinBoxEmax->setValidator(0);
     ui->sliderEmax->setRange(0,3*(int)pow(10.0,m+n-1));
     ui->sliderEmax->setTickInterval((int)(0.25*pow(10.0,m+n-1)));

     m=2;
     n=4;
     // Pcon Group Setting
     ui->floatSpinBoxPcon->setRange(0,10);
     //ui->floatSpinBoxPcon->setValidator(m,n);
     ui->floatSpinBoxNorm->setDecimals(n);
     ui->floatSpinBoxPcon->setMaximum(1.0);
     ui->floatSpinBoxPcon->setMinimum(-1.0);
     ui->floatSpinBoxPcon->setValue(0.0045);
     //ui->floatSpinBoxPcon->setValidator(0);
     ui->sliderPcon->setRange((int)(-0.1*pow(10.0,m+n-1)),(int)(0.1*pow(10.0,m+n-1)));
     ui->sliderPcon->setTickInterval((int)(0.1*pow(10.0,m+n-2)));

     m=2;
     n=4;
     /* REMOVED
     // A Group Setting
     ui->floatSpinBoxAval->setRange(0,10);
     ui->floatSpinBoxAval->setValidator(m,n);
     ui->floatSpinBoxAval->setMaximum(100.0);
     ui->floatSpinBoxAval->setMinimum(-10.0);
     ui->floatSpinBoxAval->setValue(0.0);
     ui->floatSpinBoxAval->setValidator(0);
     ui->sliderAval->setRange(0,10*(int)pow(10.0,m+n-1));
     ui->sliderAval->setTickInterval((int)(0.5*pow(10.0,m+n-1)));
     */

     // QRegExp regExp2d("[1-9]\\d{0,1}");
     QRegExp regExpReal("\\d{0,3}\\.\\d{0,2}");
     ui->lineEditE->setValidator(new QRegExpValidator(regExpReal, this));
     ui->lineEditEp->setValidator(new QRegExpValidator(regExpReal, this));
     ui->lineEditEmean->setValidator(new QRegExpValidator(regExpReal, this));

     updateFS();
     initWaterFit();
     resetWaterFitValues();  // from wfit.inp

     if (readSettingString("MULTIWFIT") == "Checked")
        ui->checkBoxFS2->setChecked(true);
     else
        ui->checkBoxFS2->setChecked(false);

     QFile wFile("wfit.inp.tmp");
     if (wFile.exists()) {
         QString CMD = "rm wfit.inp.tmp";
         if (system(CMD.toLatin1()) != 0) {
            QTextStream (stdout) << "ERROR: Somethings are wrong: " << CMD << endl ;
         }
     }
     writeSpect();

}

void WaterFit::updateFS(){
    QString FS1 = readSettingString("FS1");
    QString FS2 = readSettingString("FS2");

    bool ok;
    if (FS1 != "None") {
        wfit->FS1 = FS1.toInt(&ok,10)/10;
        bool exist = false;
        for (int i=0; i<ui->comboBoxFS->count(); i++){
            if (ui->comboBoxFS->itemText(i) == FS1) exist = true;
        }
        if (!exist) ui->comboBoxFS->insertItem(-1,FS1);
    }

    if (FS2 == "None") {
        wfit->FS2 = -1;
        ui->checkBoxFS2->setEnabled(false);
    }
    else {
        ui->checkBoxFS2->setEnabled(true);
        ui->checkBoxFS2->setText("Plus 2nd FS = " + FS2 + " x " + FS2 + " (mm x mm)");

        bool exist = false;
        for (int i=0; i<ui->comboBoxFS->count(); i++){
            if (ui->comboBoxFS->itemText(i) == FS2) exist = true;
        }
        if (!exist) ui->comboBoxFS->insertItem(-1,FS2);

        if (ui->checkBoxFS2->isChecked()) {
            wfit->FS2 = FS2.toInt(&ok,10)/10;
        }
        else {
            wfit->FS2 = -1;
        }
    }
    toggleFS();
}

void WaterFit::runGammaTable() {

   // char fname[256];

   QString LBIN;
   LBIN = getenv("XVMC_HOME");

   QString CMD = LBIN + "/gammatable.exe -i " + ui->qwtPlot->ddFile;

  if (system(CMD.toLatin1()) != 0) {
     QTextStream (stdout) << "ERROR: Somethings are wrong: " << CMD << '\n' ;
  }
}



// ----------------------------------------------
void WaterFit::initWaterFit() {
    wfit->initWaterFitInput();
    // QTextStream(stdout) << "* initAirFit got initAirFitInput " <<  endl;
}


void WaterFit::readWaterFit(char *fname) {
   wfit->readWaterFitInput(fname);
}

void WaterFit::writeWaterFit(char *fname) {
   getFloatSpinBoxLval();
   getFloatSpinBoxBval();
   getFloatSpinBoxAval();
   getFloatSpinBoxEmin();
   getFloatSpinBoxEmax();
   getFloatSpinBoxPcon();
   getFloatSpinBoxNorm();
   updateFS();
   // QTextStream (stdout) << "wfit->" << wfit->inorm << wfit->ilval << wfit->ibval << wfit->iEmin << wfit->iEmax << wfit->ipcon << endl;
   wfit->writeWaterFitInput(fname);
}


void WaterFit::runWaterFit() {
  writeWaterFit("wfit.inp.tmp");
  QString EXE="wfit";
  if (ui->checkBoxVersion->isChecked()) EXE="wfit_old";
  QFile wFile("wfit.inp.tmp");
  if (wFile.exists()) {
     wfit->readWaterFitInput("wfit.inp.tmp");
     QString LBIN = getenv("XVMC_HOME");
#ifdef CMS_SPECT
     QString CMD = LBIN + "/wfit_cms wfit.inp.tmp > wfit.out.tmp";
#elif XVMC
     QString CMD = LBIN + "/wfit_xvmc wfit.inp.tmp > wfit.out.tmp; ";
#else
     QString CMD = LBIN + "/"+EXE+" wfit.inp.tmp > wfit.out.tmp";
#endif
     if (system(CMD.toLatin1()) != 0) {
        QTextStream (stdout) << "ERROR: Somethings are wrong: " << CMD << endl ;
     }

     QString Energy = ui->lineEditE->text();

     bool ok;
     QString Eng;
     Eng.sprintf("%02d",(int)Energy.toFloat(&ok));
     // Copy calculated diff.dd to current directory
     QString WFIT_DIR = getenv("PWD");
#ifdef XVMC
     QString WFIT_DD_FILE = WFIT_DIR + "/x"+Eng+"/diff"+Eng+"mv.dd";
#else
     QString WFIT_DD_FILE = WFIT_DIR + "/results.diff";
#endif
     QFile ddFile(WFIT_DD_FILE);
     if (ddFile.exists()) {
        CMD = "cp " + WFIT_DD_FILE + " results.diff.tmp";
        if (system(CMD.toLatin1()) != 0) {
           QTextStream (stdout) << "ERROR: Somethings are wrong: " << CMD << endl ;
        }
        ui->qwtPlot->usrDataFile("results.diff.tmp");
     }
  }
  readOut("wfit.out.tmp");
  writeSpect();
}

void WaterFit::readOut(char *fname) {
  bool ok;
  QString CMDERROR = "";
  QString FNAME = fname;
  QFile mFile(fname);
  if (mFile.exists()) {
    QTextStream stream( &mFile );
    QString sLine;
    mFile.open( QIODevice::ReadOnly );
    while ( !stream.atEnd() ) {
      sLine = stream.readLine();
      if (!sLine.contains(',')) {
        if (sLine.contains("norm:")) {
           QString norm = sLine.section(':',1,1).section("+/-",0,0);
           wfit->norm = norm.simplified().toFloat(&ok);
        }
        if (sLine.contains("l_value:")) {
           QString lval = sLine.section(':',1,1);
           wfit->lval = lval.simplified().toFloat(&ok);
        }
        if (sLine.contains("b_value:")) {
           QString bval = sLine.section(':',1,1);
           wfit->bval = bval.simplified().toFloat(&ok);
        }
          /* REMOVED
        if (sLine.contains("a_value:")) {
           QString aval = sLine.section(':',1,1);
           wfit->aval = aval.simplified().toFloat(&ok);
        }
        */
        if (sLine.contains("energy_min:")) {
           QString Emin = sLine.section(':',1,1);
           wfit->Emin = Emin.simplified().toFloat(&ok);
        }
        if (sLine.contains("energy_max:")) {
           QString Emax = sLine.section(':',1,1);
           wfit->Emax = Emax.simplified().toFloat(&ok);
        }
        if (sLine.contains("p_con:")) {
           QString pcon = sLine.section(':',1,1);
           wfit->pcon = pcon.simplified().toFloat(&ok);
        }
        if (sLine.contains("gy_mu_dmax:")) {
           QString gymu = sLine.section(':',1,1).section('(',0,0);
           wfit->gy_mu_dmax = gymu.simplified().toFloat(&ok);
        }
        if (sLine.contains("Emean:")) {
           QString Emean = sLine.section(':',1,1);
           wfit->Emean = Emean.simplified().toFloat(&ok);
        }
        if (sLine.contains("Eprob:")) {
           QString Eprob = sLine.section(':',1,1);
           wfit->Eprob = Eprob.simplified().toFloat(&ok);
        }
        if (sLine.contains("norm_value:")) {
           QString normValue = sLine.section(':',1,1);
           wfit->normValue = normValue.simplified().toFloat(&ok);
        }
        if (sLine.contains("Writing file:")) {
           QString writeFile = sLine.section(':',1,1);
           wfit->writeFile = writeFile.simplified();
        }
        if (sLine.contains("Numerical Recipes run-time error...")) {
           CMDERROR = "Numerical Recipes run-time error...";
        }
      } else {
        if (sLine.contains("number of total points:")) {
           QString DEV = sLine.section("average deviation:",1,1)
                              .section("%",0,0);
           wfit->avgdev = DEV.simplified().toFloat(&ok);
        }
        if (sLine.contains("number of fit points:")) {
           QString CHI = sLine.section("chi:",1,1).section("%",1,1);
           wfit->chi = CHI.simplified().toFloat(&ok);
        }
      }
    } // End of While
    mFile.close();
   }
   if (CMDERROR.length() > 0 && FNAME == "wfit.out.tmp")   {
      QMessageBox::critical( 0, "wfit",CMDERROR );
      QString FNAME_PREV = "wfit.out.tmp.prev";
      QFile prevFile(FNAME_PREV);
      if (prevFile.exists()) {
         readOut("wfit.out.tmp.prev");
      }
   } else {
      QFile prevFile("wfit.out.tmp");
      if (prevFile.exists()) {
         QString CMD = "cp wfit.out.tmp wfit.out.tmp.prev";
         if (system(CMD.toLatin1()) != 0) {
            QTextStream (stdout) << "ERROR: Somethings are wrong: " << CMD << endl ;
         }
      }
      newWaterFitValues();
      QString fName(fname);
      fName.replace(".out",".inp");
      wfit->writeWaterFitInput(fName.toLatin1());
   }
}

void WaterFit::guessSpect() {
 QString XVMC_HOME = getenv("XVMC_HOME");
 QString XVMC_HOME_LIB = XVMC_HOME.section("bin",0,0)+"/lib";
 QString PWD = getenv("PWD");
 QString refSpect = "reference_spectrum.dat";
 QString refSpectFile = "";
 //QTextStream (stdout) << "PWD = " << PWD+"/"+refSpect << endl;
 QFile refFile(PWD+"/"+refSpect);
 if (!refFile.exists()) {
   refSpectFile = QFileDialog::getOpenFileName(
        this,"Choose a spectrum file",
        XVMC_HOME_LIB,
        " Spectrum (*.spectrum *.Spect *.spect)"
        // "Reference Spectrum Dialog",
   );
   // QTextStream (stdout) << "XVMC_HOME_LIB = " << XVMC_HOME_LIB << endl;
   //QTextStream (stdout) << "reference spectrum file = " << refSpectFile << endl;
   if (refSpectFile.section("lib/",1,1).length() > 0) {
     QString CMD = "cp " + refSpectFile + " " + refSpect;
     if (system(CMD.toLatin1()) != 0) {
       QTextStream (stdout) << "ERROR: Somethings are wrong: " << CMD << endl ;
     }
   }
 }
 else {
     QTextStream (stdout) << "PWD = " << PWD+"/"+refSpect << endl;
     getFloatSpinBoxLval();
     getFloatSpinBoxBval();
     getFloatSpinBoxAval();
     getFloatSpinBoxEmin();
     getFloatSpinBoxEmax();
     getFloatSpinBoxNorm();
     QString Option = "";
     if (!wfit->inorm) Option.sprintf(" %s -N %f ",Option.toStdString().c_str(),wfit->norm);
     if (!wfit->ilval) Option.sprintf(" %s -L %f ",Option.toStdString().c_str(),wfit->lval);
     if (!wfit->ibval) Option.sprintf(" %s -B %f ",Option.toStdString().c_str(),wfit->bval);
     if (!wfit->iEmax) Option.sprintf(" %s -Emax %f ",Option.toStdString().c_str(),wfit->Emax);
     if (!wfit->iEmin) Option.sprintf(" %s -Emin %f ",Option.toStdString().c_str(),wfit->Emin);
     // QTextStream (stdout) << Option << endl;
#ifdef CMS_SPECT
     QString CMD = XVMC_HOME + "/sFitting_cms.exe -i " + refSpect + Option + " > sFitting.out";
#elif XVMC
     QString CMD = XVMC_HOME + "/sFitting_xvmc.exe -i " + refSpect + Option + " > sFitting.out";
#else
     QString CMD = XVMC_HOME + "/sFitting.exe -i " + refSpect + Option + " > sFitting.out";
#endif
     if (system(CMD.toLatin1()) != 0) {
       QTextStream (stdout) << "ERROR: Somethings are wrong: " << CMD << endl ;
     } else {
       bool ok;
       QFile sFile("sFitting.out");
       if (sFile.exists()) {
         QTextStream stream( &sFile );
         QString sLine;
         sFile.open( QIODevice::ReadOnly );
         while ( !stream.atEnd() ) {
           sLine = stream.readLine();
           if (sLine.contains("final L")) {
             QString lval = sLine.section("final L",1,1).section("=",1,1);
             wfit->lval = lval.simplified().toFloat(&ok);
           }
           if (sLine.contains("final B")) {
             QString bval = sLine.section("final B",1,1).section("=",1,1);
             wfit->bval = bval.simplified().toFloat(&ok);
           }
           if (sLine.contains("final Emax")) {
             QString Emax = sLine.section("final Emax",1,1).section("=",1,1);
             wfit->Emax = Emax.simplified().toFloat(&ok);
           }
           if (sLine.contains("final N")) {
             QString norm = sLine.section("final N",1,1).section("=",1,1);
             wfit->norm = norm.simplified().toFloat(&ok)*1000;
           }
         } // End of While
         wfit->norm = 1000.0; // Fix to aviod nan
         setFloatSpinBoxNorm(wfit->norm);
         setFloatSpinBoxLval(wfit->lval);
         setFloatSpinBoxBval(wfit->bval);
         setFloatSpinBoxEmax(wfit->Emax);
       } // End of if (sFile.exists())
     } // else
  writeSpect();
 } // if
}

void WaterFit::Interrupt() {
  writeSettingString("WFIT", "NotDone");
  // QTextStream (stdout) << "Canceled\n";
  close();
}

void WaterFit::Done() {
 QString pwd = getenv("PWD");
 QFile wfitInpFile("wfit.inp.tmp");
 if (wfitInpFile.exists()) {
   QString CMD = "mv -f wfit.inp.tmp wfit.inp";
   if (system(CMD.toLatin1()) != 0) {
       QTextStream (stdout) << "ERROR: Somethings are wrong: " << CMD << endl ;
   }
 }
 QFile wfitOutFile("wfit.out.tmp");
 if (wfitOutFile.exists()) {
   QString CMD = "mv -f wfit.out.tmp wfit.out";
   if (system(CMD.toLatin1()) != 0) {
       QTextStream (stdout) << "ERROR: Somethings are wrong: " << CMD << endl ;
   }
 }

 if (makeDir(pwd.section('/',0,-2) + "/dat")){
    QString bdtFile = pwd.section('/',0,-2) + "/dat/"
                    + pwd.section('/',-2,-2) + ".bdt";
#ifdef XVMC
    QString bdtFileMono = pwd.section('/',0,-2) + "/dat/"
                    + pwd.section('/',-2,-2) + "_GeoModel.bdt";
    readBDT(bdtFileMono);
#else
  readBDT(bdtFile); // Changed for mwfit 4.0
#endif
 // getFloatSpinBoxAval();  // REMOVED
    readOut("wfit.out");
    writeBDT(bdtFile);
 }
 if (ui->checkBoxFS2->isChecked())
    writeSettingString("MULTIWFIT", "Checked");
 else
    writeSettingString("MULTIWFIT", "NotChecked");
  writeSettingString("WFIT", "Done");
 //QTextStream (stdout) << "Done\n";
 close();
}

void WaterFit::plotWaterFit() {
 QString LBIN = getenv("XVMC_HOME");
 QString CMD = "cd .; " + LBIN + "/plotPDD.exe";
   if (system(CMD.toLatin1()) != 0) {
       QTextStream (stdout) << "ERROR: Somethings are wrong: " << CMD << '\n' ;
   }
}

QString WaterFit::readSettingString(QString keyWord){
   QString XVMC_WORK = getenv("XVMC_WORK");
   QString group = XVMC_WORK.section('/',-1);
   QString keyValue = "";
#ifdef XML
   QString mName = group;
   QFile xmlFile(XVMC_WORK+"/status.xml");
   // QTextStream (stdout) << XVMC_WORK+"/status.xml" << endl;
   if (!xmlFile.open( QIODevice::ReadOnly )){
      QTextStream (stdout) << "No " << XVMC_WORK+"/status.xml is found" << endl;
      exit(-1);
   }

   QDomDocument docType(mName);
   docType.setContent(&xmlFile);
   xmlFile.close();

   QDomElement root = docType.documentElement();
   // QTextStream (stdout) << "root.tagName()=" << root.tagName() << endl;
   if (root.tagName() != mName) {
      // QTextStream (stdout) << "Tag Name ("<<root.tagName()<<") is different from " << mName << endl;
      exit(-1);
   }

   QDomNode n = root.firstChild();

   while (!n.isNull()){
      QDomElement e = n.toElement();
      if(!e.isNull()) {
         if(e.tagName() == keyWord) keyValue = e.attribute("value", "");
      }
      n = n.nextSibling();
   }
#else

   QString Entry = "/"+group+"/" + keyWord;
   QSettings settings;
   settings.insertSearchPath( QSettings::Unix, XVMC_WORK);
   settings.beginGroup("/"+group);
   QString keyValue = settings.readEntry(Entry, "");
   settings.endGroup();
#endif
   return (keyValue.simplified());
}

void WaterFit::writeSettingString(QString keyWord, QString keyValue){
   QDateTime currentDateTime = QDateTime::currentDateTime();
   QString DT = currentDateTime.toString();
   QString XVMC_WORK = getenv("XVMC_WORK");
   QString group = XVMC_WORK.section('/',-1);
#ifdef XML
   QString mName = group;
   QFile xmlFile(XVMC_WORK+"/status.xml");
   // QTextStream (stdout) << XVMC_WORK+"/status.xml" << endl;
   if (!xmlFile.open( QIODevice::ReadOnly )){
      QTextStream (stdout) << "No " << XVMC_WORK+"/status.xml is found" << endl;
      exit(-1);
   }

   QDomDocument doc(mName);
   doc.setContent(&xmlFile);
   xmlFile.close();

   QDomElement root = doc.documentElement();
   if (root.tagName() != mName) {
      // QTextStream (stdout) << "Tag Name ("<<eRoot.tagName()<<") is different from " << mName << endl;
      exit(-1);
   }

   QDomNode n = root.firstChild();

   // Set attribute for existing tag
   bool isDone = false;
   while (!n.isNull()){
      QDomElement e = n.toElement();
      if(!e.isNull()) {
         if(e.tagName() == keyWord) {
        e.setAttribute("value", keyValue);
        isDone = true;
         }
      }
      n = n.nextSibling();
   }

   if(!isDone) {
      Status s;  // Status class

      QString eName = keyWord;
      s.value = keyValue;  s.date = DT;
      root.appendChild(StatusToNode(doc, s, eName));
   }

   xmlFile.open( QIODevice::WriteOnly );
   QTextStream txtStrm(&xmlFile);
   txtStrm << doc.toString();
   xmlFile.close();

#else
   QString Entry = "/"+group+"/" + keyWord;
   QSettings settings;
   settings.insertSearchPath(QSettings::Unix, XVMC_WORK);
   settings.beginGroup("/"+group);
   settings.writeEntry(Entry, keyValue);
   settings.endGroup();
   // QTextStream (stdout) << "End of writeSetting" << endl;
#endif
}

void WaterFit::writeSpect() {
  getFloatSpinBoxNorm();
  getFloatSpinBoxBval();
  getFloatSpinBoxAval();
  getFloatSpinBoxLval();
  getFloatSpinBoxEmin();
  getFloatSpinBoxEmax();
  getFloatSpinBoxPcon();
  float dE = 0.01;

  QFile oFile("spectrum.dat");
  oFile.remove();
  oFile.open( QIODevice::ReadWrite );
  QTextStream oStream( &oFile );

  QString XVMC_WORK = getenv("XVMC_WORK");
  QString mName = XVMC_WORK.section('/',-1);
  QFile sFile(mName+".spect");
  sFile.remove();
  sFile.open( QIODevice::ReadWrite );
  QTextStream sStream( &sFile );

  float peak = 0.0;
#ifdef CMS_SPECT
  float Emin = 0.25;
#endif
  for (float E = dE; E < wfit->Emax; E += dE) {
#ifdef CMS_SPECT
  float p = wfit->norm * (exp(-wfit->lval*Emin)-exp(-wfit->lval*E))*(exp(-wfit->bval*E) - exp(-wfit->bval*wfit->Emax));
#elif XVMC
  float p = wfit->norm * (pow(E,wfit->lval)*(exp(-wfit->bval*E)));
#else
  float p = wfit->norm * (1.0-exp(-wfit->lval*E))*(exp(-wfit->bval*E) - exp(-wfit->bval*wfit->Emax));
#endif
    if (peak <= p) peak = p;
  }

  for (float E = dE; E < wfit->Emax; E += dE) {
    oStream << E
            << "  "
#ifdef CMS_SPECT
            <<  wfit->norm/peak * (exp(-wfit->lval*Emin)-exp(-wfit->lval*E))*(exp(-wfit->bval*E) - exp(-wfit->bval*wfit->Emax))
            << "  "
            <<  wfit->norm/peak * (exp(-wfit->lval*Emin)-exp(-wfit->lval*E))*(exp(-wfit->bval*E) - exp(-wfit->bval*wfit->Emax))
#elif XVMC
            <<  wfit->norm/peak * (pow(E,wfit->lval)*(exp(-wfit->bval*E)))
            << "  "
            <<  wfit->norm/peak * (pow(E,wfit->lval)*(exp(-wfit->bval*E)))
#else
            <<  wfit->norm/peak * (1.0-exp(-wfit->lval*E))*(exp(-wfit->bval*E) - exp(-wfit->bval*wfit->Emax))
            << "  "
            <<  wfit->norm/peak * (1.0-exp(-wfit->lval*E))*(exp(-wfit->bval*E) - exp(-wfit->bval*wfit->Emax))
#endif
            << "  0.0"
            << endl;

    sStream << E
            << "  "
#ifdef CMS_SPECT
            <<  wfit->norm/peak * (exp(-wfit->lval*Emin)-exp(-wfit->lval*E))*(exp(-wfit->bval*E) - exp(-wfit->bval*wfit->Emax))
#elif XVMC
            <<  wfit->norm/peak * (pow(E,wfit->lval)*(exp(-wfit->bval*E)))
#else
            <<  wfit->norm/peak * (1.0-exp(-wfit->lval*E))*(exp(-wfit->bval*E) - exp(-wfit->bval*wfit->Emax))
#endif
            << endl;
  }
  oFile.close();
  sFile.close();
  QFile spectFile("spectrum.dat");
  if (spectFile.exists()) ui->qwtPlotSpect->usrDataFile("spectrum.dat");

  QFile refFile("reference_spectrum.dat");
  if (refFile.exists()) ui->qwtPlotSpect->usrDataFile("reference_spectrum.dat");
}

void WaterFit::toggleSpectButton() {
  if (ui->spectButton->text().contains("Hide")) {
   // QTextStream (stdout) << "Hide:" << spectButton->text() << endl;
   ui->qwtPlotSpect->setGeometry(QRect(300,0,0,0));
   ui->qwtPlotSpect->enableLegend(false);
   ui->qwtPlotSpect->hide();
   ui->spectButton->setText("Show &Spectrum");
  } else {
   // QTextStream (stdout) << "Show:" << spectButton->text() << endl;
   ui->qwtPlotSpect->setGeometry(QRect(300,0,310,250));
   ui->qwtPlotSpect->enableLegend(false);
   ui->spectButton->setText("Hide &Spectrum");
  }
  writeSpect();
}

void WaterFit::writeBDT(QString bdtFile) {

 QString pwd = getenv("PWD");
 QString machineInfo = pwd.section('/',0,-2) + "/"
                     + pwd.section('/',-2,-2) + ".info";
 QString MAXFW = "";
 QString MAXFL = "";
 QString AVAL = "";
 QString ZVAL = "";

 QFile mFile(machineInfo);
 if (mFile.exists()) {
  QTextStream stream( &mFile );
  QString sLine;
  mFile.open( QIODevice::ReadOnly );
  // QTextStream (stdout) << modelFile << " is open" << endl;
  while ( !stream.atEnd() ) {
   sLine = stream.readLine();
   QString strLine = sLine.toLatin1();
   QString keyWord = strLine.section('=',0,0);
   QString keyValueTmp = strLine.section('#',0,0).section('=',1,1);
   QString keyValue = keyValueTmp.simplified();
   if (keyWord.contains("MAXFW"))  MAXFW = keyValue;
   if (keyWord.contains("MAXFL"))  MAXFL = keyValue;
   if (keyWord.contains("AVAL"))   AVAL  = keyValue;
   if (keyWord.contains("ZVAL"))   ZVAL  = keyValue;
  }
 }
 mFile.close();
 bool ok;
 // wfit->aval = AVAL.toFloat(&ok); // Take A-Value from Machine Setting
 // wfit->aval = ui->floatSpinBoxAval->text().toFloat(&ok); // Take A-Value from WaterFit GUI
 wfit->aval = ui->lineEditAval->text().toFloat(&ok); // Take A-Value from WaterFit GUI
 wfit->zval = ZVAL.toFloat(&ok); // Take Z-Value from Machine Setting

 QFile oFile(bdtFile);
 oFile.remove();
 oFile.open( QIODevice::ReadWrite );
 QTextStream oStream( &oFile );
// oStream << "BASE-DATA-FILE-VERSION:  1.3" << endl;
#ifdef XVMC
 oStream << "BASE-DATA-FILE-VERSION:  1.4" << endl;
#else
 oStream << "BASE-DATA-FILE-VERSION:  1.5" << endl;
#endif
 oStream << "BASE-DATA-ENTRY" << endl;
 oStream << "   PARTICLE-TYPE:     " << wfit->particle << endl;
 oStream << "   NOMINAL-ENERGY:    "
         << QString("%1").arg(wfit->E,0,'f',2) << endl;
 oStream << "   BEAM-MODEL-ID:     " << abs(wfit->ID) << endl;
 oStream << "   BEGIN-PARAMETERS" << endl;
 oStream << "      PRIMARY-PHOTONS:    "
         << QString("%1").arg(wfit->p0,0,'f',5) << endl;
#ifdef XVMC
 oStream << "      PRIMARY-DIST:       "
         << QString("%1").arg(wfit->Z0,0,'f',3) << endl;
#endif
 oStream << "      PRIMARY-SIGMA:      "
         << QString("%1").arg(wfit->s0,0,'f',5) << endl;
 oStream << "      PRIMARY-HORN0:      "
         << QString("%1").arg(wfit->h0,0,'f',5) << endl;
 oStream << "      PRIMARY-HORN1:      "
         << QString("%1").arg(wfit->h1,0,'f',5) << endl;
 oStream << "      PRIMARY-HORN2:      "
         << QString("%1").arg(wfit->h2,0,'f',5) << endl;
 oStream << "      PRIMARY-HORN3:      "
         << QString("%1").arg(wfit->h3,0,'f',5) << endl;
 oStream << "      PRIMARY-HORN4:      "
         << QString("%1").arg(wfit->h4,0,'f',5) << endl;
 oStream << "      SCATTER-DIST:       "
         << QString("%1").arg(wfit->ZS,0,'f',3) << endl;
 oStream << "      SCATTER-SIGMA:      "
         << QString("%1").arg(wfit->ss,0,'f',5) << endl;
#ifdef XVMC
 oStream << "      COLM-DIST:          "
         << QString("%1").arg(wfit->ZM,0,'f',3) << endl;
 oStream << "      COLC-DIST:          "
         << QString("%1").arg(wfit->ZC,0,'f',3) << endl;
 oStream << "      COLX-DIST:          "
         << QString("%1").arg(wfit->ZX,0,'f',3) << endl;
 oStream << "      COLY-DIST:          "
         << QString("%1").arg(wfit->ZY,0,'f',3) << endl;
#endif
 oStream << "      NORM-VALUE:         "
         << QString("%1").arg(wfit->normValue,0,'f',5) << endl;
 oStream << "      GY/MU-DMAX:         "
         << QString("%1").arg(wfit->gy_mu_dmax,0,'f',5) << endl;
 oStream << "      ENERGY-MIN:         "
         << QString("%1").arg(wfit->Emin,0,'f',5) << endl;
 oStream << "      ENERGY-MAX:         "
         << QString("%1").arg(wfit->Emax,0,'f',5) << endl;
 oStream << "      L-VALUE:            "
         << QString("%1").arg(wfit->lval,0,'f',5) << endl;
 oStream << "      B-VALUE:            "
         << QString("%1").arg(wfit->bval,0,'f',5) << endl;
 oStream << "      A-VALUE:            "
         << QString("%1").arg(wfit->aval,0,'f',5) << endl;
 oStream << "      Z-VALUE:            "
         << QString("%1").arg(wfit->zval,0,'f',5) << endl;
 if (wfit->nu == 0.0) wfit->nu = 0.45;
 oStream << "      NU-VALUE:           "
         << QString("%1").arg(wfit->nu,0,'f',3) << endl;
 oStream << "      CHARGED-PARTICLES:  "
         << QString("%1").arg(wfit->pcon,0,'f',5) << endl;
 oStream << "      CHARGED-DIST:       "
         << QString("%1").arg(wfit->ZS,0,'f',3) << endl;
 oStream << "      CHARGED-RADIUS:     "
         << QString("%1").arg(wfit->FFRad,0,'f',3) << endl;
 oStream << "      CHARGED-E-MEAN:     "
         << QString("%1").arg(wfit->eEnergy,0,'f',3) << endl;
 oStream << "   END-PARAMETERS" << endl;

 oFile.close();
}

QString WaterFit::getKeyValue(QString strLine) {
 QString keyValue = strLine.section(':',1,1);
 return (keyValue.simplified());
}

void WaterFit::readBDT(QString fName) {
 // define bool variables
 QString version          = "";      bool version_found       = false;
 QString entry            = "";      bool entry_found         = false;
 QString ptl_type         = "";      bool ptl_type_found      = false;
 QString energy           = "";      bool energy_found        = false;
 QString model_id         = "";      bool model_id_found      = false;
 QString begin_parameter  = "";      bool begin_para_found    = false;
 QString end_parameter    = "";      bool end_para_found      = false;

 // initialization
 QString p_pri            = "";      bool p_pri_found         = false;
 QString p_sct            = "";
 QString pri_distance     = "";      bool pri_distance_found  = false;
 QString sigma_pri        = "";      bool sigma_pri_found     = false;
 QString horn_0           = "";      bool horn_0_found        = false;
 QString horn_1           = "";      bool horn_1_found        = false;
 QString horn_2           = "";      bool horn_2_found        = false;
 QString horn_3           = "";      bool horn_3_found        = false;
 QString horn_4           = "";      bool horn_4_found        = false;
 QString sct_distance     = "";      bool sct_distance_found  = false;
 QString sigma_sct        = "";      bool sigma_sct_found     = false;
 QString col_mdistance    = "";      bool col_mdistance_found = false;
 QString col_cdistance    = "";      bool col_cdistance_found = false;
 QString col_xdistance    = "";      bool col_xdistance_found = false;
 QString col_ydistance    = "";      bool col_ydistance_found = false;
 QString norm_value       = "";      bool norm_value_found    = false;
 QString gray_mu_dmax     = "";      bool gray_mu_dmax_found  = false;
 QString energy_min       = "";      bool energy_min_found    = false;
 QString energy_max       = "";      bool energy_max_found    = false;
 QString l_value          = "";      bool l_value_found       = false;
 QString b_value          = "";      bool b_value_found       = false;
 QString a_value          = "";      bool a_value_found       = false;
 QString z_value          = "";      bool z_value_found       = false;
 QString p_con            = "";      bool p_con_found         = false;
 QString distance_con     = "";      bool distance_con_found  = false;
 QString radius_con       = "";      bool radius_con_found    = false;
 QString e_mean_con       = "";      bool e_mean_con_found    = false;
 QString nu_value         = "0.45";  bool nu_value_found      = false;

 bool ok;
 // read input file
 QFile iFile( fName );
 if ( iFile.open( QIODevice::ReadOnly ) ) {
  QTextStream stream( &iFile );
  QString line;
  while ( !stream.atEnd() ) {
   line = stream.readLine(); // line of text excluding '\n'
   QString strLine = line.toLatin1();
   if (strLine.contains("BASE-DATA-FILE-VERSION:") != 0) {
    version = getKeyValue(strLine); version_found = true;
    wfit->version = version;
   }
   if (strLine.contains("BASE-DATA-ENTRY") != 0) {
    entry_found = true;
   }
   if (strLine.contains("PARTICLE-TYPE:") != 0) {
    ptl_type = getKeyValue(strLine); ptl_type_found = true;
    wfit->particle = ptl_type;
   }
   if (strLine.contains("NOMINAL-ENERGY:") != 0) {
    energy = getKeyValue(strLine); energy_found = true;
    wfit->E = energy.toFloat(&ok);
   }
   if (strLine.contains("BEAM-MODEL-ID:") != 0) {
    model_id = getKeyValue(strLine); model_id_found = true;
    wfit->ID = model_id.toInt(&ok,10);
   }
   if (strLine.contains("BEGIN-PARAMETERS") != 0) {
    begin_para_found = true;
   }
   if (strLine.contains("PRIMARY-PHOTONS:") != 0) {
    p_pri = getKeyValue(strLine); p_pri_found = true;
    wfit->p0 = p_pri.toFloat(&ok);
   }
   if (strLine.contains("PRIMARY-DIST:") != 0) {
    pri_distance = getKeyValue(strLine); pri_distance_found = true;
    wfit->Z0 = pri_distance.toFloat(&ok);
   }
   if (strLine.contains("PRIMARY-SIGMA:") != 0) {
    sigma_pri = getKeyValue(strLine); sigma_pri_found = true;
    wfit->s0 = sigma_pri.toFloat(&ok);
   }
   if (strLine.contains("SCATTER-DIST:") != 0) {
    sct_distance = getKeyValue(strLine); sct_distance_found = true;
    wfit->ZS = sct_distance.toFloat(&ok);
   }
   if (strLine.contains("SCATTER-SIGMA:") != 0) {
    sigma_sct = getKeyValue(strLine); sigma_sct_found = true;
    wfit->ss = sigma_sct.toFloat(&ok);
   }
   if (strLine.contains("PRIMARY-HORN0:") != 0) {
    horn_0 = getKeyValue(strLine); horn_0_found = true;
    wfit->h0 = horn_0.toFloat(&ok);
   }
   if (strLine.contains("PRIMARY-HORN1:") != 0) {
    horn_1 = getKeyValue(strLine); horn_1_found = true;
    wfit->h1 = horn_1.toFloat(&ok);
   }
   if (strLine.contains("PRIMARY-HORN2:") != 0) {
    horn_2 = getKeyValue(strLine); horn_2_found = true;
    wfit->h2 = horn_2.toFloat(&ok);
   }
   if (strLine.contains("PRIMARY-HORN3:") != 0) {
    horn_3 = getKeyValue(strLine); horn_3_found = true;
    wfit->h3 = horn_3.toFloat(&ok);
   }
   if (strLine.contains("PRIMARY-HORN4:") != 0) {
    horn_4 = getKeyValue(strLine); horn_4_found = true;
    wfit->h4 = horn_4.toFloat(&ok);
   }
   if (strLine.contains("COLM-DIST:") != 0) {
    col_mdistance = getKeyValue(strLine); col_mdistance_found = true;
    wfit->ZM = col_mdistance.toFloat(&ok);
   }
   if (strLine.contains("COLC-DIST:") != 0) {
    col_cdistance = getKeyValue(strLine); col_cdistance_found = true;
    wfit->ZC = col_cdistance.toFloat(&ok);
   }
   if (strLine.contains("COLX-DIST:") != 0) {
    col_xdistance = getKeyValue(strLine); col_xdistance_found = true;
    wfit->ZX = col_xdistance.toFloat(&ok);
   }
   if (strLine.contains("COLY-DIST:") != 0) {
    col_ydistance = getKeyValue(strLine); col_ydistance_found = true;
    wfit->ZY = col_ydistance.toFloat(&ok);
   }
   if (strLine.contains("NORM-VALUE:") != 0) {
    norm_value = getKeyValue(strLine); norm_value_found = true;
    wfit->normValue = norm_value.toFloat(&ok);
   }
   if (strLine.contains("GY/MU-DMAX:") != 0) {
    gray_mu_dmax = getKeyValue(strLine); gray_mu_dmax_found = true;
    wfit->gy_mu_dmax = gray_mu_dmax.toFloat(&ok);
   }
   if (strLine.contains("ENERGY-MAX:") != 0) {
    energy_max = getKeyValue(strLine); energy_max_found = true;
    wfit->Emax = energy_max.toFloat(&ok);
   }
   if (strLine.contains("ENERGY-MIN:") != 0) {
    energy_min = getKeyValue(strLine); energy_min_found = true;
    wfit->Emin = energy_min.toFloat(&ok);
   }
   if (strLine.contains("L-VALUE:") != 0) {
    l_value = getKeyValue(strLine); l_value_found = true;
    wfit->lval = l_value.toFloat(&ok);
   }
   if (strLine.contains("B-VALUE:") != 0) {
    b_value = getKeyValue(strLine); b_value_found = true;
    wfit->bval = b_value.toFloat(&ok);
   }
   if (strLine.contains("A-VALUE:") != 0) {
    a_value = getKeyValue(strLine); a_value_found = true;
    wfit->aval = a_value.toFloat(&ok);
   }
   if (strLine.contains("Z-VALUE:") != 0) {
    z_value = getKeyValue(strLine); z_value_found = true;
    wfit->zval = z_value.toFloat(&ok);
   }
   if (strLine.contains("CHARGED-PARTICLES:") != 0) {
    p_con = getKeyValue(strLine); p_con_found = true;
    wfit->pcon = p_con.toFloat(&ok);
   }
   if (strLine.contains("CHARGED-DIST:") != 0) {
    distance_con = getKeyValue(strLine); distance_con_found = true;
   }
   if (strLine.contains("CHARGED-RADIUS:") != 0) {
    radius_con = getKeyValue(strLine); radius_con_found = true;
    wfit->FFRad = radius_con.toFloat(&ok);
   }
   if (strLine.contains("CHARGED-E-MEAN:") != 0) {
    e_mean_con = getKeyValue(strLine); e_mean_con_found = true;
    wfit->eEnergy = e_mean_con.toFloat(&ok);
   }
   if (strLine.contains("NU-VALUE:") != 0) {
    nu_value = getKeyValue(strLine); nu_value_found = true;
    wfit->nu = nu_value.toFloat(&ok);
   }
   if (strLine.contains("END-PARAMETERS") != 0) {
    end_para_found = true;
   }
  }
 }
 iFile.close();
}


bool WaterFit::makeDir(QString dirStr) {
 bool dirExists = false;
 QString lastDir = dirStr.section('/',-1);
 QString dirPath = dirStr.section('/',0,-2);
 QDir *afitDir = new QDir;
 afitDir->setPath(dirStr);
 if(afitDir->exists()) dirExists = true;
 else {
    afitDir->setPath(dirPath);
    afitDir->mkdir(lastDir);
    dirExists = true;
 }
 return (dirExists);
}


void WaterFit::toggleFS() {
  bool ok;
  QString FS1 = ui->comboBoxFS->currentText();
  wfit->FS1 = FS1.toInt(&ok,10)/10;
}

void WaterFit::removeSpect() {
 QString XVMC_HOME = getenv("XVMC_HOME");
 QString XVMC_HOME_LIB = XVMC_HOME.section("bin",0,0)+"/lib";
 QString refSpect = "reference_spectrum.dat";
 QString refSpectFile = "";
 QFile refFile(refSpect);
 if (refFile.exists()) {
     QString CMD = "rm " + refSpect;
     if (system(CMD.toLatin1()) != 0) {
       QTextStream (stdout) << "ERROR: Somethings are wrong: " << CMD << endl ;
     }
 }
}

void WaterFit::setLineEditE()
{
    QString usrText;
    QTextStream(&usrText) << wfit->E;
    ui->lineEditE->setText(usrText);
}

void WaterFit::setLineEditEp()
{
    QString usrText;
  if (wfit->bval != 0.0) {
     // QTextStream(&usrText) << (wfit->lval / wfit->bval);
     QTextStream(&usrText) << (1.0/wfit->lval*log((wfit->lval+wfit->bval)/wfit->bval));
  } else {
  QTextStream(&usrText) << "0.0";
  }
    ui->lineEditEp->setText(usrText);
}

void WaterFit::setLineEditEmean()
{
    QString usrText;
  if (wfit->bval != 0.0) {
     // QTextStream(&usrText) << (wfit->lval+1)/wfit->bval;
     QTextStream(&usrText) << (3.0/wfit->lval*log((wfit->lval+wfit->bval)/wfit->bval));
  } else {
  QTextStream(&usrText) << "0.0";
  }
    ui->lineEditEmean->setText(usrText);
}

// ---------------------------------------------
void WaterFit::setFloatSpinBoxNorm(float norm) {
     ui->floatSpinBoxNorm->setValue(norm);
}
void WaterFit::setFloatSpinBoxLval(float lval) {
     ui->floatSpinBoxLval->setValue(lval);
}
void WaterFit::setFloatSpinBoxBval(float bval) {
     ui->floatSpinBoxBval->setValue(bval);
}
void WaterFit::setFloatSpinBoxAval(float aval) {
     // ui->floatSpinBoxAval is preplaced with lineEditAval
     // ui->floatSpinBoxAval->setValue(aval); // REMOVED

     // QString AVAL=""; AVAL.sprintf("%6.2f",aval);
     // Get A-Value from Machine Information File instead of current BDT
     QString AVAL = readSettingString("AVALUE");
     // cout << "AVAL= " << AVAL << " aval = " << aval << endl;
     ui->lineEditAval->setText(AVAL);
}
void WaterFit::setFloatSpinBoxEmin(float Emin) {
     ui->floatSpinBoxEmin->setValue(Emin);
}
void WaterFit::setFloatSpinBoxEmax(float Emax) {
     ui->floatSpinBoxEmax->setValue(Emax);
}
void WaterFit::setFloatSpinBoxPcon(float pcon) {
     ui->floatSpinBoxPcon->setValue(pcon);
}

void WaterFit::setCheckBoxNorm(int inorm) {
    if(inorm == 1) ui->checkBoxNorm->setChecked(false);
    if(inorm == 0) ui->checkBoxNorm->setChecked(true);
}
void WaterFit::setCheckBoxLval(int ilval) {
    if(ilval == 1) ui->checkBoxLval->setChecked(false);
    if(ilval == 0) ui->checkBoxLval->setChecked(true);
}
void WaterFit::setCheckBoxBval(int ibval) {
    if(ibval == 1) ui->checkBoxBval->setChecked(false);
    if(ibval == 0) ui->checkBoxBval->setChecked(true);
}
void WaterFit::setCheckBoxAval(int iaval) {
  iaval = 1; // To remove Warning in compilation
    // if(iaval == 1) ui->checkBoxAval->setChecked(false); // REMOVED
    // if(iaval == 0) ui->checkBoxAval->setChecked(true);  // REMOVED
}
void WaterFit::setCheckBoxEmin(int iEmin) {
    if(iEmin == 1) ui->checkBoxEmin->setChecked(false);
    if(iEmin == 0) ui->checkBoxEmin->setChecked(true);
}
void WaterFit::setCheckBoxEmax(int iEmax) {
    if(iEmax == 1) ui->checkBoxEmax->setChecked(false);
    if(iEmax == 0) ui->checkBoxEmax->setChecked(true);
}
void WaterFit::setCheckBoxPcon(int ipcon) {
    if(ipcon == 1) ui->checkBoxPcon->setChecked(false);
    if(ipcon == 0) ui->checkBoxPcon->setChecked(true);
}
// ---------------------------------------------
void WaterFit::getFloatSpinBoxNorm() {
 bool ok;
 wfit->norm = ui->floatSpinBoxNorm->text().toFloat(&ok);
 if(ui->checkBoxNorm->isChecked()) wfit->inorm = 0;
 else wfit->inorm = 1;
}
void WaterFit::getFloatSpinBoxLval() {
 bool ok;
 wfit->lval = ui->floatSpinBoxLval->text().toFloat(&ok);
 if(ui->checkBoxLval->isChecked()) wfit->ilval = 0;
 else wfit->ilval = 1;
}
void WaterFit::getFloatSpinBoxBval() {
 bool ok;
 wfit->bval = ui->floatSpinBoxBval->text().toFloat(&ok);
 if(ui->checkBoxBval->isChecked()) wfit->ibval = 0;
 else wfit->ibval = 1;
}
void WaterFit::getFloatSpinBoxAval() {
 bool ok;
 // floatSpinBoxAval is replaced with lineEditAval
 // wfit->aval = ui->floatSpinBoxAval->text().toFloat(&ok); // REMOVED
 wfit->aval = ui->lineEditAval->text().toFloat(&ok);
 // if(ui->checkBoxAval->isChecked()) wfit->iaval = 0;
 // else wfit->iaval = 1;
 wfit->iaval = 0;
}
void WaterFit::getFloatSpinBoxEmin() {
 bool ok;
 wfit->Emin = ui->floatSpinBoxEmin->text().toFloat(&ok);
 if(ui->checkBoxEmin->isChecked()) wfit->iEmin = 0;
 else wfit->iEmin = 1;
}
void WaterFit::getFloatSpinBoxEmax() {
 bool ok;
 wfit->Emax = ui->floatSpinBoxEmax->text().toFloat(&ok);
 if(ui->checkBoxEmax->isChecked()) wfit->iEmax = 0;
 else wfit->iEmax = 1;
}
void WaterFit::getFloatSpinBoxPcon() {
 bool ok;
 wfit->pcon = ui->floatSpinBoxPcon->text().toFloat(&ok);
 if(ui->checkBoxPcon->isChecked()) wfit->ipcon = 0;
 else wfit->ipcon = 1;
}

// --------------------------------------------------------------------------------
void WaterFit::setWaterFitValues(char *fname){
 readWaterFit(fname);
 setLineEditE();
 setLineEditEp();
 setLineEditEmean();

 float norm = wfit->norm;    int inorm = wfit->inorm;
 float lval = wfit->lval;    int ilval = wfit->ilval;
 float bval = wfit->bval;    int ibval = wfit->ibval;
 float aval = wfit->aval;    int iaval = wfit->iaval;
 float Emin = wfit->Emin;    int iEmin = wfit->iEmin;
 float Emax = wfit->Emax;    int iEmax = wfit->iEmax;
 float pcon = wfit->pcon;    int ipcon = wfit->ipcon;

 setFloatSpinBoxNorm(norm);  setCheckBoxNorm(inorm);
 setFloatSpinBoxLval(lval);  setCheckBoxLval(ilval);
 setFloatSpinBoxBval(bval);  setCheckBoxBval(ibval);
 setFloatSpinBoxAval(aval);  setCheckBoxAval(iaval);
 setFloatSpinBoxEmin(Emin);  setCheckBoxEmin(iEmin);
 setFloatSpinBoxEmax(Emax);  setCheckBoxEmax(iEmax);
 setFloatSpinBoxPcon(pcon);  setCheckBoxPcon(ipcon);

 wfit->norm = norm; wfit->inorm = inorm;
 wfit->lval = lval; wfit->ilval = ilval;
 wfit->bval = bval; wfit->ibval = ibval;
 wfit->aval = aval; wfit->iaval = iaval;
 wfit->Emin = Emin; wfit->iEmin = iEmin;
 wfit->Emax = Emax; wfit->iEmax = iEmax;
 wfit->pcon = pcon; wfit->ipcon = ipcon;
}

void WaterFit::newWaterFitValues(){
 setLineEditE();
 setLineEditEp();
 setLineEditEmean();

 float norm = wfit->norm;    int inorm = wfit->inorm;
 float lval = wfit->lval;    int ilval = wfit->ilval;
 float bval = wfit->bval;    int ibval = wfit->ibval;
 float aval = wfit->aval;    int iaval = wfit->iaval;
 float Emin = wfit->Emin;    int iEmin = wfit->iEmin;
 float Emax = wfit->Emax;    int iEmax = wfit->iEmax;
 float pcon = wfit->pcon;    int ipcon = wfit->ipcon;

 setFloatSpinBoxNorm(norm);  setCheckBoxNorm(inorm);
 setFloatSpinBoxLval(lval);  setCheckBoxLval(ilval);
 setFloatSpinBoxBval(bval);  setCheckBoxBval(ibval);
 setFloatSpinBoxAval(aval);  setCheckBoxAval(iaval);
 setFloatSpinBoxEmin(Emin);  setCheckBoxEmin(iEmin);
 setFloatSpinBoxEmax(Emax);  setCheckBoxEmax(iEmax);
 setFloatSpinBoxPcon(pcon);  setCheckBoxPcon(ipcon);

 wfit->norm = norm; wfit->inorm = inorm;
 wfit->lval = lval; wfit->ilval = ilval;
 wfit->bval = bval; wfit->ibval = ibval;
 wfit->aval = aval; wfit->iaval = iaval;
 wfit->Emin = Emin; wfit->iEmin = iEmin;
 wfit->Emax = Emax; wfit->iEmax = iEmax;
 wfit->pcon = pcon; wfit->ipcon = ipcon;
}

void WaterFit::updateWaterFitValues(){ // Update Values from Temp File
 QFile wFile("wfit.inp.tmp");
 if (wFile.exists()) {
  setWaterFitValues("wfit.inp.tmp");
  // setWaterFitValues("wfit.inp.tmp"); // to remove unknow bug that does not update bval
 }
  QFile ddFile("results.diff.tmp");
  if (ddFile.exists()) ui->qwtPlot->usrDataFile("results.diff.tmp");
}

void WaterFit::resetWaterFitValues(){
 QFile wFile("wfit.inp");
 if (wFile.exists()) {
  setWaterFitValues("wfit.inp");
  // setWaterFitValues("wfit.inp"); // to remove unknow bug that does not update bval
 }
  QFile ddFile("results.diff");
  if (ddFile.exists()) ui->qwtPlot->usrDataFile("results.diff");
 writeSpect();
}

void WaterFit::calcEpEmean(){
  getFloatSpinBoxBval();
  getFloatSpinBoxLval();
  getFloatSpinBoxEmin();
  getFloatSpinBoxEmax();

  float dE = 0.001;
  float peak = 0.0;
  float Ep = 0.0;
  float norm = 0.0;
  float sumSpect = 0.5 * (exp(-wfit->lval*wfit->Emin)-exp(-wfit->lval*wfit->Emin))
                    *(exp(-wfit->bval*wfit->Emin) - exp(-wfit->bval*wfit->Emax));

  float y = 0.0;
  for (float x = wfit->Emin+dE; x < wfit->Emax; x += dE) {
#ifdef CMS_SPECT
    y = (exp(-wfit->lval*wfit->Emin)-exp(-wfit->lval*x))*(exp(-wfit->bval*x) - exp(-wfit->bval*wfit->Emax));
#elif XVMC
    // if (wfit->bval != 0.0) y = wfit->lval/wfit->bval;
    y = pow(x,wfit->lval)*exp(-wfit->bval*x);
#else
    y = (1.0-exp(-wfit->lval*x))*(exp(-wfit->bval*x) - exp(-wfit->bval*wfit->Emax));
#endif
    if (y >= peak) {
       peak = y;
       Ep = x;
    }
    norm += y*dE;
    sumSpect += x*y*dE;
  }
  QString usrText;
  usrText.sprintf("%6.3f",Ep);
  ui->lineEditEp->setText(usrText);
  // int nE = (int)((wfit->Emax - wfit->Emin)/dE);
  // cout << "norm = " << norm << " sumSpect = " << sumSpect << endl;
  float Emean = sumSpect/norm;
#ifdef XVMC
  Emean = 0.0;
  if (wfit->bval != 0.0) Emean = (wfit->lval + 1.0)/wfit->bval;
#endif
  usrText.sprintf("%6.3f",Emean);
  ui->lineEditEmean->setText(usrText);
  // setLineEditEmean();
  // setLineEditEp();
  writeSpect();
}

// ------------------------------------------------------------------------------
void WaterFit::checkBoxClicked(bool checked, QDoubleSpinBox *SP, QSlider *SL) {
    SP->setDisabled(checked);
    SL->setDisabled(checked);
    SL->setHidden(checked);
}

void WaterFit::spinBoxValueChanged(double arg1, QDoubleSpinBox *SP, QSlider *SL) {
    int value = (int)(SL->minimum()
                      + (arg1 - SP->minimum())/(SP->maximum() - SP->minimum())
                      * (SL->maximum() - SL->minimum()));
    SL->setValue(value);
}


void WaterFit::sliderValueChanged(int value, QDoubleSpinBox *SP, QSlider *SL) {
    double arg1 = (SP->minimum()+ ((double)value - SL->minimum())
                   /(SL->maximum() - SL->minimum())
                   *(SP->maximum() - SP->minimum()));
    SP->setValue(arg1);
}

// ---Norm -----------------------------------------------------------------------

void WaterFit::on_checkBoxNorm_clicked(bool checked)
{
    checkBoxClicked(checked, ui->floatSpinBoxNorm, ui->sliderNorm);
}

void WaterFit::on_floatSpinBoxNorm_valueChanged(double arg1)
{
    spinBoxValueChanged(arg1, ui->floatSpinBoxNorm, ui->sliderNorm);
}

void WaterFit::on_sliderNorm_valueChanged(int value)
{
    sliderValueChanged(value, ui->floatSpinBoxNorm, ui->sliderNorm);
}

// ---Lval -----------------------------------------------------------------------
void WaterFit::on_checkBoxLval_clicked(bool checked)
{
    checkBoxClicked(checked, ui->floatSpinBoxLval, ui->sliderLval);
}

void WaterFit::on_floatSpinBoxLval_valueChanged(double arg1)
{
    spinBoxValueChanged(arg1, ui->floatSpinBoxLval, ui->sliderLval);
    calcEpEmean();
}

void WaterFit::on_sliderLval_valueChanged(int value)
{
    sliderValueChanged(value, ui->floatSpinBoxLval, ui->sliderLval);
    calcEpEmean();
}


// ---Bval -----------------------------------------------------------------------
void WaterFit::on_checkBoxBval_clicked(bool checked)
{
    checkBoxClicked(checked, ui->floatSpinBoxBval, ui->sliderBval);
}

void WaterFit::on_floatSpinBoxBval_valueChanged(double arg1)
{
    spinBoxValueChanged(arg1, ui->floatSpinBoxBval, ui->sliderBval);
    calcEpEmean();
}

void WaterFit::on_sliderBval_valueChanged(int value)
{
    sliderValueChanged(value, ui->floatSpinBoxBval, ui->sliderBval);
    calcEpEmean();
}


// ---Pcon -----------------------------------------------------------------------
void WaterFit::on_checkBoxPcon_clicked(bool checked)
{
    checkBoxClicked(checked, ui->floatSpinBoxPcon, ui->sliderPcon);
}

void WaterFit::on_floatSpinBoxPcon_valueChanged(double arg1)
{
    spinBoxValueChanged(arg1, ui->floatSpinBoxPcon, ui->sliderPcon);
}

void WaterFit::on_sliderPcon_valueChanged(int value)
{
    sliderValueChanged(value, ui->floatSpinBoxPcon, ui->sliderPcon);
}


// ---Emax -----------------------------------------------------------------------
void WaterFit::on_checkBoxEmax_clicked(bool checked)
{
    checkBoxClicked(checked, ui->floatSpinBoxEmax, ui->sliderEmax);
}

void WaterFit::on_floatSpinBoxEmax_valueChanged(double arg1)
{
    spinBoxValueChanged(arg1, ui->floatSpinBoxEmax, ui->sliderEmax);
    calcEpEmean();
}

void WaterFit::on_sliderEmax_valueChanged(int value)
{
    sliderValueChanged(value, ui->floatSpinBoxEmax, ui->sliderEmax);
}

// ---Emin -----------------------------------------------------------------------
void WaterFit::on_checkBoxEmin_clicked(bool checked)
{
    checkBoxClicked(checked, ui->floatSpinBoxEmin, ui->sliderEmin);
}

void WaterFit::on_floatSpinBoxEmin_valueChanged(double arg1)
{
    spinBoxValueChanged(arg1, ui->floatSpinBoxEmin, ui->sliderEmin);
}

void WaterFit::on_sliderEmin_valueChanged(int value)
{
    sliderValueChanged(value, ui->floatSpinBoxEmin, ui->sliderEmin);
}

void WaterFit::on_resetButton_clicked()
{
    resetWaterFitValues();
}

void WaterFit::on_fitButton_clicked()
{
    runWaterFit();
}

void WaterFit::on_interruptButton_clicked()
{
    Interrupt();
}

void WaterFit::on_doneButton_clicked()
{
    Done();
}

void WaterFit::on_guessButton_clicked()
{
    guessSpect();
}

void WaterFit::on_spectButton_clicked()
{
    toggleSpectButton();
}

void WaterFit::on_removeButton_clicked()
{
    removeSpect();
}

void WaterFit::on_comboBoxFS_highlighted(const QString &arg1)
{
    toggleFS();
}

void WaterFit::on_checkBoxFS2_toggled(bool checked)
{
    updateFS();
}
