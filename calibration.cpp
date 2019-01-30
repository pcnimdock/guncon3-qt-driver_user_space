#include "calibration.h"

Calibration::Calibration()
{
    num_sample_points=num_reference_points=k_coefs_seted=0;

}

qint8 Calibration::append_reference_point(quint16 x, quint16 y)
{
    if(num_reference_points>=N)
    {
        return -1; //demasiados puntos de referencia
    }
    ReferencePoint[num_reference_points][0]=x;
    ReferencePoint[num_reference_points][1]=y;
    num_reference_points++;
    return 0;

}

qint8 Calibration::append_sample_point(qint16 x, qint16 y)
{
    if(num_sample_points>=N)
    {
        return -1; //demasiados puntos de referencia
    }
    SamplePoint[num_sample_points][0]=x;
    SamplePoint[num_sample_points][1]=y;
    num_sample_points++;
    if((num_sample_points==N)&&(num_reference_points==N))
    {
        Get_Calibration_Coefficient();
        k_coefs_seted=1;
    }
    return 0;

}

qint8 Calibration::reset()
{
    num_sample_points=num_reference_points=k_coefs_seted=0;
return 0;
}

qint8 Calibration::get_coefficients_cal(QList<double> *coefs)
{
 coefs->clear();
 coefs->append(KX1);
 coefs->append(KX2);
 coefs->append(KX3);
 coefs->append(KY1);
 coefs->append(KY2);
 coefs->append(KY3);
 return 0;
}

qint8 Calibration::Do_Calibration(signed short int *Px, signed short int *Py) // do calibration for point (Px, Py) using the calculated coefficients
{
    if(k_coefs_seted)
    {
    *Px=(signed short int)(KX1*(*Px)+KX2*(*Py)+KX3+0.5);
    *Py=(signed      short      int)(KY1*(*Px)+KY2*(*Py)+KY3+0.5);
    return 0;
    }
    else
    {return -1;}
}

qint8 Calibration::set_coefficients_cal(QList<double> coefs)
{
    if(coefs.size()!=6)
    {return -1;}
    KX1 = coefs.at(0);
    KX2 = coefs.at(1);
    KX3 = coefs.at(2);
    KY1 = coefs.at(3);
    KY2 = coefs.at(4);
    KY3 = coefs.at(5);
    k_coefs_seted=1;
    return 0;
}

bool Calibration::isKsetted()
{
    return(k_coefs_seted);
}

int Calibration::Get_Calibration_Coefficient()
{
    int      i;
    int      Points=N;
    double      a[3],b[3],c[3],d[3],k;
    if(Points<3)
    {
        return      0;
    }
    else
    {
        if(Points==3)
        {
            for(i=0;      i<Points;      i++)
            {
                a[i]=(double)(SamplePoint[i][0]);
                b[i]=(double)(SamplePoint[i][1]);
                c[i]=(double)(ReferencePoint[i][0]);
                d[i]=(double)(ReferencePoint[i][1]);
            }
        }
        else      if(Points>3)
        {
            for(i=0;      i<3;      i++)
            {
                a[i]=0;
                b[i]=0;
                c[i]=0;
                d[i]=0;
            }
            for(i=0;      i<Points;      i++)

            {
                a[2]=a[2]+(double)(SamplePoint[i][0]);
                b[2]=b[2]+(double)(SamplePoint[i][1]);
                c[2]=c[2]+(double)(ReferencePoint[i][0]);
                d[2]=d[2]+(double)(ReferencePoint[i][1]);
                a[0]=a[0]+(double)(SamplePoint[i][0])*(double)(SamplePoint[i][0]);
                a[1]=a[1]+(double)(SamplePoint[i][0])*(double)(SamplePoint[i][1]);
                b[0]=a[1];
                b[1]=b[1]+(double)(SamplePoint[i][1])*(double)(SamplePoint[i][1]);
                c[0]=c[0]+(double)(SamplePoint[i][0])*(double)(ReferencePoint[i][0]);
                c[1]=c[1]+(double)(SamplePoint[i][1])*(double)(ReferencePoint[i][0]);
                d[0]=d[0]+(double)(SamplePoint[i][0])*(double)(ReferencePoint[i][1]);
                d[1]=d[1]+(double)(SamplePoint[i][1])*(double)(ReferencePoint[i][1]);
            }
            a[0]=a[0]/a[2];
            a[1]=a[1]/b[2];
            b[0]=b[0]/a[2];
            b[1]=b[1]/b[2];
            c[0]=c[0]/a[2];
            c[1]=c[1]/b[2];
            d[0]=d[0]/a[2];
            d[1]=d[1]/b[2];
            a[2]=a[2]/Points;
            b[2]=b[2]/Points;
            c[2]=c[2]/Points;
            d[2]=d[2]/Points;
        }
        k=(a[0]-a[2])*(b[1]-b[2])-(a[1]-a[2])*(b[0]-b[2]);
        KX1=((c[0]-c[2])*(b[1]-b[2])-(c[1]-c[2])*(b[0]-b[2]))/k;
        KX2=((c[1]-c[2])*(a[0]-a[2])-(c[0]-c[2])*(a[1]-a[2]))/k;
        KX3=(b[0]*(a[2]*c[1]-a[1]*c[2])+b[1]*(a[0]*c[2]-a[2]*c[0])+b[2]*(a[1]*c[0]-
                a[0]*c[1]))/k;
        KY1=((d[0]-d[2])*(b[1]-b[2])-(d[1]-d[2])*(b[0]-b[2]))/k;
        KY2=((d[1]-d[2])*(a[0]-a[2])-(d[0]-d[2])*(a[1]-a[2]))/k;
        KY3=(b[0]*(a[2]*d[1]-a[1]*d[2])+b[1]*(a[0]*d[2]-a[2]*d[0])+b[2]*(a[1]*d[0]-
                a[0]*d[1]))/k;
        return      Points;
    }

}
