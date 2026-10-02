#include "VentanaConfig.h"

#include "VentanaCamara.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QColorDialog>
#include <QDebug>

VentanaConfig::VentanaConfig(
    Camara* camara,
    VentanaCamara* ventanaCamara
)
    : camara(camara),
      ventanaCamara(ventanaCamara)
{
    setWindowTitle("Configuración");

    resize(400, 480);

    /*
     * ========================================================
     * TAMAÑO DE LA VENTANA
     * ========================================================
     */

    QLabel* etiquetaVentana =
        new QLabel("<b>Tamaño de la ventana</b>");

    QLabel* etiquetaAnchoVentana =
        new QLabel("Ancho ventana:");

    anchoVentana = new QSpinBox();
    anchoVentana->setRange(160, 1920);
    anchoVentana->setValue(640);

    QLabel* etiquetaAltoVentana =
        new QLabel("Alto ventana:");

    altoVentana = new QSpinBox();
    altoVentana->setRange(120, 1080);
    altoVentana->setValue(480);

    /*
     * ========================================================
     * RESOLUCIÓN DE CAPTURA
     * ========================================================
     */

    QLabel* etiquetaCaptura =
        new QLabel("<b>Resolución de captura</b>");

    QLabel* etiquetaAnchoCaptura =
        new QLabel("Ancho captura:");

    anchoCaptura = new QSpinBox();
    anchoCaptura->setRange(160, 1920);
    anchoCaptura->setValue(640);

    QLabel* etiquetaAltoCaptura =
        new QLabel("Alto captura:");

    altoCaptura = new QSpinBox();
    altoCaptura->setRange(120, 1080);
    altoCaptura->setValue(480);

    /*
     * ========================================================
     * TEXTO
     * ========================================================
     */

    QLabel* etiquetaTexto = new QLabel("<b>Texto</b>");

    texto = new QLineEdit();
    texto->setPlaceholderText("Texto sobre la cámara");

    QLabel* etiquetaTamanoTexto =
        new QLabel("Tamaño del texto:");

    tamanoTexto = new QSpinBox();
    tamanoTexto->setRange(1, 10);
    tamanoTexto->setValue(2);

    QLabel* etiquetaColorTexto =
        new QLabel("Color del texto:");

    botonColorTexto = new QPushButton("Elegir color...");
    colorTexto = QColor(255, 255, 255);

    /*
     * ========================================================
     * ESPEJO
     * ========================================================
     */

    espejo = new QCheckBox("Modo espejo");
    espejo->setChecked(true);

    /*
     * ========================================================
     * SEPARADORES
     * ========================================================
     */

    QFrame* linea1 = new QFrame();
    linea1->setFrameShape(QFrame::HLine);
    linea1->setFrameShadow(QFrame::Sunken);

    QFrame* linea2 = new QFrame();
    linea2->setFrameShape(QFrame::HLine);
    linea2->setFrameShadow(QFrame::Sunken);

    /*
     * ========================================================
     * LAYOUT
     * ========================================================
     */

    QVBoxLayout* layout = new QVBoxLayout(this);

    layout->addWidget(etiquetaVentana);
    layout->addWidget(etiquetaAnchoVentana);
    layout->addWidget(anchoVentana);
    layout->addWidget(etiquetaAltoVentana);
    layout->addWidget(altoVentana);

    layout->addWidget(linea1);

    layout->addWidget(etiquetaCaptura);
    layout->addWidget(etiquetaAnchoCaptura);
    layout->addWidget(anchoCaptura);
    layout->addWidget(etiquetaAltoCaptura);
    layout->addWidget(altoCaptura);

    layout->addWidget(linea2);

    layout->addWidget(etiquetaTexto);
    layout->addWidget(texto);
    layout->addWidget(etiquetaTamanoTexto);
    layout->addWidget(tamanoTexto);
    layout->addWidget(etiquetaColorTexto);
    layout->addWidget(botonColorTexto);

    layout->addWidget(espejo);

    /*
     * ========================================================
     * CARGAR CONFIGURACIÓN GUARDADA
     * ========================================================
     */

    cargarConfiguracion();

    /*
     * ========================================================
     * CONEXIONES
     * ========================================================
     */

    connect(
        anchoVentana,
        QOverload<int>::of(&QSpinBox::valueChanged),
        this,
        [this](int valor)
        {
            config.guardarAnchoVentana(valor);
            actualizarTamanoVentana();
        }
    );

    connect(
        altoVentana,
        QOverload<int>::of(&QSpinBox::valueChanged),
        this,
        [this](int valor)
        {
            config.guardarAltoVentana(valor);
            actualizarTamanoVentana();
        }
    );

    connect(
        anchoCaptura,
        QOverload<int>::of(&QSpinBox::valueChanged),
        this,
        [this](int valor)
        {
            config.guardarAnchoCaptura(valor);
            actualizarResolucionCaptura();
        }
    );

    connect(
        altoCaptura,
        QOverload<int>::of(&QSpinBox::valueChanged),
        this,
        [this](int valor)
        {
            config.guardarAltoCaptura(valor);
            actualizarResolucionCaptura();
        }
    );

    connect(
        texto,
        &QLineEdit::textChanged,
        this,
        [this](const QString& valor)
        {
            this->camara->establecerTexto(valor);
            guardarNombre(valor);
        }
    );

    connect(
        tamanoTexto,
        QOverload<int>::of(&QSpinBox::valueChanged),
        this,
        [this](int valor)
        {
            this->camara->establecerTamanoTexto(valor);
            config.guardarTamanoTexto(valor);
        }
    );

    connect(
        botonColorTexto,
        &QPushButton::clicked,
        this,
        [this]() { seleccionarColorTexto(); }
    );

    connect(
        espejo,
        &QCheckBox::toggled,
        this,
        [this](bool activo)
        {
            this->camara->activarEspejo(activo);
            config.guardarEspejo(activo);
        }
    );
}

