#pragma once

#include "Transform.h"

#include <QList>
#include <QString>
#include <QDataStream>
#include <QHash>

using namespace std;

class Way
{
public:
	Way();

	void setName(const QString& n);
	QString name() const;

	void addNode(int ref);
	int numNodes() const;

	GridRef position(const QHash<int, GridRef>& nds);

	friend QDataStream& operator<<(QDataStream& out, const Way& hash);
	friend QDataStream& operator>>(QDataStream& in, Way& w);

private:
	QList<int> nodes;
	QString wayName;
};

inline QDataStream& operator<<(QDataStream& out, const Way& w)
{
	return (out << w.wayName << w.nodes);
}
inline QDataStream& operator>>(QDataStream& in, Way& w)
{
	return (in >> w.wayName >> w.nodes);
}
