#include "listviews.h"
#include "network/messages.h"
#include "playergamelistings.h"

ObserverListModel::ObserverListModel()
{
	QList <QVariant> rootData;
	rootData << "Name" << "Rank";
	headerItem = new ListItem(rootData);

	for(int i = 0; i < O_TOTALCOLUMNS; i++)
		sort_priority.append(i);
	//list_sort_order = Qt::AscendingOrder;
	list_sort_order = Qt::DescendingOrder;	//initially unsorted
}

ObserverListModel::~ObserverListModel()
{
	delete headerItem;
}

void ObserverListModel::insertListing(const PlayerListing * listing)
{
	for(int i = 0; i < items.count(); i++)
	{
		if(((ObserverListItem *)items[i])->data(OC_NAME) == listing->name)
			return;
	}
	ObserverListItem * item = new ObserverListItem(listing);
	ListModel::insertListing(*item);	
}

void ObserverListModel::removeListing(const PlayerListing * listing)
{
	for(int i = 0; i < items.count(); i++)
	{
		const ObserverListItem * item = static_cast<const ObserverListItem *>(items[i]);
		if(item->getListing() == listing)
		{
			emit beginRemoveRows(QModelIndex(), i, i);
			items.removeAt(i);
			emit endRemoveRows();
			delete item;
			return;
		}
	}
}

void ObserverListModel::clearList(void)
{
	if(!items.count())
		return;
	emit beginRemoveRows(QModelIndex(), 0, items.count() - 1);
	for(int i = 0; i < items.count(); i++)
	{
		const ObserverListItem * item = static_cast<const ObserverListItem *>(items[i]);
		items.removeAt(i);
		i--;		//right? FIXME
		delete item;
	}
	emit endRemoveRows();
}

QVariant ObserverListModel::data(const QModelIndex & index, int role) const
{
	if(!index.isValid())
		return QVariant();

	ObserverListItem * item = static_cast<ObserverListItem*>(index.internalPointer());
	if(role == Qt::DisplayRole)
		return item->data(index.column());
	else if(role == Qt::ForegroundRole)
	{
		if(item->getListing()->name == account_name)
			return Qt::blue;
		else
			return QVariant();
	}
	else
		return QVariant();
}

PlayerListModel::PlayerListModel()
{
	QList <QVariant> rootData;
	
	rootData << "Stat" << "Name" << "Rank" << "Play"<< "Obs" <<
			"Won" << "Lost" << "Idle" << "Country" << "Match Prefs";
	headerItem = new ListItem(rootData);

	for(int i = 0; i < P_TOTALCOLUMNS; i++)
		sort_priority.append(i);
	//list_sort_order = Qt::AscendingOrder;
	list_sort_order = Qt::DescendingOrder;	//initially unsorted
}

PlayerListModel::~PlayerListModel()
{
	delete headerItem;
}

void PlayerListModel::insertListing(const PlayerListing * l)
{
	PlayerListItem * item = new PlayerListItem(l);	
	ListModel::insertListing(*item);	
	//sort(P_TOTALCOLUMNS);	//too slow?	FIXME
}

void PlayerListModel::updateListing(const PlayerListing * l)
{
	//if(!l)
	//	return;	//nothing to update
	//removeListing(l);
	for(int i = 0; i < items.count(); i++)
	{
		if(static_cast<const PlayerListItem *>(items[i])->getListing() == l)
		{
			//this listing needs to be reloaded
			emit dataChanged(createIndex(i,0), createIndex(i, headerItem->columnCount() - 1)); 
			return;
		}
	}
	insertListing(l);
}

void PlayerListModel::removeListing(const PlayerListing * l)
{
	for(int i = 0; i < items.count(); i++)
	{
		const PlayerListItem * item = static_cast<const PlayerListItem *>(items[i]);
		if(item->getListing() == l)
		{
			emit beginRemoveRows(QModelIndex(), i, i);
			items.removeAt(i);
			emit endRemoveRows();
			delete item;
			return;
		}
	}
}

void PlayerListModel::clearList(void)
{
	if(!items.count())
		return;
	//emit beginRemoveRows(QModelIndex(), 0, items.count() - 1);
	for(int i = 0; i < items.count(); i++)
	{
		const PlayerListItem * item = static_cast<const PlayerListItem *>(items[i]);
		emit beginRemoveRows(QModelIndex(), i, i);
		items.removeAt(i);
		emit endRemoveRows();
		i--;	//right? removeAt changes items.count()...
		delete item;
	}
}

