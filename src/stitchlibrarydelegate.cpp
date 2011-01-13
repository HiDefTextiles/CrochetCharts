/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "stitchlibrarydelegate.h"

#include "stitch.h"
#include <QPainter>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>

#include <QStyleOption>
#include <QStyleOptionViewItem>

#include <math.h>

#include <QApplication>

#include <QDebug>
#include "stitchcollection.h"

StitchLibraryDelegate::StitchLibraryDelegate(QWidget *parent)
    : QStyledItemDelegate(parent)
{
    mSignalMapper = new QSignalMapper(this);   
    connect(mSignalMapper, SIGNAL(mapped(int)), this, SLOT(addStitchToMasterSet(int)));
}

void StitchLibraryDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(!index.isValid())
        return;
        
    if(index.column() == 5) {
        QStyleOption opt = option;

        qApp->style()->drawPrimitive(QStyle::PE_PanelButtonCommand, &opt, painter);
        QString buttonText = tr("Add Stitch");

        int width = option.fontMetrics.width(buttonText);
        int height = option.fontMetrics.height();
        int borderW = ceil((option.rect.width() - width) / 2.0);
        int borderH = ceil((option.rect.height() - height) / 4.0);
        painter->drawText(option.rect.x() + borderW, option.rect.y() + (borderH + height), buttonText);

        if(option.state == QStyle::State_Selected)
            painter->fillRect(option.rect, option.palette.highlight());
        
    } else {
        //fall back to the basic painter.
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize StitchLibraryDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    int padding = 0;
    
    if(!index.isValid())
        return QSize(100, 32);

    Stitch *s = static_cast<Stitch*>(index.internalPointer());
    if(!s)
        return QSize(100, 32);

    QString text;

    switch(index.column()) {
        case Stitch::Name:
            text = s->name();
            padding += 50;
            break;
        case Stitch::Icon:
            padding += 5;
            return QSize(64, 64); //TODO: get this from the stitch class.
        case Stitch::Description:
            padding +=150;
            text = s->description();
            break;
        case Stitch::Category:
            padding += 50;
            text = s->category();
            break;
        case Stitch::WrongSide:
            padding +=50;
            text = s->wrongSide();
            break;
        case 5:
            padding += 0;
            text = tr("Add Stitch"); //TODO: there's a button to estimate too.
            break;
        default:
            text = "";
            break;
    }
    
    QSize hint = option.fontMetrics.size(Qt::TextWordWrap, text);
    hint.setWidth(hint.width() + padding);
    
    return hint;
}

QWidget* StitchLibraryDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    if(!index.isValid())
        return new QWidget(parent);
    
    switch(index.column()) {
        case Stitch::Name:{ //TODO: add a validator that checks if the name already exists.
            QLineEdit *editor = new QLineEdit(parent);
            
            editor->setText(index.data(Qt::EditRole).toString());
            return editor;
        }
        case Stitch::Icon: {
            
            return new QWidget(parent); //TODO: create an editor widget for selecting icons.
        }
        case Stitch::Description: {
            QLineEdit *editor = new QLineEdit(parent);

            editor->setText(index.data(Qt::EditRole).toString());
            return editor;
        }
        case Stitch::Category: {
            QComboBox *cb = new QComboBox(parent);
            cb->addItems(StitchCollection::inst()->categoryList());
            cb->setCurrentIndex(cb->findText(index.data(Qt::EditRole).toString()));
            return cb;
        }
        case Stitch::WrongSide: {
            QComboBox *cb = new QComboBox(parent);
            cb->addItems(StitchCollection::inst()->stitchList());
            cb->setCurrentIndex(cb->findText(index.data(Qt::EditRole).toString()));
            return cb;
        }
        case 5: {
            QPushButton *pb = new QPushButton(parent);
            pb->setText(tr("Add Stitch"));
            mSignalMapper->setMapping(pb, index.row());
            connect(pb, SIGNAL(clicked(bool)), mSignalMapper, SLOT(map()));
            return pb;
        }
        default:
            return new QWidget(parent);
    }

    return new QWidget(parent);
}

void StitchLibraryDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(!index.isValid())
        return;

    switch(index.column()) {
        case Stitch::Name: {
            QLineEdit *le = static_cast<QLineEdit*>(editor);
            le->setText(index.data(Qt::EditRole).toString());
            break;
        }
        case Stitch::Icon: {
            //TODO: custom widget for icon editing.
            break;
        }
        case Stitch::Description: {
            QLineEdit *le = static_cast<QLineEdit*>(editor);
            le->setText(index.data(Qt::EditRole).toString());
            break;
        }
        case Stitch::Category: {
            QComboBox *cb = static_cast<QComboBox*>(editor);
            cb->setCurrentIndex(cb->findText(index.data(Qt::EditRole).toString()));
            break;
        }
        case Stitch::WrongSide: {
            QComboBox *cb = static_cast<QComboBox*>(editor);
            cb->setCurrentIndex(cb->findText(index.data(Qt::EditRole).toString()));
            break;
        }
        default:
            break;
    }
}

void StitchLibraryDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    switch(index.column()) {
        case Stitch::Icon: {
            //TODO: custom editor widget and data.
            break;
        }
        case Stitch::Name: //TODO: make the name check if it already exists before accepting any alterations.
        case Stitch::Description: {
            QLineEdit *le = static_cast<QLineEdit*>(editor);
            model->setData(index, le->text(), Qt::EditRole);
            break;
        }
        case Stitch::WrongSide:
        case Stitch::Category: {
            QComboBox *cb = static_cast<QComboBox*>(editor);
            model->setData(index, cb->currentText(), Qt::EditRole);
            break;
        }
        default:
            break;
    }
}

void StitchLibraryDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);

    editor->setGeometry(option.rect);
}

void StitchLibraryDelegate::addStitchToMasterSet(int row)
{
    qDebug() << "row: " << row;
    
}
