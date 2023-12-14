#include "bindyusermodel.h"

int BindyUserModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)
	return static_cast<int>(users.size());
}

int BindyUserModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)
	return 3;
}

QVariant BindyUserModel::data(const QModelIndex &index, int role) const
{
	if (role == Qt::DisplayRole) {
		switch (index.column()) {
		case 0: return QString::fromStdString(users.at(index.row()).name);
		case 1: return QString(QByteArray((char*)&users.at(index.row()).uid, sizeof (bindy::user_id_t)).toHex());
		case 2: return QString(QByteArray((char*)&users.at(index.row()).key, sizeof (bindy::aes_key_t)).toHex());
		default: return QVariant();
		}
	}
	else
		return QVariant();
}

QVariant BindyUserModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
		switch (section) {
		case 0: return QString("user");
		case 1: return QString("uuid");
		case 2: return QString("key");
		default: return QVariant();
		}
	}
	else
		return QVariant();
}
