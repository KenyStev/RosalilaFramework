Rosalila Engine
===============

[![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](https://github.com/Rosalila/RosalilaEngine/blob/master/LICENSE)
[![Travis](https://img.shields.io/travis/Rosalila/RosalilaEngine.svg?logo=travis)](https://travis-ci.org/Rosalila/RosalilaEngine)

1. **Simple**: You only need to understand a few functions to create your game.
2. **Modern**: Use the prebuilt third party API’s support to catch up with the thrends.
3. **Libre**: Free as in freedom, available even for commercial projects.

| Feature | Supported |
|----------|------------ |
| Windows / Linux / MacOS | ✔ |
| Flexible 2d graphics | ✔ |
| Simplified sound and music api | ✔ |
| PC, XInput, PS4 and Steam input supported | ✔ |
| Built in JSON parsing | ✔ |
| Steamworks | ✔ |
| Collision detection, math, seeded randoms, logs etc... | ✔ |
| Libre, MIT licensed | ✔ |

## Usage

### The basics

#### Draw an image

```c++
Image* image = rosalila()->graphics->getImage("image.png");
rosalila()->graphics->drawImage(image, 100, 200 /*x, y position*/);
```

#### Draw a rectangle

```c++
rosalila()->graphics->drawRectangle(10, 20, /*position*/
                                    200, 150, /*size*/
                                    0.0, /*rotation*/
                                    255, 0, 0, 255); /*RGBA*/
```

#### Play music and sounds

```c++
rosalila()->sound->playMusic("music.ogg", 0 /*music loops, 0 means infinite*/);
rosalila()->sound->addSound("my_sound", "sound.ogg");

[...]

rosalila()->sound->playSound("my_sound", -1 /*channel, use -1 to autopick*/, 1 /*loops, 0 means infinite*/, -1 /*stereo panning, 0 means mono, range from 1 to screen width*/);

[...]

rosalila()->sound->stopMusic();
```

#### Check if an input is down or was pressed

```c++
if(rosalila()->receiver->isDown(0 /*0 is player 1, 1 is player 2 etc.*/, "up"))
{
    // Player 1's "Up" button is down
}

if(rosalila()->receiver->isPressed(1 /*0 is player 1, 1 is player 2 etc.*/, "a"))
{
    // Player 2 pressed the "a" button
}
```

### Advanced stuff

#### Unlock an achievement (Compatible with Steam)

![Alt text](img/achievements.png?raw=true)

```c++
rosalila()->api_integrator->unlockAchievement("My achievement");
```

#### Get a stat (Compatible with Steam)

![Alt text](img/stats.png?raw=true)

```c++
int counter = rosalila()->api_integrator->getStat("My counter");
```

#### Set a stat (Compatible with Steam)

```c++
rosalila()->api_integrator->setStat("My counter",counter + 1);
```

#### Submit a leaderboard entry (Compatible with Steam)

![Alt text](img/leaderboards.png?raw=true)

```c++
rosalila()->api_integrator->setScore("Top scores", 143 /*score*/);
```

#### Get leaderboard (Compatible with Steam)

```c++
rosalila()->api_integrator->findLeaderboard("Top scores"); // Retreive the leaderboard externally

while (rosalila()->api_integrator->getState() == "loading") // Wait for it...
{
  rosalila()->update();
}

if (rosalila()->api_integrator->getState() != "error")
{
  Leaderboard *leaderboard = rosalila()->api_integrator->getLeaderboard("Top scores");

  for (auto entry : current_leaderboard->top_entries) // Inspect entries
  {
    int rank = entry->rank;
    std::string name = entry->name;
    int score = entry->score;
    [...]
  }

  for (auto entry : current_leaderboard->near_entries) // Same thing for entries near the player
  {
    [...]
  }

  for (auto entry : current_leaderboard->friends_entries) // And for his friend's entries
  {
    [...]
  }
} else
{
  // Handle errors
}
```

#### Check if external API is running (Compatible with Steam)

```c++
if (rosalila()->api_integrator->isUsingApi())
{
  // External api is running
}
```

#### Trigger a notification

![Alt text](img/notifications.gif?raw=true)

```c++
rosalila()->graphics->notification_handler.notifications.push_back(
  new Notification(image,
                    0 /*x*/, 0 /*y*/,
                    100 /*target y, used for moving animation*/,
                    300 /*duration, in frames*/));

[...]

rosalila()->graphics->notification_handler.interruptCurrentNotification(); // Interrupt / Hide it
```

#### Get screen size

```c++
int width = rosalila()->graphics->screen_width;
int height = rosalila()->graphics->screen_height;
```

#### Get a random number

```c++
int random_number = rosalila()->utility->getRandomNumber();
int non_seeded_random_number = rosalila()->utility->getNonSeededRandomNumber();
```

#### Seed randoms

```c++
rosalila()->utility->setRandomSeed(543245 /*seed*/);
```

#### Write a log line

```c++
rosalila()->utility->writeLogLine("My log text.");
```

#### Read json

```c++
Node* example_root_node = rosalila()->parser->getNodes("example.json");

Node* tag = example_root_node->getNodeByName("Tag");
cout << tag->attributes["a"] << endl;

for(auto array_element : example_root_node->getNodesByName("Array"))
{
  cout << array_element->attributes["value"] << endl;
}
```

### Fancy details


#### Add gfx to your images

![Alt text](img/gfx.gif?raw=true)

```c++
image.scale = 0.5; // defaults to 1.0
image.width = image.original_width * 2;
image.rotation = 45.0; // defaults to 0.0
image.flip_horizontally = true; // defaults to false
image.blend_effect = true; //  defaults to false
image.color_filter.red = 0; // defaults to RGBA(255,255,255,255)
image.color_filter.alpha = 128;
```

#### Fade music

```c++
rosalila()->sound->fadeMusicVolume(128 /*Target volume, 0 min 128 max*/,
                                   2 /*Delta change speed*/);
```

#### Set transparency effect

```c++
rosalila()->graphics->transparency_effect.set(1.0 /*Target alpha percentage*/,
                                              0.03 /*Delta change speed*/);
```

## Getting started

Link the Rosalila libraries and include it.

```c++
#include "Rosalila.h"

int main()
{
  rosalila()->init("config.json");
  
  // Initialize stuff here
  
  while(true)
  {
    // Draw, play sounds, check inputs here

    rosalila()->update();
  }

  return 0;
}
```

Also, setup a `font.ttf`, `notification_background.png` and a `config.json` file.

config.json:

```json
{
  "Resolution": 
  {
    "x": "1280",
    "y": "720"
  },
  "ScreenSize": 
  {
    "x": "1280",
    "y": "720"
  },
  "Fullscreen": 
  {
    "enabled": "no"
  },
  "Font": 
  {
    "path": "font.ttf",
    "red": "255",
    "green": "255",
    "blue": "255",
    "size": "30"
  },
  "Notifications": 
  {
    "background_path": "notification_background.png"
  },
  "Inputs": 
  {
    "Player": 
    {
      "number": "0",
      "Type": 
      [
        {
          "name": "keyboard",
          "button": 
          [
            {
              "name": "up",
              "key": "up"
            },            
            {
              "name": "down",
              "key": "down"
            },            
            {
              "name": "left",
              "key": "left"
            },            
            {
              "name": "right",
              "key": "right"
            },            
            {
              "name": "a",
              "key": "z"
            },            
            {
              "name": "b",
              "key": "x"
            }
          ]
        },        
        {
          "name": "gamepad",
          "button": 
          [            
            {
              "name": "up",
              "key": "up"
            },            
            {
              "name": "down",
              "key": "down"
            },            
            {
              "name": "left",
              "key": "left"
            },            
            {
              "name": "right",
              "key": "right"
            },            
            {
              "name": "a",
              "key": "1"
            },            
            {
              "name": "b",
              "key": "2"
            }
          ]
        }
      ]
    }
  }
}
```

## Build

```
# Dependencies
sudo apt-get install g++ cmake libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev libsdl2-net-dev libglew-dev

# Compile
mkdir build
cd build
cmake ..
make
```
