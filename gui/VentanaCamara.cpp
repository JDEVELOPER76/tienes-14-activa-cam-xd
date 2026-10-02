#include "VentanaCamara.h"

#include <QVBoxLayout>
#include <QPixmap>

VentanaCamara::VentanaCamara(Camara* camara)
    : camara(camara)
{
    setWindowTitle("Cámara");

    setWindowFlags(
        Qt::FramelessWindowHint |
        Qt::WindowStaysOnTopHint
    );

    resize(640, 480);

    /*
     * CLAVE: permitir que la ventana se encoja
     * libremente, incluso más pequeña que la imagen.
     */

    setMinimumSize(1, 1);

    pantalla = new QLabel(this);

    pantalla->setAlignment(Qt::AlignCenter);
    pantalla->setStyleSheet("background-color: black;");

    /*
     * CLAVE: Ignored para que no imponga su sizeHint
     * al layout.
     */

    pantalla->setSizePolicy(
        QSizePolicy::Ignored,
        QSizePolicy::Ignored
    );

    pantalla->setMinimumSize(1, 1);

    QVBoxLayout* layout = new QVBoxLayout(this);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addWidget(pantalla);

    timer = new QTimer(this);

    connect(
        timer,
        &QTimer::timeout,
        this,
        &VentanaCamara::actualizar
    );

    timer->start(33);
}

void VentanaCamara::aplicarConfiguracionInicial()
{
    /*
     * La configuración ya se aplica en VentanaConfig,
     * pero por claridad dejamos este método para futuras
     * extensiones.
     */
}

void VentanaCamara::actualizar()
{
    /*
     * Le pasamos el tamaño ACTUAL de la pantalla a la
     * cámara para que el frame devuelto ya venga
     * adaptado al tamaño de la ventana (responsivo).
     */

    QImage imagen = camara->capturar(
        pantalla->width(),
        pantalla->height()
    );

    if (imagen.isNull())
    {
        return;
    }

    QPixmap pixmap = QPixmap::fromImage(imagen);

    pixmap = pixmap.scaled(
        pantalla->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
    );

    pantalla->setPixmap(pixmap);
}

void VentanaCamara::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
    {
        close();
        return;
    }

    QWidget::keyPressEvent(event);
}

void VentanaCamara::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        posicionInicial =
            event->globalPosition().toPoint()
            - frameGeometry().topLeft();

        moviendo = true;
    }
}

void VentanaCamara::mouseMoveEvent(QMouseEvent* event)
{
    if (moviendo &&
        (event->buttons() & Qt::LeftButton))
    {
        move(
            event->globalPosition().toPoint()
            - posicionInicial
        );
    }
}
