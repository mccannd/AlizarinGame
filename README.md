# Alizarin

[![Intro Screen](https://4.bp.blogspot.com/-J2CHOgMFEJQ/WE97dR-QU3I/AAAAAAAAAFE/LKSJ9Ujpon81zqS82MqtvyrKcBTrZVThgCLcB/s1600/TitleScreen.png)](https://player.vimeo.com/video/199881242 "Menu Screen")


This was my senior design project for Fall 2016 for UPenn's digital media design program. This is a Diablo-inspired dungeon crawler created in Unreal Engine 4. The main task was to create an art-directable level generator, and the gameplay is very simple. Development has paused since then. This repo will show off the highlights of the project.

# Generator Tool

The generator is written in C++ and each 'tileset' would be a blueprint subclass of the base. For the project I created a single tileset, a space platform, and the editor within Unreal looks like this:

![](https://3.bp.blogspot.com/-ylQAXj7Uweg/WE-ATqwJIgI/AAAAAAAAAFc/0clgfOQ37IQzBtPnY_u4SLLeuxWkdIO_wCLcB/s1600/menucontrols.PNG)

The generator modules section is where I define what rooms are included in the generator.  If I develop a new room that I want to add, I find the list with the appropriate number of doorways (1, 2, 3, or 4) and click the (+) button. This will add a new drop-down menu that allows me to pick out my room. Here is what a single, ordinary room module looks like:

![](https://4.bp.blogspot.com/-0nKF8eWcumw/WE-BJgYFoII/AAAAAAAAAFk/VxHwiE6teZkS1o73njJraLvLLC2Zxx3IQCLcB/s1600/crossroads.PNG)

The same is true for the objective rooms, where there are more specific gameplay encounters. However, those rooms can cover more than one grid cell. I also must specify a start and end room. These can also be generalized across multiple cells and even ordinary objective rooms if I want. If I developed a story mode, for example, I could make a big setpiece area for the start and end to fit the plot of the level if I wanted. And if I wanted to make another tileset altogether, I could just make another blueprint class and fill in the same lists with new modules.


