#include "Configuracion.h"

#include <QDir>
#include <QFile>
#include <QTextStream>

Configuracion::Configuracion()
{
}

QString Configuracion::carpetaDatos() const
{
    /*
     * La carpeta "datos" se crea junto al ejecutable
     * (en el directorio de trabajo actual).
     */

    QDir dir(QDir::currentPath() + "/datos");

    if (!dir.exists())
    {
        dir.mkpath(".");
    }

    return dir.absolutePath();
}

QString Configuracion::leerTexto(
    const QString& archivo,
    const QString& porDefecto
) const
{
    QFile file(carpetaDatos() + "/" + archivo);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return porDefecto;
    }

    QTextStream in(&file);

    QString contenido = in.readAll().trimmed();

    file.close();

    if (contenido.isEmpty())
    {
        return porDefecto;
    }

    return contenido;
}

int Configuracion::leerEntero(
    const QString& archivo,
    int porDefecto
) const
{
    QString valor = leerTexto(
        archivo,
        QString::number(porDefecto)
    );

    bool ok = false;
    int resultado = valor.toInt(&ok);

    if (!ok)
    {
        return porDefecto;
    }

    return resultado;
}

bool Configuracion::leerBooleano(
    const QString& archivo,
    bool porDefecto
) const
{
    QString valor = leerTexto(
        archivo,
        porDefecto ? "1" : "0"
    );

    return valor == "1";
}

void Configuracion::escribirTexto(
    const QString& archivo,
    const QString& valor
)
{
    QFile file(carpetaDatos() + "/" + archivo);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        return;
    }

    QTextStream out(&file);

    out << valor;

    file.close();
}

/* ============================================================
 * ANCHO / ALTO VENTANA
 * ============================================================ */

int Configuracion::anchoVentana() const
{
    return leerEntero("ancho.txt", 640);
}

void Configuracion::guardarAnchoVentana(int valor)
{
    escribirTexto("ancho.txt", QString::number(valor));
}

int Configuracion::altoVentana() const
{
    return leerEntero("alto.txt", 480);
}

void Configuracion::guardarAltoVentana(int valor)
{
    escribirTexto("alto.txt", QString::number(valor));
}

/* ============================================================
 * ANCHO / ALTO CAPTURA
 * ============================================================ */

int Configuracion::anchoCaptura() const
{
    return leerEntero("ancho_captura.txt", 640);
}

void Configuracion::guardarAnchoCaptura(int valor)
{
    escribirTexto("ancho_captura.txt", QString::number(valor));
}

int Configuracion::altoCaptura() const
{
    return leerEntero("alto_captura.txt", 480);
}

void Configuracion::guardarAltoCaptura(int valor)
{
    escribirTexto("alto_captura.txt", QString::number(valor));
}

/* ============================================================
 * NOMBRE / TEXTO
 * ============================================================ */

QString Configuracion::nombre() const
{
    return leerTexto("nombre.txt", "");
}

void Configuracion::guardarNombre(const QString& valor)
{
    escribirTexto("nombre.txt", valor);
}

int Configuracion::tamanoTexto() const
{
    return leerEntero("texto_tamano.txt", 1);
}

void Configuracion::guardarTamanoTexto(int valor)
{
    escribirTexto("texto_tamano.txt", QString::number(valor));
}

QColor Configuracion::colorTexto() const
{
    QString valor = leerTexto("texto_color.txt", "#FFFFFF");

    QColor color(valor);

    if (!color.isValid())
    {
        color = QColor("#FFFFFF");
    }

    return color;
}

void Configuracion::guardarColorTexto(const QColor& color)
{
    escribirTexto("texto_color.txt", color.name());
}

/* ============================================================
 * ESPEJO
 * ============================================================ */

bool Configuracion::espejo() const
{
    return leerBooleano("espejo.txt", true);
}

void Configuracion::guardarEspejo(bool valor)
{
    escribirTexto("espejo.txt", valor ? "1" : "0");
}
