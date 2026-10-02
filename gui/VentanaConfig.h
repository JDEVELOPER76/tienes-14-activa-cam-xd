#pragma once

#include <QWidget>

#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>

#include "../camara/Camara.h"
#include "../config/Configuracion.h"

class VentanaCamara;

class VentanaConfig : public QWidget
{
public:

    VentanaConfig(
        Camara* camara,
        VentanaCamara* ventanaCamara
    );

private:

    void seleccionarColorTexto();

    void actualizarTamanoVentana();
    void actualizarResolucionCaptura();

    void cargarConfiguracion();

    void guardarNombre(const QString& valor);

    Camara* camara;

    VentanaCamara* ventanaCamara;

    Configuracion config;

    QSpinBox* anchoVentana;
    QSpinBox* altoVentana;

    QSpinBox* anchoCaptura;
    QSpinBox* altoCaptura;

    QLineEdit* texto;
    QSpinBox* tamanoTexto;

    QPushButton* botonColorTexto;
    QColor colorTexto;

    QCheckBox* espejo;
};
