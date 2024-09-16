#ifndef BIT_FIRMWARE_STATICRC_H
#define BIT_FIRMWARE_STATICRC_H

#include "RenderComponent.h"
#include "FS/File.h"

class StaticRC : public RenderComponent {
public:
	/**
	 * @param file Raw RGB565 image file.
	 * @param dim Width, height.
	 */
	StaticRC(File file, PixelDim dim, glm::vec2 scale = { 1.0f, 1.0f });
	~StaticRC() override = default;

	void setFile(File file); //no change in dim
	void setFile(File file, PixelDim dim); //change in dim
	const File& getFile() const { return file; }
	void setScale(glm::vec2 value);

protected:
	void push(Sprite& parent, PixelDim pos, float rot, bool flipX, bool flipY) const override;

private:
	File file;
	PixelDim dim;
	glm::vec2 scale{ 1.0f, 1.0f };
};


#endif //BIT_FIRMWARE_STATICRC_H
