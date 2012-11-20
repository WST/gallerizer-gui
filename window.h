#ifndef WINDOW_H
#define WINDOW_H

#include <QtCore>
#include <QtGui>

#define GALLERIZER_VERSION_TEXT "v 0.1"

namespace Ui {
class Window;
}

class GalleryItem {
public:
    GalleryItem(QString &filename);
    void setImageSize(unsigned int width, unsigned int height);
    void setFileSize(unsigned long int bytes);
    void setSlideSize(unsigned long int bytes);
    void setPreviewSize(unsigned long int bytes);
    unsigned int width();
    unsigned int height();
    unsigned long int size();
    unsigned long int slideSize();
    unsigned long int previewSize();
    QString filename();

private:
    unsigned int m_width;
    unsigned int m_height;
    unsigned long int m_size;
    unsigned long int m_slide_size;
    unsigned long int m_preview_size;
    QString m_filename;
};

class Window: public QWidget
{
    Q_OBJECT

private:
    bool wanna_stop;
    QString openTemplate();
    QString getSharedPath();
    void initializeTarget(QDir &target);
    bool cp(const QString &source, const QString &destination);
    void renderPage(QList<GalleryItem *> images, QFile *file);
    QString formatSize(unsigned long int bytes);

public:
    explicit Window(QWidget *parent = 0);
    ~Window();
    
private slots:
    void on_browse_clicked();
    void on_about_clicked();
    void on_start_clicked();
    void on_stop_clicked();
    void setReady();
    void setBusy();

    void on_settings_clicked();

private:
    Ui::Window *ui;
};

#endif // WINDOW_H
