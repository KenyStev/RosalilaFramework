#include "Rosalila.h"

int main()
{
  rosalila()->init("../assets/config.json");

  Image* image = rosalila()->graphics->getImage("../assets/image.png");
  Image* image2 = rosalila()->graphics->getImage("../assets/x.png");

  while(true)
  {
    rosalila()->graphics->draw2DImage(image, 0, 0);
    rosalila()->graphics->draw2DImage(image2, 0, 0);

    rosalila()->update();
  }
  return 0;
}