QVariant PlayerListModel::data(const QModelIndex & index, int role) const
{
	if(!index.isValid())
		return QVariant();

	PlayerListItem * item = static_cast<PlayerListItem*>(index.internalPointer());
	if(role == Qt::DisplayRole)
		return item->data(index.column());
	else if(role == Qt::ForegroundRole)
	{
		if(item->getListing()->name == account_name)
			return Qt::blue;
		else
			return QVariant();
	}
	else
		return QVariant();
}

const PlayerListing * PlayerListModel::playerListingFromIndex(const QModelIndex & index)
{
	PlayerListItem * item = static_cast<PlayerListItem*>(index.internalPointer());
	return item->getListing();
}

GamesListModel::GamesListModel()
{
	QList <QVariant> rootData;
	rootData << "Id" << "White" << "WR" << "Black" << "BR" << "Mv" << "Sz"
		<< "H" << "K" << "By" << "Flags" << "Obs";
	headerItem = new ListItem(rootData);

	for(int i = 0; i < G_TOTALCOLUMNS; i++)
		sort_priority.append(i);
	//list_sort_order = Qt::AscendingOrder;
	list_sort_order = Qt::DescendingOrder;	//initially unsorted
}

GamesListModel::~GamesListModel()
{
	delete headerItem;
}

void GamesListModel::insertListing(const GameListing * l)
{
	GamesListItem * item = new GamesListItem(l);	
	ListModel::insertListing(*item);	
	/*emit dataChanged(createIndex(0, 0),
			createIndex(items.count() - 1, headerItem->columnCount() - 1));
	*/
}

void GamesListModel::updateListing(const GameListing * l)
{
	//if(!l)
	//	return;	//nothing to update
	/* FIXME:
	 * I've come to the conclusion that this isn't really a bug.  It has
	 * to do with the template registries.  We sort of misused our
	 * own template when we adapted to the player/games registries.
	 * Specifically, if there's no player or game there, there's no
	 * information to create one.  We could create it with just the
	 * name itself, but then we might as well wait to get the stats
	 * information.  At any rate, that means we pass a null to the initEntry
	 * which then calls this.  I think... I think I'm going to alter it to
	 * send out stats requests all around */
	//if(!l)
	//	qDebug("GamesListModel::updateListing passed null listing!!!!!");
	for(int i = 0; i < items.count(); i++)
	{
		if(static_cast<const GamesListItem *>(items[i])->getListing() == l)
		{
			//this item needs to be reloaded
			emit dataChanged(createIndex(i,0), createIndex(i, headerItem->columnCount() - 1)); 
			return;
		}
	}
	insertListing(l);
}

void GamesListModel::removeListing(const GameListing * l)
{
	for(int i = 0; i < items.count(); i++)
	{
		const GamesListItem * item = static_cast<const GamesListItem *>(items[i]);
		if(item->getListing() == l)
		{
			/* Really this is supposed to be not QModelIndex() but the
			 * parent model index of the model... ?!?? */
			emit beginRemoveRows(QModelIndex(), i, i);
			items.removeAt(i);
			emit endRemoveRows();
			//qDebug("Removing %p %s %p %s", item->getListing()->white, item->getListing()->white_name().toLatin1().constData(),
			 //     item->getListing()->black, item->getListing()->black_name().toLatin1().constData());
			delete item;
			return;
		}
	}
	qDebug("Couldn't find listing to remove for game id %d", l->number);
	exit(0);
}

void GamesListModel::clearList(void)
{
	if(!items.count())
		return;
	//emit beginRemoveRows(QModelIndex(), 0, items.count() - 1);
	for(int i = 0; i < items.count(); i++)
	{
		const GamesListItem * item = static_cast<const GamesListItem *>(items[i]);
		emit beginRemoveRows(QModelIndex(), i, i);
		items.removeAt(i);
		emit endRemoveRows();
		i--;		//right FIXME?? 
		delete item;
	}
}

QVariant GamesListModel::data(const QModelIndex & index, int role) const
{
	if(!index.isValid())
		return QVariant();
	
	GamesListItem * item = static_cast<GamesListItem*>(index.internalPointer());
	if(role == Qt::DisplayRole)
		return item->data(index.column());
	else if(role == Qt::ForegroundRole)
	{
		return QVariant();
	}
	else
		return QVariant();
}

const GameListing * GamesListModel::gameListingFromIndex(const QModelIndex & index)
{
	GamesListItem * item = static_cast<GamesListItem*>(index.internalPointer());
	return item->getListing();
}

