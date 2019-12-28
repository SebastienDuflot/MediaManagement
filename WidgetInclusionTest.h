#ifndef WIDGET_INCLUSION_TEST_H
#define WIDGET_INCLUSION_TEST_H

#include "MediaFileData.h"

#include <QWidget>
#include <QSettings>
#include <QFutureWatcher>

namespace Ui {
class WidgetInclusionTest;
}

class WidgetInclusionTest : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetInclusionTest( QWidget * parent, QSettings & p_settings );
    ~WidgetInclusionTest();

private:
    void checkInclusion_RelativeFilePath();
    void checkInclusion_FileID();

private slots:
    void on_pushButton_switchDirs_clicked();
    void on_pushButton_referenceDirectory_clicked();
    void on_pushButton_subsetDirectory_clicked();
    void on_pushButton_checkInclusion_clicked();

    void slot_analysisRef_finished();
    void slot_analysisSubset_finished();

private:
    QSettings & m_settings;

    Ui::WidgetInclusionTest *ui;

    QFutureWatcher<QMap<FileID, QList<MediaFileData>>> m_watcherAnalysisRef;
    QFutureWatcher<QMap<FileID, QList<MediaFileData>>> m_watcherAnalysisSubset;

    QMap<FileID, QList<MediaFileData>> m_mapFileID_FileDataList_ref;
    QMap<FileID, QList<MediaFileData>> m_mapFileID_FileDataList_subset;
};

#endif // WIDGET_INCLUSION_TEST_H
