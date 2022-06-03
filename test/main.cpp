#include <irrlicht.h>
#include "driverChoice.h"
#include "exampleHelper.h"

using namespace irr;

int main()
{
    video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;

	IrrlichtDevice *device = createDevice(driverType, core::dimension2d<u32>(512, 384));

	device->setWindowCaption(L"Irrlicht Engine - 2D Graphics Demo");

	video::IVideoDriver* driver = device->getVideoDriver();

	const io::path mediaPath = getExampleMediaPath();

	video::ITexture* images = driver->getTexture(mediaPath + "2ddemo.png");
	driver->makeColorKeyTexture(images, core::position2d<s32>(0,0));

	core::rect<s32> imp1(349,15,385,78);
	core::rect<s32> imp2(387,15,423,78);

	while(device->run() && driver)
	{
		if (device->isWindowActive())
		{
			u32 time = device->getTimer()->getTime();

			driver->beginScene(video::ECBF_COLOR | video::ECBF_DEPTH, video::SColor(255,120,102,136));

			// draw fire & dragons background world
			driver->draw2DImage(images, core::position2d<s32>(50,50),
				core::rect<s32>(0,0,342,224), 0,
				video::SColor(255,255,255,255), true);

			// draw flying imp
			driver->draw2DImage(images, core::position2d<s32>(164,125),
				(time/500 % 2) ? imp1 : imp2, 0,
				video::SColor(255,255,255,255), true);

			driver->endScene();
		}
	}

	device->drop();

	return 0;
}