void ListModel::sort(int column, Qt::SortOrder order)
{
	/* Take column out of previous place in sort_priority and
	 * put it in first place */
	if(column != headerItem->columnCount())	//if ! resort
	{
		for(int i = 0; i < headerItem->columnCount(); i++)
		{
			if(sort_priority[i] == column)
			{
				sort_priority.removeAt(i);
				sort_priority.prepend(column);
				break;
			}
		}
		/* This method below of swapping the sort order is annoying
		 * because it makes it almost like a puzzle trying to get it
		 * to properly prioritize in the right ascension order. At
		 * the very least, we need a different sort order for
		 * each column, which data can check out at the time.
		 * Sort also seems a bit slow right now, there's a definite
		 * beat there... FIXME */
		// For some dumb reason, order isn't changing anymore... so:
		//list_sort_order = order;
		if(list_sort_order == Qt::AscendingOrder)
			list_sort_order = Qt::DescendingOrder;
		else
			list_sort_order = Qt::AscendingOrder;
	}
	//qDebug("sort! %d %d\n", sort_priority[0], order);
	quicksort(0, items.count() - 1);
	/* FIXME We might want to make this more specific if its what's causing the slowdown */
	emit dataChanged(createIndex(0, 0, 0),
			createIndex(items.count() - 1, headerItem->columnCount() - 1, 0));
}

void ListModel::quicksort(int b, int e)
{
	if(b < e)
	{
		int partition = qs_partition(b, e);
		quicksort(b, partition - 1);
		quicksort(partition + 1, e);
	}
}

int ListModel::qs_partition(int b, int e)
{
	const ListItem * item, * pivot_item;
	pivot_item = items[e];
	int i, j;
	i = b - 1;
	for(j = b; j < e; j++)
	{
		item = items[j];
		switch(priorityCompare(*item, *pivot_item))
		{
			case LESSTHAN:
			case EQUALTO:
				i++;
				items[j] = items[i];
				items[i] = item;
				break;
			case GREATERTHAN:
				break;
		}
	}
	items[e] = items[i + 1];
	items[i + 1] = pivot_item;
	return i + 1;
}

void ListModel::insertListing(const ListItem & item)
{
	//for(int j = 0; j < G_TOTALCOLUMNS; j++)
	//{
		//int sortColumn = sort_priority[0];
		//list = &(items[sortColumn]);
		for(int i = 0; i < items.count(); i++)
		{
			int result = priorityCompare(item, *(items[i]));
			if(result == GREATERTHAN || result == EQUALTO)
			{
				emit beginInsertRows(QModelIndex(), i + 1, i + 1);
				items.insert(i, &item); 
				//emit beginInsertRows(QModelIndex(), i, i);
				emit endInsertRows();
				return;
			}
		}
		emit beginInsertRows(QModelIndex(), items.count(), items.count());
		items.append(&item);
		//emit beginInsertRows(QModelIndex(), items.count() - 1, items.count() - 1);
		emit endInsertRows();
	//}
}

int ListModel::priorityCompare(const ListItem & i, const ListItem & j)
{
	static int k = 0;
	int columns = columnCount(QModelIndex());
	int result;
	result = i.compare(j, sort_priority[k]);
	if(result == EQUALTO && k < columns - 1)
	{
		k++;
		result = priorityCompare(i, j);
	}
	k = 0;	
	return result;
}

ObserverListItem::ObserverListItem(const PlayerListing * l) : listing(l)
{
}

QVariant ObserverListItem::data(int column) const
{
	switch(column)
	{
		case OC_NAME:
			return listing->name;
			break;
		case OC_RANK:
			return listing->rank;
			break;
		default:
			return QVariant();
			break;
	}
}

int ObserverListItem::columnCount(void) const
{
	return O_TOTALCOLUMNS;
}

int ObserverListItem::compare(const ListItem & i, int column) const
{
	const ObserverListItem & l = (const ObserverListItem &)i;
	switch(column)
	{
		case OC_NAME:
			return compareNames(listing->name, l.listing->name);
			break;
		case OC_RANK:
			if(listing->rank_score > l.listing->rank_score)
				return GREATERTHAN;
			else if(listing->rank_score == l.listing->rank_score)
				return EQUALTO;
			else
				return LESSTHAN;
			break;
		default:
			break;
	}
	return EQUALTO;
}

GamesListItem::GamesListItem(const GameListing * l) : listing(l)
{
}


