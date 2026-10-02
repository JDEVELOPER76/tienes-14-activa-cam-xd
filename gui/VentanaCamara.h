#pragma once

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QKeyEvent>
#include <QMouseEvent>

#include "../camara/Camara.h"

class VentanaCamara : public QWidget
{
public:

    explicit VentanaCamara(Camara* camara);

    void aplicarConfiguracionInicial();

protected:

    void keyPressEvent(QKeyEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;

    void mouseMoveEvent(QMouseEvent* event) override;

private:

    void actualizar();

    Camara* camara;

    QLabel* pantalla;

    QTimer* timer;

    QPoint posicionInicial;

    bool moviendo = false;
};
