/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "model.h"
#include <QtGlobal>
#include "api-windowed-list.h"

#define LOG_PRINTF(format,...) do { log_printf(format, ## __VA_ARGS__); } while(0)
#define LOG_GEN() do { log_printf("=== LOG_GEN: [%s: %u: %s]\n", __FILE__, __LINE__, __PRETTY_FUNCTION__); } while(0)
#define LOG_GEN_PRINTF(format,...) do { log_printf((std::string("=== LOG_GEN: [%s: %u: %s] ")+format).c_str(), __FILE__, __LINE__, __PRETTY_FUNCTION__, ## __VA_ARGS__); } while(0)
extern void log_printf(const char *pFormat, ...);

using namespace api;

const int LIST_WINDOW_SIZE = 50;
const int LIST_PREFETCH_SIZE = 15;
const int TOTAL_ITEM_COUNT = 1000;

struct ListItemContent {
    ListItemContent(int a_n = -1) : n(a_n) {}
    int n;
};
typedef WindowedList<ListItemContent, LIST_WINDOW_SIZE, LIST_PREFETCH_SIZE> MyWindowedList;

ListItemContent g_allListItemContent[2000];
ListItemContent g_dummyListItemContent;
MyWindowedList g_myWindowedList(g_dummyListItemContent);

Animal::Animal(const QString &type, const QString &size)
    : m_type(type), m_size(size)
{
}

QString Animal::type() const
{
    return m_type;
}

QString Animal::size() const
{
    return m_size;
}

int AnimalModel::currentIndex() {
    return m_currentIndex;
}
void AnimalModel::setCurrentIndex(int currentIndex) {
    m_currentIndex = currentIndex;
    LOG_GEN_PRINTF("currentIndex=%d\n", currentIndex);
    emit currentIndexChanged(m_currentIndex);
}

void AnimalModel::slotGetItemDataFromBL(int i)
{
    int begin = 0, end = 0;

    if (begin < 0) {
        return;
    }

    begin = i;
    end = i + LIST_WINDOW_SIZE;

    if (end > TOTAL_ITEM_COUNT) {
        end = TOTAL_ITEM_COUNT;
    }

    LOG_GEN_PRINTF("i=%d, range=[%d,%d)\n", i, begin, end);
    g_myWindowedList.update(begin, &g_allListItemContent[begin], end - begin);
    emit dataChanged(createIndex(begin, 0), createIndex(end - 1, 0));
}

void AnimalModel::getItemDataFromBL(int i)
{
    //LOG_GEN_PRINTF("i=%d\n", i);
    emit sigGetItemDataFromBL(i);
}

//#define TO_FIX_CUREENT_INDEX_ISSUE

QVariant AnimalModel::data(const QModelIndex & index, int role) const {
    //LOG_GEN_PRINTF("index=%d\n", index.row());

#ifdef TO_FIX_CUREENT_INDEX_ISSUE
    g_myWindowedList.getItem2(index.row(), [this](std::size_t i){ ((AnimalModel *)this)->getItemDataFromBL(i); }, ((int)index.row() == m_currentIndex));
#else
    g_myWindowedList.getItem(index.row(), [this](std::size_t i){ ((AnimalModel *)this)->getItemDataFromBL(i); });
#endif // #ifdef TO_FIX_CUREENT_INDEX_ISSUE

    if (index.row() < 0 || index.row() >= m_animals.count())
        return QVariant();

    const Animal &animal = m_animals[index.row()];
    if (role == TypeRole)
        return animal.type();
    else if (role == SizeRole)
        return animal.size();
    return QVariant();
}

AnimalModel::AnimalModel(QObject *parent)
    : QAbstractListModel(parent), m_currentIndex(-1)
{
    g_myWindowedList.setFullListSize(TOTAL_ITEM_COUNT);
    connect(this, SIGNAL(sigGetItemDataFromBL(int)), this, SLOT(slotGetItemDataFromBL(int)), Qt::QueuedConnection);
}

void AnimalModel::addAnimal(const Animal &animal)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_animals << animal;
    endInsertRows();
}

int AnimalModel::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return m_animals.count();
}

QHash<int, QByteArray> AnimalModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[TypeRole] = "type";
    roles[SizeRole] = "size";
    return roles;
}

void AnimalModel::initializeItems()
{
    for (unsigned int i = 0; i < TOTAL_ITEM_COUNT; i++) {
        QString Name = QString::asprintf("Wolf_%04u", i);
        addAnimal(Animal(Name, "Medium"));
    }
}
