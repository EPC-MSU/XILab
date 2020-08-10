#ifndef BINDYUSERMODEL_H
#define BINDYUSERMODEL_H

#include <QTableView>
#include "bindy.h"

class BindyUserModel : public QAbstractTableModel
{
public:
	BindyUserModel(const bindy::user_vector_t &vector, QObject *parent = 0)
		: QAbstractTableModel(parent), users(vector) {}

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

private:
	bindy::user_vector_t users;
};


#endif // BINDYUSERMODEL_H
