#include "UIButton.h"

#include <string>

#include "FreeType.h"
#include "Noggit.h" // app.getArial12()
#include "TextureManager.h" // TextureManager
#include "UIText.h"
#include "Video.h" // Texture

UIButton::UIButton(float pX, float pY, float w, float h, const std::string& pTexNormal, const std::string& pTexDown)
	: UIFrame(pX, pY, w, h)
	, texture(TextureManager::newTexture(pTexNormal))
	, textureDown(TextureManager::newTexture(pTexDown))
	, _textureFilename(pTexNormal)
	, _textureDownFilename(pTexDown)
	, clickFunc(NULL)
	, id(0)
	, clicked(false)
	, text(new UIText(width() / 2.0f, 2.0f, app.getArial12(), eJustifyCenter))
{
	addChild(text);
}

UIButton::UIButton(float pX, float pY, float w, float h, const std::string& pText, const std::string& pTexNormal, const std::string& pTexDown)
	: UIFrame(pX, pY, w, h)
	, texture(TextureManager::newTexture(pTexNormal))
	, textureDown(TextureManager::newTexture(pTexDown))
	, _textureFilename(pTexNormal)
	, _textureDownFilename(pTexDown)
	, clickFunc(NULL)
	, id(0)
	, clicked(false)
	, text(new UIText(width() / 2.0f, 2.0f, pText, app.getArial12(), eJustifyCenter))
{
	addChild(text);
}

UIButton::UIButton(float pX, float pY, float h, const std::string& pText, const std::string& pTexNormal, const std::string& pTexDown)
	: UIFrame(pX, pY, app.getArial12().width(pText) + 20.0f, h)
	, texture(TextureManager::newTexture(pTexNormal))
	, textureDown(TextureManager::newTexture(pTexDown))
	, _textureFilename(pTexNormal)
	, _textureDownFilename(pTexDown)
	, clickFunc(NULL)
	, id(0)
	, clicked(false)
	, text(new UIText(width() / 2.0f, 2.0f, pText, app.getArial12(), eJustifyCenter))
{
	addChild(text);
}

UIButton::UIButton(float pX, float pY, float w, float h, const std::string& pText, const std::string& pTexNormal, const std::string& pTexDown, void(*pFunc)(UIFrame *, int), int pFuncParam)
	: UIFrame(pX, pY, w, h)
	, texture(TextureManager::newTexture(pTexNormal))
	, textureDown(TextureManager::newTexture(pTexDown))
	, _textureFilename(pTexNormal)
	, _textureDownFilename(pTexDown)
	, clickFunc(pFunc)
	, id(pFuncParam)
	, clicked(false)
	, text(new UIText(width() / 2.0f, 2.0f, pText, app.getArial12(), eJustifyCenter))
{
	addChild(text);
}

UIButton::UIButton(float pX, float pY, float w, float h, const std::string& pText, const std::string& pTexNormal, const std::string& pTexDown, boost::function<void(UIFrame::Ptr, int)> pFunc, int pFuncParam)
	: UIFrame(pX, pY, w, h)
	, texture(TextureManager::newTexture(pTexNormal))
	, textureDown(TextureManager::newTexture(pTexDown))
	, _textureFilename(pTexNormal)
	, _textureDownFilename(pTexDown)
	, clickFunc(pFunc)
	, id(pFuncParam)
	, clicked(false)
	, text(new UIText(width() / 2.0f, 2.0f, pText, app.getArial12(), eJustifyCenter))
{
	addChild(text);
}

UIButton::~UIButton()
{
	TextureManager::delbyname(_textureFilename);
	TextureManager::delbyname(_textureDownFilename);
}

void UIButton::setLeft()
{
	text->setJustify(eJustifyLeft);
	text->x(10.0f);
}

void UIButton::setText(const std::string& pText)
{
	text->setText(pText);
}

void UIButton::render() const
{
	gl.pushMatrix();
	gl.translatef(x(), y(), 0.0f);

	gl.color3f(1.0f, 1.0f, 1.0f);

	opengl::texture::set_active_texture();
	opengl::texture::enable_texture();

	if (!clicked)
		texture->bind();
	else
		textureDown->bind();

	gl.begin(GL_TRIANGLE_STRIP);
	gl.texCoord2f(0.0f, 0.0f);
	gl.vertex2f(0.0f, 0.0f);
	gl.texCoord2f(1.0f, 0.0f);
	gl.vertex2f(width(), 0.0f);
	gl.texCoord2f(0.0f, 1.0f);
	gl.vertex2f(0.0f, height());
	gl.texCoord2f(1.0f, 1.0f);
	gl.vertex2f(width(), height());
	gl.end();

	opengl::texture::disable_texture();

	text->render();

	gl.popMatrix();
}

UIFrame* UIButton::processLeftClick(float /*mx*/, float /*my*/)
{
	clicked = true;
	if (clickFunc)
		clickFunc(this, id);
	return this;
}

void UIButton::processUnclick()
{
	clicked = false;
}

void UIButton::setClickFunc(void(*f)(UIFrame *, int), int num)
{
	clickFunc = f;
	id = num;
}