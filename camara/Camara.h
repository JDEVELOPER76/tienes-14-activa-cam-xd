#pragma once

#include <memory>

#include <opencv2/opencv.hpp>

#include <QImage>
#include <QString>
#include <QColor>

class Camara
{
public:

    Camara();
    ~Camara();

    bool abrir(int dispositivo = 0);
    void cerrar();

    bool estaAbierta() const;

    /*
     * Captura un frame.
     *
     * anchoDestino / altoDestino:
     *   Si son mayores que 0, la imagen devuelta se
     *   escala a ese tamaño (manteniendo la proporción,
     *   con bandas negras si hace falta). La ventana
     *   pasa aqui su tamaño actual para que el video
     *   sea responsivo al redimensionar.
     */

    QImage capturar(int anchoDestino = 0, int altoDestino = 0);

    /*
     * Resolución de captura de la cámara.
     * Es independiente del tamaño de la ventana.
     */
    void establecerResolucionCaptura(int ancho, int alto);

    int anchoCapturaActual() const;
    int altoCapturaActual() const;

    /*
     * Texto.
     */

    void establecerTexto(const QString& texto);
    void establecerTamanoTexto(int tamano);
    void establecerColorTexto(const QColor& color);

    /*
     * Espejo.
     */

    void activarEspejo(bool activo);

private:

    cv::VideoCapture cap;

    int anchoCaptura = 640;
    int altoCaptura  = 480;

    QString texto;
    int tamanoTexto = 1;              // multiplicador
    QColor colorTexto = QColor(255, 255, 255);

    bool espejo = true;
};
