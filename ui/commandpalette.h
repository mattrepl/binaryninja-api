#pragma once

#include <QtWidgets/QListView>
#include <QtCore/QAbstractItemModel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QFrame>
#include <QtCore/QPointer>
#include <vector>
#include "action.h"

struct BINARYNINJAUIAPI CommandListItem
{
	QString name;
	QString shortcut;
	QString action;
};

class CommandPalette;
class CommandListFilter;

class BINARYNINJAUIAPI CommandListModel: public QAbstractItemModel
{
	Q_OBJECT

	std::vector<CommandListItem> m_items;
	std::vector<CommandListItem> m_allItems;

	bool isFilterMatch(const QString& name, const QString& filter);
	int getFilterMatchScore(const QString& name, const QString& filter);

public:
	CommandListModel(QWidget* parent, const std::vector<CommandListItem>& items);

	virtual QModelIndex index(int row, int col, const QModelIndex& parent) const override;
	virtual QModelIndex parent(const QModelIndex& i) const override;
	virtual bool hasChildren(const QModelIndex& parent) const override;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex& parent) const override;
	virtual QVariant data(const QModelIndex& i, int role) const override;

	QString getActionForItem(int row);
	void setFilterText(const QString& text);
};

class BINARYNINJAUIAPI CommandList: public QListView
{
	Q_OBJECT

	CommandPalette* m_palette;
	CommandListModel* m_model;
	CommandListFilter* m_filter;

public:
	CommandList(CommandPalette* parent, const std::vector<CommandListItem>& items);
	void setFilter(CommandListFilter* filter) { m_filter = filter; }
	void setFilterText(const QString& text);

	QString getActionForItem(int row);

	QModelIndex index(int row, int col, const QModelIndex& parent = QModelIndex()) const;

protected:
	virtual void keyPressEvent(QKeyEvent* event) override;
	virtual void focusOutEvent(QFocusEvent* event) override;
};

class BINARYNINJAUIAPI CommandListFilter: public QLineEdit
{
	Q_OBJECT

	CommandPalette* m_palette;
	CommandList* m_list;

public:
	CommandListFilter(CommandPalette* parent, CommandList* list);

protected:
	virtual void keyPressEvent(QKeyEvent* event) override;
	virtual void focusOutEvent(QFocusEvent* event) override;
};

class BINARYNINJAUIAPI CommandPalette: public QFrame
{
	Q_OBJECT

	UIActionHandler* m_handler;
	UIActionContext m_context;
	QPointer<QWidget> m_previousWidget;

	CommandListFilter* m_filter;
	CommandList* m_list;

	bool m_executing;

	std::vector<CommandListItem> getCommandList();
	void init();

public:
	CommandPalette(QWidget* parent, UIActionHandler* handler);
	CommandPalette(QWidget* parent, UIActionHandler* handler, const UIActionContext& context);

	void focusInput();
	void activateFirstItem();
	void selectFirstItem();
	void close();

private Q_SLOTS:
	void itemClicked(const QModelIndex& idx);
	void filterChanged(const QString& text);
};