int GamesListItem::compare(const ListItem & i, int column) const
{
	const GamesListItem & g = (const GamesListItem &)i;
	if(!listing)
	{
		qDebug("!!! No listing on GamesListItem!!!");
		return LESSTHAN;
	}
	switch((GameListingColumn)column)
	{
		case GC_ID:
			if(listing->number > g.getListing()->number)
				return GREATERTHAN;
			else if(listing->number == g.getListing()->number)
				return EQUALTO;
			else
				return LESSTHAN;
			break;
		case GC_WHITENAME:
			return compareNames(listing->white_name(), g.getListing()->white_name());
			break;
		case GC_WHITERANK:
			if(listing->white_rank_score() > g.getListing()->white_rank_score())
				return GREATERTHAN;
			else if(listing->white_rank_score() == g.getListing()->white_rank_score())
				return EQUALTO;
			else
				return LESSTHAN;
			break;
		case GC_BLACKNAME:
			return compareNames(listing->black_name(), g.getListing()->black_name());
			break;
		case GC_BLACKRANK:
			if(listing->black_rank_score() > g.getListing()->black_rank_score())
				return GREATERTHAN;
			else if(listing->black_rank_score() == g.getListing()->black_rank_score())
				return EQUALTO;
			else
				return LESSTHAN;
			break;
		case GC_MOVES:
			if(listing->moves > g.getListing()->moves)
				return GREATERTHAN;
			else if(listing->moves == g.getListing()->moves)
				return EQUALTO;
			else
				return LESSTHAN;
			break;
		case GC_SIZE:
			if(listing->board_size > g.getListing()->board_size)
				return GREATERTHAN;
			else if(listing->board_size == g.getListing()->board_size)
				return EQUALTO;
			else
				return LESSTHAN;
			break;
		case GC_HANDICAP:
			if(listing->handicap > g.getListing()->handicap)
				return GREATERTHAN;
			else if(listing->handicap == g.getListing()->handicap)
				return EQUALTO;
			else
				return LESSTHAN;
			break;
		case GC_KOMI:
			if(listing->komi > g.getListing()->komi)
				return GREATERTHAN;
			else if(listing->komi == g.getListing()->komi)
				return EQUALTO;
			else
				return LESSTHAN;
			break;
		case GC_BYOMI:
			if(listing->By > g.getListing()->By)
				return GREATERTHAN;
			else if(listing->By == g.getListing()->By)
				return EQUALTO;
			else
				return LESSTHAN;
			break;
		case GC_FLAGS:
			if(listing->FR > g.getListing()->FR)
				return GREATERTHAN;
			else if(listing->FR == g.getListing()->FR)
				return EQUALTO;
			else
				return LESSTHAN;
			break;
		case GC_OBSERVERS: 
			if(listing->observers > g.getListing()->observers)
				return GREATERTHAN;
			else if(listing->observers == g.getListing()->observers)
				return EQUALTO;
			else
				return LESSTHAN;
			break;
		default:
			break;
	}
	return EQUALTO;
}

/* Probably better ways to do this, an enum, something, maybe
 * an enum keyed with the names set in the header above, but
 * right now we're just getting it out there */
QVariant GamesListItem::data(int column) const
{
	if(!listing)
	{
		/* I think this has to do with us getting a games message
		 * from a game we're playing that isn't in the list FIXME */
		qDebug("!!! No listing on GamesListItem!!!");
		return QVariant();
	}
	
	std::map<PlayerListing *, unsigned short>::iterator it;
	if(listing->white)
		it = removed_player.find(listing->white);
	if(listing->white && it != removed_player.end())
	{
		qDebug("Listing: %p %d", listing, listing->number);
		qDebug("Found removed white player: %p, %d game %d", listing->white, removed_player[listing->white], listing->number);
		//QMessageBox here is BAD, its in some Qt draw loop here
		//QMessageBox::information(0 , "Crash Imminent!", "Game list corruption!");
		return QVariant();
	}
	if(listing->black)
		it = removed_player.find(listing->black);
	if(listing->black && it != removed_player.end())
	{
		qDebug("Listing: %p %d", listing, listing->number);
		qDebug("Found removed black player: %p, %d game %d", listing->black, removed_player[listing->black], listing->number);
		//QMessageBox::information(0 , "Crash Imminent!", "Game list corruption!");
		return QVariant();
	}
	switch(column)
	{
		case GC_ID:
			return QVariant(listing->number);
			break;
		case GC_WHITENAME:
			//qDebug("%s", listing->white_name().toLatin1().constData());
			return QVariant(listing->white_name());
			break;
		case GC_WHITERANK:
			//qDebug("%s", listing->white_rank().toLatin1().constData());
			return QVariant(listing->white_rank());
			break;
		case GC_BLACKNAME:
			//qDebug("%s", listing->black_name().toLatin1().constData());
			return QVariant(listing->black_name());
			break;
		case GC_BLACKRANK:
			return QVariant(listing->black_rank());
			break;
		case GC_MOVES:
			return QVariant(listing->moves);
			break;
		case GC_SIZE:
			return QVariant(listing->board_size);
			break;
		case GC_HANDICAP:
			return QVariant(listing->handicap);
			break;
		case GC_KOMI:
			return QVariant(listing->komi);
			break;
		case GC_BYOMI:
			return QVariant(listing->By);
			break;
		case GC_FLAGS:
			return QVariant(listing->FR);
			break;
		case GC_OBSERVERS: 
			return QVariant(listing->observers);
			break;
		default:
			return QVariant();
			break;
	}
}

