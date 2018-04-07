# Alizarin

[![Intro Screen](https://4.bp.blogspot.com/-J2CHOgMFEJQ/WE97dR-QU3I/AAAAAAAAAFE/LKSJ9Ujpon81zqS82MqtvyrKcBTrZVThgCLcB/s1600/TitleScreen.png)](https://player.vimeo.com/video/199881242 "Menu Screen")


This was my senior design project for Fall 2016 for UPenn's digital media design program. This is a Diablo-inspired dungeon crawler created in Unreal Engine 4. The main task was to create an art-directable level generator, and the gameplay is very simple. Development has paused since then. This readme is my postmortem and will show off the highlights of the project.

# High Level Generation Algorithm

This is a tileset generator made to allow artists (especially me) space to make interesting tiles, so it is broken up into large modules. These modules occupy a grid, with objective / hero modules generalized to multiple grid spaces. The algorithm is:

- Scatter objective rooms and remove collisions
- BFS from each objective to the next, and mark the best path as a required path
- Along each required path, recursively generate a hallway / maze area from the modules (pick some compatible number of doors, pick a room with that many doors, place it and recurse)

This means that in the Unreal editor window, none of the level exists before runtime. Unfortunately this means no light baking or nav-mesh baking. Still, with some tweaking the updating nav-mesh was responsive enough and the level still looks good with two shadow-casting lights. It runs at 60FPS on my laptop with a GTX 970M. Generating the level itself and spawning assets is on the order of milliseconds. 

# Generator Tool

The generator is written in C++ and each 'tileset' would be a blueprint (Unreal's visual scripting system) subclass of the base. This provided an easy frontend editor for someone to use while working in engine. For the project I created a single tileset, a space platform, and its editor looks like this:

![](https://3.bp.blogspot.com/-ylQAXj7Uweg/WE-ATqwJIgI/AAAAAAAAAFc/0clgfOQ37IQzBtPnY_u4SLLeuxWkdIO_wCLcB/s1600/menucontrols.PNG)

The generator modules section is where I define what rooms are included in the generator.  If I develop a new room that I want to add, I find the list with the appropriate number of doorways (1, 2, 3, or 4) and click the (+) button. This will add a new drop-down menu that allows me to pick out my room. Here is what a single, ordinary room module looks like:

![](https://4.bp.blogspot.com/-0nKF8eWcumw/WE-BJgYFoII/AAAAAAAAAFk/VxHwiE6teZkS1o73njJraLvLLC2Zxx3IQCLcB/s1600/crossroads.PNG)

The same is true for the objective rooms, where there are more specific gameplay encounters. However, those rooms can cover more than one grid cell. I also must specify a start and end room. These can also be generalized across multiple cells and even ordinary objective rooms if I want. If I developed a story mode, for example, I could make a big setpiece area for the start and end to fit the plot of the level if I wanted. And if I wanted to make another tileset altogether, I could just make another blueprint class and fill in the same lists with new modules.

# Generation Parameters and Effects

I implemented three simple parameters to control the randomness of my generator. I will break down the results of testing each extensively.

## Target Density

Treat every room module as a vertex in a graph, and the number of doorways as its degree. The Target Density (TD) is the desired average degree of all vertices. Here are some sample outputs:

![](https://2.bp.blogspot.com/-x4yhyI-e8U8/WE-EEq6m3bI/AAAAAAAAAFw/z-p2nRvEf8shCFSRVd2BtgPz5VdXZ0AIACLcB/s1600/tableDensities.png)

And here are the points of interest in the same outputs:

![](https://1.bp.blogspot.com/-W7HwVxm0r1Q/WE-Ej0rvZmI/AAAAAAAAAF0/9xHF2O20Yj4BOAZ6Uqk49ljOLex3l3mWQCLcB/s1600/tableInterests.png)

And lastly the paths found by the generation algorithm that made 'required' routes:

![](https://2.bp.blogspot.com/-bp3eVqN9eMk/WE-GDO0g3DI/AAAAAAAAAGE/uKFb5T_aE7cFVAC9hYy3-hEm_Fz0xtQMQCLcB/s1600/tablePathfinding.png)

Target Density of 1, of course, produces (usually) completely linear levels. In the top-middle example you can see that the generator placed the exit of objective 1 fairly far away from the entrance of objective 2, so the path between them has generated a cycle. But really, the only interesting thing about TD 1 is that it shows my pathfinding working. TD of 4 is also boring: it will connect everything to everything else and become a grid. Target Density of 2.5 is an even distribution because it is the expected value of a randomly chosen degree. This is probably higher than we would want if I didn't add other parameters, because it will usually fill up the allotted grid. But while the shapes are looking more interesting, it did happen to generate a pretty unfun level:

![](https://3.bp.blogspot.com/-nYNtB4gAM4o/WE-H81A52DI/AAAAAAAAAGQ/2NCDyaum12UYtBCXVnmCFC6hvOs0xYoawCLcB/s1600/badlevel.png)

This is the middle-right example in the table. I've marked each of the required cells (on connecting paths) with dots. Blue dots are in one path. Green are in two, which means backtracking. There is even a cell where all three intersect, with the red dot. What makes this egregiously bad is the objectives must be completed in order, so the far one must be completed before the near one. This backtracking / path intersection is a real issue because it won't become evident until a lot of work has already been done.  I will have to find a better strategy of placing rooms, or perhaps I will alternate between generating rooms and paths.


The other issue is labelled with the yellow numbers. They show the distance in cells from the nearest required / path cells. If you explore this path in game, you'll have to walk all the way back. These kinds of large branches are mostly gone with the addition of my other parameters, but if I allow something like this to exist then there needs to be some quicker way to get back, like a teleporter.
