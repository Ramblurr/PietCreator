#include "ResizeDialog.h"

#include <QtGui>

ResizeDialog::ResizeDialog( const QSize &size, QWidget *parent ) :
    QDialog( parent )
{
    QVBoxLayout* mainLayout = new QVBoxLayout( this );
    QWidget* formWidget = new QWidget( this );
    QFormLayout* formLayout = new QFormLayout( formWidget );
    QLabel* widthLabel = new QLabel( this );
    widthLabel->setText( tr( "Width:" ) );
    QLabel* heightLabel = new QLabel( this );
    heightLabel->setText( tr( "Height:" ) );

    mWidthSpin = new QSpinBox( this );
    mWidthSpin->setMinimum( 1 );
    mWidthSpin->setMaximum( 100 );
    mWidthSpin->setSuffix( tr( " codels" ) );
    mWidthSpin->setValue( size.width() );
    mHeightSpin = new QSpinBox( this );
    mHeightSpin->setMinimum( 1 );
    mHeightSpin->setMaximum( 1000 );
    mHeightSpin->setSuffix( tr( " codels" ) );
    mHeightSpin->setValue( size.height() );

    formLayout->addRow( widthLabel, mWidthSpin );
    formLayout->addRow( heightLabel, mHeightSpin );

    formWidget->setLayout( formLayout );
    mainLayout->addWidget( formWidget );

    QDialogButtonBox* buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
    connect( buttonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( buttonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );

    mainLayout->addWidget( buttonBox );
    setLayout( mainLayout );
}

ResizeDialog::~ResizeDialog()
{
}

QSize ResizeDialog::newSize() const
{
    return QSize( mWidthSpin->value(), mHeightSpin->value() );
}

#include "ResizeDialog.moc"
