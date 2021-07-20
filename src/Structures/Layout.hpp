#pragma once

#include "src/DistrhoDefines.h"
#include "Widget.hpp"
#include <vector>

START_NAMESPACE_DISTRHO

struct Anchors
{
	enum AnchorType
	{
		None = 1,
		Left = 2,
		Right = 4,
		Top = 8,
		Bottom = 16,
		All = 32
	};

	Anchors();
	Anchors(bool left, bool right, bool top, bool bottom);

	bool left;
	bool right;
	bool top;
	bool bottom;
};

struct RelativePosition
{
	RelativePosition();
	RelativePosition(int left, int right, int top, int bottom);

	int left;
	int right;
	int top;
	int bottom;
};

class Layout;

class LayoutItem
{
  public:
	LayoutItem(Layout *parent, Widget *widget);

	Widget *getWidget();

	LayoutItem &setAnchors(int anchors);
	LayoutItem &setAnchors(Anchors anchors);

	Anchors getAnchors();

	LayoutItem &setSize(const uint width, const uint height);
	LayoutItem &setPosition(const int x, const int y);

	void setRelativePos(int left, int right, int top, int bottom);
	void setRelativeLeft(int left);
	void setRelativeRight(int right);
	void setRelativeTop(int top);
	void setRelativeBottom(int bottom);

	RelativePosition getRelativePos();

  private:
	Widget *fWidget;
	Layout *fParent;
	Anchors fAnchors;

	// the absolute distance of the item from the sides of the layout
	RelativePosition fRelativePos;
};

class Layout : public Widget
{
  public:
	Layout(Widget *parent);
	LayoutItem &addItem(Widget *widget);
	size_t getItemCount();
	LayoutItem *getItem(const uint index);
	LayoutItem *getFirstItem();
	LayoutItem *getLastItem();

  protected:
	virtual void onItemAdded(const LayoutItem &item);
	std::vector<LayoutItem> fItems;
};

class RelativeLayout : public Layout
{
  public:
	RelativeLayout(Widget *parent);
	void repositionItems(Size<uint> oldSize, Size<uint> newSize);
	void repositionItems();

  protected:
	void onResize(const ResizeEvent &ev) override;
	void onPositionChanged(const PositionChangedEvent &ev) override;
	void onDisplay() override;
};

END_NAMESPACE_DISTRHO