int GamesListItem::columnCount(void) const
{
	return G_TOTALCOLUMNS;
}

PlayerListItem::PlayerListItem(const PlayerListing * l) : listing(l)
{
}

/* We changed the playing and observing
 * to ints to point to game id but this
 * will be a problem if there's ever a game 0.
 * -1 would fix it but I'll hold off on it
 *  for now.  The way it is is better for sorting. */
QVariant PlayerListItem::data(int column) const
{
	switch(column)
	{
		case PC_STATUS:
			return QVariant(listing->info);
			break;
		case PC_NAME:
			return QVariant(listing->name);
			break;
		case PC_RANK:
			return QVariant(listing->rank);
			break;
		case PC_PLAYING:
			if(listing->playing == 0)
				return QVariant("-");
			return QVariant(listing->playing);
			break;
		case PC_OBSERVING:
			if(listing->observing == 0 ||
			  	listing->observing == listing->playing)
				return QVariant("-");
			return QVariant(listing->observing);
			break;
		case PC_WINS:
			return QVariant(listing->wins);
			break;
		case PC_LOSSES:
			return QVariant(listing->losses);
			break;
		case PC_IDLE:
			return QVariant(listing->idletime);
			break;
		case PC_COUNTRY:
			return QVariant(listing->country);
			break;
		case PC_MATCHPREFS:
			//is this right?  FIXME takes up too much space, should go with simple numbers, abbrev.
			return QVariant(listing->nmatch_settings);
			break;
		default:
			return QVariant();
			break;
	}
}

int PlayerListItem::columnCount(void) const
{
	return P_TOTALCOLUMNS;
}

int PlayerListItem::compare(const ListItem & i, int column) const
{
	const PlayerListItem & p = (const PlayerListItem &)i;
	switch((GameListingColumn)column)
	{
		case PC_STATUS:
			if(listing->info > p.getListing()->info)
				return GREATERTHAN;
			else if(listing->info == p.getListing()->info)
				return EQUALTO;
			else
				return LESSTHAN;
			break;
		case PC_NAME:
			return compareNames(listing->name, p.getListing()->name);
			break;
		case PC_RANK:
			if(listing->rank_score > p.getListing()->rank_score)
				return GREATERTHAN;
			else if(listing->rank_score == p.getListing()->rank_score)
				return EQUALTO;
			else
				return LESSTHAN;
			break;
		case PC_PLAYING:
			if(listing->playing > p.getListing()->playing)
				return GREATERTHAN;
			else if(listing->playing == p.getListing()->playing)
				return EQUALTO;
			else
				return LESSTHAN;
			break;
		case PC_OBSERVING:
			if(listing->observing > p.getListing()->observing)
				return GREATERTHAN;
			else if(listing->observing == p.getListing()->observing)
				return EQUALTO;
			else
				return LESSTHAN;
			break;
		case PC_WINS:
			if(listing->wins > p.getListing()->wins)
				return GREATERTHAN;
			else if(listing->wins == p.getListing()->wins)
				return EQUALTO;
			else
				return LESSTHAN;
			break;
		case PC_LOSSES:
			if(listing->losses > p.getListing()->losses)
				return GREATERTHAN;
			else if(listing->losses == p.getListing()->losses)
				return EQUALTO;
			else
				return LESSTHAN;
			break;
		case PC_IDLE:
			if(listing->seconds_idle > p.getListing()->seconds_idle)
				return GREATERTHAN;
			else if(listing->seconds_idle == p.getListing()->seconds_idle)
				return EQUALTO;
			else
				return LESSTHAN;
			break;
		case PC_COUNTRY:
			return compareNames(listing->country, p.getListing()->country);
			break;
		case PC_MATCHPREFS:
			/* FIXME Need to add this all around */
			break;
		default:
			break;
	}
	return EQUALTO;
}

