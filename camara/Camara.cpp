#include "Camara.h"

#include <iostream>
#include <vector>

#include <opencv2/imgproc.hpp>

#include <QDebug>

/*
 * ============================================================
 * Camara
 * ============================================================
 */

Camara::Camara()
{
}

Camara::~Camara()
{
    cerrar();
}

bool Camara::abrir(int dispositivo)
{
    qDebug() << "[Camara] Abriendo dispositivo" << dispositivo;

    cap.open(dispositivo, cv::CAP_V4L2);

    if (!cap.isOpened())
    {
        qDebug() << "[Camara] ERROR: no se pudo abrir la cámara";
        return false;
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH,  anchoCaptura);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, altoCaptura);

    qDebug() << "[Camara] Cámara abierta correctamente";
    return true;
}

void Camara::cerrar()
{
    if (cap.isOpened())
    {
        cap.release();
    }
}

bool Camara::estaAbierta() const
{
    return cap.isOpened();
}

void Camara::establecerResolucionCaptura(int nuevoAncho, int nuevoAlto)
{
    qDebug() << "[Camara] establecerResolucionCaptura:" 
             << nuevoAncho << "x" << nuevoAlto;

    anchoCaptura = nuevoAncho;
    altoCaptura  = nuevoAlto;

    if (cap.isOpened())
    {
        cap.set(cv::CAP_PROP_FRAME_WIDTH,  anchoCaptura);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, altoCaptura);
    }
}

int Camara::anchoCapturaActual() const
{
    return anchoCaptura;
}

int Camara::altoCapturaActual() const
{
    return altoCaptura;
}

void Camara::establecerTexto(const QString& nuevoTexto)
{
    texto = nuevoTexto;
}

void Camara::establecerTamanoTexto(int tamano)
{
    if (tamano < 1)
        tamano = 1;

    tamanoTexto = tamano;
}

void Camara::establecerColorTexto(const QColor& color)
{
    colorTexto = color;
}

void Camara::activarEspejo(bool activo)
{
    espejo = activo;
}

QImage Camara::capturar(int anchoDestino, int altoDestino)
{
    if (!cap.isOpened())
    {
        return QImage();
    }

    cv::Mat frame;

    if (!cap.read(frame))
    {
        return QImage();
    }

    if (espejo)
    {
        cv::flip(frame, frame, 1);
    }

    /*
     * TEXTO
     */

    if (!texto.isEmpty())
    {
        double escala = 0.5 + (tamanoTexto * 0.2);
        int grosor = 1 + (tamanoTexto / 2);

        cv::Scalar color(
            colorTexto.blue(),
            colorTexto.green(),
            colorTexto.red()
        );

        cv::putText(
            frame,
            texto.toStdString(),
            cv::Point(10, frame.rows - 20),
            cv::FONT_HERSHEY_SIMPLEX,
            escala,
            color,
            grosor
        );
    }

    /*
     * BGR -> RGB
     */

    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

    QImage imagen(
        frame.data,
        frame.cols,
        frame.rows,
        static_cast<int>(frame.step),
        QImage::Format_RGB888
    );

    imagen = imagen.copy();

    /*
     * RESPONSIVO
     */

    if (anchoDestino > 0 && altoDestino > 0)
    {
        imagen = imagen.scaled(
            anchoDestino,
            altoDestino,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        );
    }

    return imagen;
}
