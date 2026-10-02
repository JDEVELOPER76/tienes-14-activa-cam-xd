#include <QApplication>
#include <QDebug>
#include <QDir>

#include "camara/Camara.h"
#include "gui/VentanaCamara.h"
#include "gui/VentanaConfig.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qDebug() << "===== Arrancando CamaraApp =====";
    qDebug() << "Directorio de trabajo:" << QDir::currentPath();

    Camara camara;

    if (!camara.abrir(0))
    {
        qDebug() << "ERROR: no se pudo abrir la cámara";
        return 1;
    }

    VentanaCamara ventanaCamara(&camara);
    VentanaConfig ventanaConfig(&camara, &ventanaCamara);

    ventanaCamara.show();
    ventanaConfig.show();

    return app.exec();
}