ListItem::ListItem(const QList <QVariant> & data)
{
	itemData = data;	
}

QVariant ListItem::data(int column) const
{
	return itemData.value(column);
}

int ListItem::columnCount(void) const
{
	return itemData.count();
}

/* This assumes WAY WAY too much about the character sets and locale I think */
int ListItem::compareNames(const QString & name1, const QString & name2) const
{
	int shorter = (name1.count() < name2.count() ? name1.count() : name2.count());
	char c1, c2;
	for(int i = 0; i < shorter; i++)
	{
		c1 = (char)name1[i].toAscii();
		c2 = (char)name2[i].toAscii();
		if(c1 >= 'a')
			c1 -= ('a' - 'A');
		if(c2 >= 'a')
			c2 -= ('a' - 'A');
		if(c1 < c2)
			return LESSTHAN;
		else if(c1 > c2)
			return GREATERTHAN;
	}
	if(name1.count() < name2.count())
		return LESSTHAN;
	else if(name1.count() > name2.count())
		return GREATERTHAN;
	else
		return EQUALTO;
}

ListModel::ListModel()
{

}

ListModel::~ListModel()
{
	const ListItem * item;
	for(int i = 0; i < items.count(); i++)
	{
		item = items[i];
		delete item;
	}
}

/* parent is necessary */
QModelIndex ListModel::parent(const QModelIndex & /*index*/) const
{
	return QModelIndex();
}

int ListModel::rowCount(const QModelIndex & /* parent*/) const
{
	return items.count();
}

int ListModel::columnCount(const QModelIndex & /* parent */) const
{
	return headerItem->columnCount();
}

QModelIndex ListModel::index(int row, int column, const QModelIndex & /*parent*/) const
{
	const ListItem * item;
	if(items.count() == row)
		return QModelIndex();
	if(list_sort_order == Qt::DescendingOrder)
		item = items[row];
	else
		item = items[items.count() - row - 1];
	if(item)
		return createIndex(row, column, (void*)item);
	else
		return QModelIndex();
}

QVariant ListModel::data(const QModelIndex & index, int role) const
{
	if(!index.isValid())
		return QVariant();
	
	ListItem * item = static_cast<ListItem*>(index.internalPointer());
	if(role == Qt::DisplayRole)
		return item->data(index.column());
	else if(role == Qt::ForegroundRole)
		return QVariant();
	else
		return QVariant();
}

Qt::ItemFlags ListModel::flags(const QModelIndex & index) const
{
	if(!index.isValid())
		return Qt::ItemIsEnabled;
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant ListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return headerItem->data(section);
	return QVariant();
}

void PlayerSortProxy::setFilter(int rn, int rm)
{
	// might be worth calling sort here ???
	// but what's the active sort column? That
	// needs to be fixed anyway with the ascending
	// stuff FIXME
	rankMin = rn;
	rankMax = rm;
	qDebug("player sort: %d - %d\n", rn, rm);
	sort(P_TOTALCOLUMNS);	//resort as current
}

void PlayerSortProxy::setFilter(bool oo)
{
	// might be worth calling sort here ??? FIXME
	openOnly = oo;
	sort(P_TOTALCOLUMNS);	//resort as current
}

void PlayerSortProxy::sort(int column, Qt::SortOrder order)
{
	sourceModel()->sort(column, order);
	invalidateFilter();
}


bool PlayerSortProxy::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
	QModelIndex flagIndex = sourceModel()->index(sourceRow, PC_STATUS, sourceParent);
	QModelIndex rankIndex = sourceModel()->index(sourceRow, PC_RANK, sourceParent);
	QModelIndex playingIndex = sourceModel()->index(sourceRow, PC_PLAYING, sourceParent);
	const PlayerListing * p = dynamic_cast<PlayerListModel *>(sourceModel())->playerListingFromIndex(flagIndex);

	if(openOnly)
	{
		if(sourceModel()->data(flagIndex).toString().contains("X") ||
			p->playing != 0)
			return false;
	}
	if(p->rank_score > (unsigned int)rankMax || p->rank_score + 1 < (unsigned int)rankMin)
		return false;
	return true;
}

