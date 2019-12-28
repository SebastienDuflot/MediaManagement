#ifndef WIDGETDUPLICATESEARCH_H
#define WIDGETDUPLICATESEARCH_H

#include <QWidget>
#include <QSettings>
#include <QFutureWatcher>

#include "MediaFileData.h"

namespace Ui
{
class WidgetDuplicateSearch;
}

class WidgetDuplicateSearch : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetDuplicateSearch( QWidget * parent, QSettings & p_settings );
    ~WidgetDuplicateSearch();

private:
    void startAnalysis_thread_not_used();

private slots:
    void on_pushButton_chooseDirectory_clicked();
    void on_pushButton_startAnalysis_clicked();

    void slot_receiveNumberOfFiles( int nNumber );
    void slot_receiveAnalysisProgress( double dPercent );

    void slot_analysisFinished();

signals:
    void signal_numberOfFiles( int nNumber );
    void signal_analysisProgress( double dPercent );

private:
    QSettings & m_settings;

    Ui::WidgetDuplicateSearch * ui;

    QFutureWatcher<QMap<FileID, QList<MediaFileData>>> m_watcherAnalysis;
};

#endif // WIDGETDUPLICATESEARCH_H
