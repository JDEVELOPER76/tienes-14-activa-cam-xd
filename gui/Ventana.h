#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QTimer>

#include "../camara/Camara.h"


class Ventana : public QWidget
{
public:

    Ventana();

private:

    void actualizarCamara();


    Camara camara;


    QLabel* pantalla;

    QLineEdit* texto;

    QSpinBox* ancho;

    QSpinBox* alto;


    QPushButton* botonAplicar;


    QTimer* timer;
};