void VentanaConfig::cargarConfiguracion()
{
    /*
     * Bloqueamos señales para que no se disparen
     * escrituras mientras cargamos.
     */

    anchoVentana->blockSignals(true);
    altoVentana->blockSignals(true);
    anchoCaptura->blockSignals(true);
    altoCaptura->blockSignals(true);
    texto->blockSignals(true);
    tamanoTexto->blockSignals(true);
    espejo->blockSignals(true);

    int aV = config.anchoVentana();
    int lV = config.altoVentana();
    int aC = config.anchoCaptura();
    int lC = config.altoCaptura();

    anchoVentana->setValue(aV);
    altoVentana->setValue(lV);
    anchoCaptura->setValue(aC);
    altoCaptura->setValue(lC);

    QString nombre = config.nombre();
    texto->setText(nombre);

    int tam = config.tamanoTexto();
    tamanoTexto->setValue(tam);

    colorTexto = config.colorTexto();
    botonColorTexto->setStyleSheet(
        QString("background-color: %1; color: %2;")
            .arg(colorTexto.name())
            .arg(
                colorTexto.lightness() < 128
                    ? "white"
                    : "black"
            )
    );

    espejo->setChecked(config.espejo());

    /*
     * Aplicamos al modelo.
     */

    ventanaCamara->resize(aV, lV);

    camara->establecerResolucionCaptura(aC, lC);
    camara->establecerTexto(nombre);
    camara->establecerTamanoTexto(tam);
    camara->establecerColorTexto(colorTexto);
    camara->activarEspejo(config.espejo());

    /*
     * Desbloqueamos señales.
     */

    anchoVentana->blockSignals(false);
    altoVentana->blockSignals(false);
    anchoCaptura->blockSignals(false);
    altoCaptura->blockSignals(false);
    texto->blockSignals(false);
    tamanoTexto->blockSignals(false);
    espejo->blockSignals(false);
}

void VentanaConfig::guardarNombre(const QString& valor)
{
    config.guardarNombre(valor);
}

void VentanaConfig::actualizarTamanoVentana()
{
    int nuevoAncho = anchoVentana->value();
    int nuevoAlto  = altoVentana->value();

    ventanaCamara->resize(nuevoAncho, nuevoAlto);
}

void VentanaConfig::actualizarResolucionCaptura()
{
    int nuevoAncho = anchoCaptura->value();
    int nuevoAlto  = altoCaptura->value();

    camara->establecerResolucionCaptura(nuevoAncho, nuevoAlto);
}

void VentanaConfig::seleccionarColorTexto()
{
    QColor elegido = QColorDialog::getColor(
        colorTexto,
        this,
        "Elegir color del texto"
    );

    if (!elegido.isValid())
        return;

    colorTexto = elegido;

    camara->establecerColorTexto(colorTexto);
    config.guardarColorTexto(colorTexto);

    botonColorTexto->setStyleSheet(
        QString("background-color: %1; color: %2;")
            .arg(colorTexto.name())
            .arg(
                colorTexto.lightness() < 128
                    ? "white"
                    : "black"
            )
    );
}
