#include "UILabel.h"
#include "UI/UIManager.h"
#include "Rendering/Renderers/TextRenderer.h"
#include "UIWidgetContext.h"
UILabel::UILabel(std::string  text, int w, int h, int x, int y) : UIWidget(w, h, x, y)
{
	MText = text;
	TextScale = TextDefaultScale;
	Colour = glm::vec3(1);
}


UILabel::~UILabel()
{}
//todo: Fit in box
void UILabel::Render()
{
	if (OwningContext)
	{
		OwningContext->RenderTextToScreen(MText, OwningContext->Offset.x + (float)X + 10, OwningContext->Offset.y + (float)Y + ((mheight / 2.0f) - (TextScale)), TextScale, Colour);
	}
}

void UILabel::ResizeView(int w, int h, int x, int y)
{
	mheight = h;
	mwidth = w;
	X = x;
	Y = y;
}

void UILabel::SetText(const std::string& text)
{
	MText = text;
}

std::string UILabel::GetText()
{
	return MText;
}
