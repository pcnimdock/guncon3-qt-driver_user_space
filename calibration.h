#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <QObject>
#include <QVector>
#include <QList>

#define N    9                             // number of reference points for calibration algorithm

class Calibration
{
public:
    Calibration();
    qint8 append_reference_point(quint16 x,quint16 y);
    qint8 append_sample_point(qint16 x,qint16 y);
    qint8 reset();
    qint8 get_coefficients_cal(QList <double> *coefs);
    qint8 Do_Calibration(signed short int *Px, signed short int *Py); // do calibration for point (Px, Py) using the calculated coefficients
    qint8 set_coefficients_cal(QList <double> coefs);
    bool isKsetted();
private:

    quint8 num_sample_points;
    quint8 num_reference_points;
    signed short int ReferencePoint[N][2];      // ideal position of reference points
    signed short int SamplePoint[N][2];         // sampling position of reference points
    double KX1, KX2, KX3, KY1, KY2, KY3;        // coefficients for calibration algorithm
    int Get_Calibration_Coefficient();           // calculate the coefficients for calibration algorithm: KX1, KX2, KX3, KY1, KY2, KY3
    bool k_coefs_seted;
};

#endif // CALIBRATION_H
