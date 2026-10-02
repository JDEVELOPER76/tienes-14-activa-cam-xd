#include "Ventana.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QSizePolicy>


Ventana::Ventana()
{
    // ======================================
    // VENTANA
    // ======================================

    setWindowTitle(
        "Cámara"
    );

    resize(
        700,
        600
    );


    // ======================================
    // PANTALLA
    // ======================================

    pantalla = new QLabel();

    pantalla->setMinimumSize(
        640,
        480
    );

    pantalla->setStyleSheet(
        "background-color: black;"
    );

    pantalla->setAlignment(
        Qt::AlignCenter
    );


    // ======================================
    // TEXTO
    // ======================================

    texto = new QLineEdit();

    texto->setPlaceholderText(
        "Texto sobre la cámara..."
    );


    // ======================================
    // ANCHO
    // ======================================

    ancho = new QSpinBox();

    ancho->setRange(
        160,
        1920
    );

    ancho->setValue(
        640
    );


    // ======================================
    // ALTO
    // ======================================

    alto = new QSpinBox();

    alto->setRange(
        120,
        1080
    );

    alto->setValue(
        480
    );


    // ======================================
    // BOTÓN APLICAR
    // ======================================

    botonAplicar = new QPushButton(
        "Aplicar"
    );


    // ======================================
    // LAYOUT PRINCIPAL
    // ======================================

    QVBoxLayout* principal =
        new QVBoxLayout(this);


    principal->addWidget(
        pantalla
    );


    // ======================================
    // TAMAÑO
    // ======================================

    QHBoxLayout* tamanio =
        new QHBoxLayout();


    tamanio->addWidget(
        new QLabel("Ancho:")
    );

    tamanio->addWidget(
        ancho
    );


    tamanio->addWidget(
        new QLabel("Alto:")
    );

    tamanio->addWidget(
        alto
    );


    principal->addLayout(
        tamanio
    );


    // ======================================
    // TEXTO
    // ======================================

    principal->addWidget(
        texto
    );


    // ======================================
    // APLICAR
    // ======================================

    principal->addWidget(
        botonAplicar
    );


    // ======================================
    // ABRIR CÁMARA
    // ======================================

    if (!camara.abrir(0))
    {
        pantalla->setText(
            "No se pudo abrir la cámara"
        );
    }


    // ======================================
    // TIMER
    // ======================================

    timer = new QTimer(this);


    connect(
        timer,
        &QTimer::timeout,

        this,

        [this]()
        {
            actualizarCamara();
        }
    );


    timer->start(
        33
    );


    // ======================================
    // APLICAR CONFIGURACIÓN
    // ======================================

    connect(
        botonAplicar,

        &QPushButton::clicked,

        this,

        [this]()
        {
            camara.establecerResolucionCaptura(
                ancho->value(),
                alto->value()
            );


            camara.establecerTexto(
                texto->text()
            );
        }
    );
}


// ==========================================
// ACTUALIZAR CÁMARA
// ==========================================

void Ventana::actualizarCamara()
{
    QImage imagen =
        camara.capturar();


    if (imagen.isNull())
        return;


    pantalla->setPixmap(

        QPixmap::fromImage(
            imagen
        ).scaled(

            pantalla->size(),

            Qt::KeepAspectRatio,

            Qt::SmoothTransformation
        )
    );
}
