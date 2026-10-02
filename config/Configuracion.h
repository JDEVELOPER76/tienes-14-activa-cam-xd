#pragma once

#include <QString>
#include <QColor>

/*
 * Gestiona la persistencia de la configuración
 * en archivos .txt dentro de una carpeta "datos".
 *
 * Archivos:
 *   datos/ancho.txt         -> ancho de la ventana
 *   datos/alto.txt          -> alto de la ventana
 *   datos/ancho_captura.txt -> ancho de captura
 *   datos/alto_captura.txt  -> alto de captura
 *   datos/nombre.txt        -> texto mostrado
 *   datos/texto_tamano.txt  -> tamaño del texto
 *   datos/texto_color.txt   -> color del texto (RRGGBB)
 *   datos/espejo.txt        -> 0 / 1
 */

class Configuracion
{
public:

    Configuracion();

    /*
     * Ancho / alto de la ventana.
     */

    int anchoVentana() const;
    void guardarAnchoVentana(int valor);

    int altoVentana() const;
    void guardarAltoVentana(int valor);

    /*
     * Ancho / alto de captura de la cámara.
     */

    int anchoCaptura() const;
    void guardarAnchoCaptura(int valor);

    int altoCaptura() const;
    void guardarAltoCaptura(int valor);

    /*
     * Texto, tamaño y color.
     */

    QString nombre() const;
    void guardarNombre(const QString& valor);

    int tamanoTexto() const;
    void guardarTamanoTexto(int valor);

    QColor colorTexto() const;
    void guardarColorTexto(const QColor& color);

    /*
     * Espejo.
     */

    bool espejo() const;
    void guardarEspejo(bool valor);

private:

    QString carpetaDatos() const;

    QString leerTexto(
        const QString& archivo,
        const QString& porDefecto
    ) const;

    int leerEntero(
        const QString& archivo,
        int porDefecto
    ) const;

    bool leerBooleano(
        const QString& archivo,
        bool porDefecto
    ) const;

    void escribirTexto(
        const QString& archivo,
        const QString& valor
    );
};
