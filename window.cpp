#include "window.h"
#include "ui_window.h"

GalleryItem::GalleryItem(QString &filename): m_filename(filename) {

}

void GalleryItem::setImageSize(unsigned int width, unsigned int height) {
    m_width = width;
    m_height = height;
}

void GalleryItem::setFileSize(unsigned long int bytes) {
    m_size = bytes;
}

void GalleryItem::setSlideSize(unsigned long int bytes) {
    m_slide_size = bytes;
}

void GalleryItem::setPreviewSize(unsigned long int bytes) {
    m_preview_size = bytes;
}

unsigned int GalleryItem::width() {
    return m_width;
}

unsigned int GalleryItem::height() {
    return m_height;
}

unsigned long int GalleryItem::size() {
    return m_size;
}

unsigned long int GalleryItem::slideSize() {
    return m_slide_size;
}

unsigned long int GalleryItem::previewSize() {
    return m_preview_size;
}

QString GalleryItem::filename() {
    return m_filename;
}

Window::Window(QWidget *parent): QWidget(parent), ui(new Ui::Window) {
    ui->setupUi(this);
    wanna_stop = false;
    setReady();
    ui->start->setEnabled(false);
    ui->settings->setVisible(false);
}

Window::~Window() {
    delete ui;
}

void Window::on_browse_clicked() {
    QString path = QFileDialog::getExistingDirectory(this, tr("Choose a directory"), QDesktopServices::storageLocation(QDesktopServices::DesktopLocation), /* QFileDialog::DontUseNativeDialog | */ QFileDialog::ShowDirsOnly | QFileDialog::ReadOnly);
    if(path.isEmpty()) return;

    ui->path->setText(path);

    setReady();
}

void Window::on_about_clicked() {
    QMessageBox::information(this, tr("About this program"), tr("Gallerizer - an image collection indexing tool\n(c) 2012 Ilya Averkov <averkov.web.id>\nIcons by Yusuke Kamiyamane <p.yusukekamiyamane.com>"));
}

