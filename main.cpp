#include <QApplication>
#include "window.h"

int main(int argc, char *argv[]) {
    QTranslator translator;
    translator.load("gallerizer_" + QLocale::system().name());

    QApplication application(argc, argv);
    application.installTranslator(& translator);
    Window window;
    window.show();
    
    return application.exec();
}
