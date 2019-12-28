#include "widget.h"
#include "ui_widget.h"

#include "WidgetDuplicateSearch.h"
#include "WidgetInclusionTest.h"

Widget::Widget( QWidget * parent ) :
    QWidget( parent ),
    ui( new Ui::Widget )
{
    ui->setupUi( this );

    ui->tabWidget->addTab( new WidgetDuplicateSearch( this, m_settings ), "Duplicate Search" );
    ui->tabWidget->addTab( new WidgetInclusionTest  ( this, m_settings ), "Inclusion Check" );
}

Widget::~Widget()
{
    delete ui;
}
