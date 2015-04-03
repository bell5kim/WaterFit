#ifndef WATERFIT_H
#define WATERFIT_H

#include <QMainWindow>
#include <QSpinBox>
#include <QSlider>

namespace Ui {
class WaterFit;
}

class QwtPlotZoomer;
class QwtPlotPicker;
class QwtPlotPanner;
class Plot;
class QPolygon;

class WaterFit : public QMainWindow
{
    Q_OBJECT

public:
    explicit WaterFit(QWidget *parent = 0);
    ~WaterFit();

    virtual void initApp();

    void resetZoomer();

public slots:

    virtual void updateFS();
    virtual void setLineEditE();
    virtual void setLineEditEp();
    virtual void setLineEditEmean();
    virtual void setFloatSpinBoxNorm(float norm);
    virtual void setFloatSpinBoxLval(float lval);
    virtual void setFloatSpinBoxBval(float bval);
    virtual void setFloatSpinBoxAval(float aval);
    virtual void setFloatSpinBoxEmin(float Emin);
    virtual void setFloatSpinBoxEmax(float Emax);
    virtual void setFloatSpinBoxPcon(float pcon);
    virtual void setCheckBoxNorm(int inorm);
    virtual void setCheckBoxLval(int ilval);
    virtual void setCheckBoxBval(int ibval);
    virtual void setCheckBoxAval(int iaval);
    virtual void setCheckBoxEmin(int iEmin);
    virtual void setCheckBoxEmax(int iEmax);
    virtual void setCheckBoxPcon(int ipcon);
    virtual void getFloatSpinBoxNorm();
    virtual void getFloatSpinBoxLval();
    virtual void getFloatSpinBoxBval();
    virtual void getFloatSpinBoxAval();
    virtual void getFloatSpinBoxEmin();
    virtual void getFloatSpinBoxEmax();
    virtual void getFloatSpinBoxPcon();
    virtual void setWaterFitValues(char *fname);
    virtual void newWaterFitValues();
    virtual void updateWaterFitValues();
    virtual void resetWaterFitValues();
    virtual void calcEpEmean();
    virtual void initWaterFit();
    virtual void readWaterFit(char *fname);
    virtual void writeWaterFit(char *fname);
    virtual void runWaterFit();
    virtual void readOut(char *fname);
    virtual void guessSpect();
    virtual void Interrupt();
    virtual void Done();
    virtual void plotWaterFit();
    virtual QString readSettingString(QString keyWord);
    virtual void writeSettingString(QString keyWord, QString keyValue);
    virtual void writeSpect();
    virtual void toggleSpectButton();
    virtual void writeBDT(QString bdtFile);
    virtual QString getKeyValue(QString strLine);
    virtual void readBDT(QString fName);
    virtual bool makeDir(QString dirStr);
    virtual void toggleFS();
    virtual void removeSpect();

private slots:
    void moved(const QPoint &);
    void selected( const QPolygon & );
    void runGammaTable();

#ifndef QT_NO_PRINTER
    void print();
#endif

    void exportDocument();
    void enableZoomMode( bool );

    void checkBoxClicked(bool checked, QDoubleSpinBox *SP, QSlider *SL);
    void spinBoxValueChanged(double arg1, QDoubleSpinBox *SP, QSlider *SL);
    void sliderValueChanged(int value, QDoubleSpinBox *SP, QSlider *SL);

    void on_checkBoxNorm_clicked(bool checked);
    void on_floatSpinBoxNorm_valueChanged(double arg1);
    void on_sliderNorm_valueChanged(int value);

    void on_checkBoxLval_clicked(bool checked);
    void on_floatSpinBoxLval_valueChanged(double arg1);
    void on_sliderLval_valueChanged(int value);

    void on_checkBoxBval_clicked(bool checked);
    void on_floatSpinBoxBval_valueChanged(double arg1);
    void on_sliderBval_valueChanged(int value);

    void on_checkBoxPcon_clicked(bool checked);
    void on_floatSpinBoxPcon_valueChanged(double arg1);
    void on_sliderPcon_valueChanged(int value);

    void on_checkBoxEmax_clicked(bool checked);
    void on_floatSpinBoxEmax_valueChanged(double arg1);
    void on_sliderEmax_valueChanged(int value);

    void on_checkBoxEmin_clicked(bool checked);
    void on_floatSpinBoxEmin_valueChanged(double arg1);
    void on_sliderEmin_valueChanged(int value);

    void on_resetButton_clicked();

    void on_fitButton_clicked();

    void on_interruptButton_clicked();

    void on_doneButton_clicked();

    void on_guessButton_clicked();

    void on_spectButton_clicked();

    void on_removeButton_clicked();

    void on_comboBoxFS_highlighted(const QString &arg1);

    void on_checkBoxFS2_toggled(bool checked);

private:
    void showInfo( QString text = QString::null );

    Ui::WaterFit *ui;

    QwtPlotZoomer *d_zoomer[2];
    QwtPlotPicker *d_picker;
    QwtPlotPanner *d_panner;
};

#endif // WATERFIT_H