void Window::on_start_clicked() {
    setBusy();

    ui->infobox->append(tr("<b style=\"color: #FF0000\">Gallerizer started</b><br><b style=\"color: #FFFF00\">Gonna gallerize %1...</b>").arg(ui->path->text()));

    QStringList filters;
    filters << "*.jpg" << "*.jpeg"/* << "*.png" << "*.gif" << "*.bmp"*/;
    QDir directory(ui->path->text());
    QStringList filenames = directory.entryList(filters);

    ui->progress->setMinimum(0);
    ui->progress->setValue(0);
    ui->progress->setMaximum(filenames.count());

    initializeTarget(directory);

    QList<GalleryItem *> images;

    // Цикл обработки изображений
    foreach(QString filename, filenames) {
        ui->infobox->append(tr("<font color=\"#FFFFFF\">Processing file &lt;%1&gt;</font>").arg(filename));

        // Наше изображение
        QFile image_file(ui->path->text() + "/" + filename);
        QImage image;

        if(!image.load(ui->path->text() + "/" + filename)) {
            ui->infobox->append(tr("<font color=\"#FF0000\">Failed to process &lt;%1&gt;</font>").arg(filename));
            continue;
        }

        // Слайд и эскиз
        QString slide_filename = ui->path->text() + "/slides/" + filename;
        QString preview_filename = ui->path->text() + "/previews/" + filename;
        QFile slide_file(slide_filename);
        QFile preview_file(preview_filename);
        QImage slide = image.scaled(1024, 600, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QImage preview = slide.scaled(160, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // Сохраняем слайд и эскиз
        slide.save(slide_filename, "jpg", 95);
        preview.save(preview_filename, "jpg", 80);

        // Геренируем описание элемента
        GalleryItem *item = new GalleryItem(filename);
        item->setFileSize(image_file.size());
        item->setImageSize(preview.width(), preview.height());
        item->setPreviewSize(preview_file.size());
        item->setSlideSize(slide_file.size());
        images.push_back(item);

        if(wanna_stop) {
            ui->infobox->append(tr("<b style=\"color: #FF00FF\">Operation cancelled!</b>"));
            setReady();
            return;
        }

        ui->progress->setValue(ui->progress->value() + 1);
        qApp->processEvents();
    }

    QFile file(ui->path->text() + "/index.html");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    renderPage(images, & file);
    file.close();

    foreach(GalleryItem *item, images) {
        delete item;
    }
    images.clear();

    ui->infobox->append(tr("<b style=\"color: #FF00FF\">Operation completed!</b>"));

    setReady();
}

void Window::setReady() {
    ui->start->setEnabled(true);
    ui->start->setVisible(true);
    ui->stop->setEnabled(false);
    ui->stop->setVisible(false);
    ui->browse->setEnabled(true);
    ui->progress->setEnabled(false);
    ui->progress->setValue(0);
}

void Window::setBusy() {
    ui->start->setEnabled(false);
    ui->start->setVisible(false);
    ui->stop->setEnabled(true);
    ui->stop->setVisible(true);
    ui->browse->setEnabled(false);
    ui->progress->setEnabled(true);
}

void Window::on_stop_clicked() {
    setReady();
    wanna_stop = true;
}

bool Window::cp(const QString &source, const QString &destination) {

    QDir source_directory(source);
    QDir destination_directory(destination);

    if(!source_directory.exists()) return false;
    if(!destination_directory.exists()) {
        QString name = destination_directory.dirName();
        if(destination_directory.cdUp()) {
            destination_directory.mkdir(name);
            destination_directory.cd(destination);
        } else {
            return false;
        }
    }

    foreach(const QFileInfo &info, source_directory.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot)) {
        QString source_item = source + "/" + info.fileName();
        QString destination_item = destination + "/" + info.fileName();
        if(info.isDir()) {
            if(!cp(source_item, destination_item)) return false;
        } else if (info.isFile()) {
            if(!QFile::copy(source_item, destination_item)) return false;
        }
    }
    return true;
}


void Window::on_settings_clicked() {
    QMessageBox::information(this, QString::fromUtf8("Настройки"), QString::fromUtf8("Редактирование настроек ещё не реализовано\nПрограмма использует настройки по умолчанию"));
}

void Window::initializeTarget(QDir &target) {
    ui->infobox->append(tr("<font color=\"#FFFFFF\">Creating target directories for slides and previews</font>"));
    target.mkdir("slides");
    target.mkdir("previews");
    ui->infobox->append(tr("<font color=\"#FFFFFF\">Copying needed files</font>"));
    cp(getSharedPath() + "css", target.absolutePath() + "/css");
    cp(getSharedPath() + "images", target.absolutePath() + "/images");
    cp(getSharedPath() + "js", target.absolutePath() + "/js");
}

QString Window::getSharedPath() {
    return QString(":/shared/");
}

QString Window::openTemplate() {
    QFile t(getSharedPath() + "template/index.htt");
    t.open(QIODevice::ReadOnly);
    QString data = t.readAll();
    t.close();
    return data;
}

void Window::renderPage(QList<GalleryItem *> images, QFile *file) {
    QString page_template = openTemplate();
    page_template.replace("{TITLE}", ui->title->text().toUtf8());

    unsigned long int iteration = 0;
    unsigned long int size = 0;
    unsigned long int previews_size = 0;
    unsigned long int slides_size = 0;

    QString block;
    block.append("<tr>");

    foreach(GalleryItem *image, images) {

        if(iteration ++ % 5 == 0) block.append("</tr><tr>");

        QString current("<td width=\"20%\" align=\"center\" valign=\"middle\"><a href=\"slides/%1\" rel=\"lightbox[group]\" title=\"&lt;a href=&quot;%2&quot;&gt;%3&lt;/a&gt;\"><img src=\"previews/%4\" width=\"%5\" height=\"%6\" /></a></td>");
        block.append(current.arg(image->filename(), image->filename(), image->filename(), image->filename(), QString::number(image->width()), QString::number(image->height())));

        size += image->size();
        previews_size += image->previewSize();
        slides_size += image->slideSize();
    }

    block.append("</tr>");

    page_template.replace("{PHOTOS}", block);
    page_template.replace("{SIZE}", formatSize(size));
    page_template.replace("{PREVIEWS_SIZE}", formatSize(previews_size));
    page_template.replace("{SLIDES_SIZE}", formatSize(slides_size));
    page_template.replace("{TOTAL_SIZE}", formatSize(size + previews_size + slides_size));
    page_template.replace("{UPDATED}", QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss"));
    page_template.replace("{COUNT}", QString::number(iteration));
	page_template.replace("{VERSION}", "0.4");

    ui->infobox->append(tr("<font color=\"#FFFFFF\">Rendering HTML page</font>"));
    file->write(page_template.toStdString().c_str());
}

QString Window::formatSize(unsigned long bytes) {
    const char *m[] = {"b", "kb", "M", "G", "T"};
    unsigned short int i = 0;
    double dbytes = (double) bytes;
    while(dbytes >= 1024.0) {
        dbytes /= 1024.0;
        i ++;
    }

    return QString::number(dbytes, 'f', 2) + QString(" %1").arg(m[i]);
}
