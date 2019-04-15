#pragma once

#include <QtWidgets/QLabel>
#include "viewframe.h"
#include "menus.h"
#include "uicontext.h"


class BINARYNINJAUIAPI AddressIndicator: public MenuHelper
{
	Q_OBJECT

	uint64_t m_begin, m_end;

public:
	AddressIndicator(QWidget* parent);

	void setOffsets(uint64_t begin, uint64_t end);

protected:
	virtual void showMenu();
};
