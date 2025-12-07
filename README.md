<div align="center">
  <img src="assets/double-silverfish-spawner-finder-logo.webp" alt="Double silverfish spawner finder logo" style="width:25%; image-rendering: pixelated;" onerror="this.style.display='none';">
  <h1>[WIP] Double silverfish spawner finder</h1>
</div>

## C++ console application to search for Minecraft Java Edition seeds with double silverfish spawners

[![en](https://img.shields.io/badge/lang-en-green.svg)](https://github.com/KK-mp4/double-silverfish-spawner-finder/blob/master/README.md)
[![ru](https://img.shields.io/badge/lang-ru-red.svg)](https://github.com/KK-mp4/double-silverfish-spawner-finder/blob/master/README.ru.md)
[![DeepWiki](https://deepwiki.com/badge.svg)](https://deepwiki.com/KK-mp4/double-silverfish-spawner-finder)

## Introduction

I have [a concept for a silverfish-based quarry](https://youtu.be/gz_HdqsPPS4) that would leave behind itself only ores and other stone types. A lot of work needs to be done still but what is clear is that I do need a way to quickly build a 1 high floor of infested stone blocks using [glowsquid's 4 game tick floor placer](https://youtu.be/aA4V4Ws8_ig). Thus as a side quest I'd like to find 2 nearby silverfish spawners so that with 1 players we could still get x2 speed.

> [!NOTE]
> Contributions to this repository are welcome. More about how to contribute on the [development guide](https://github.com/KK-mp4/double-silverfish-spawner-finder?tab=contributing-ov-file) page.

## Stronghold generation

Silverfish spawners [were added](https://minecraft.wiki/w/Silverfish#History) in Minecraft Java Edition 1.0.0 and they generate only in portal rooms of strongholds. You probably know that strongholds generate in rings. In `1.0.0 - 1.8.9` there is only one ring and in `1.9+` there are 9 rings. After it chose the location it actually searches for a valid biome in +/-112 block range:

```java
// Pseudo-code StrongholdStructure.java
private Biome[] validBiomes = new Biome[]{DESERT, FOREST, EXTREME_HILLS, SWAMPLAND, TAIGA, ICE_PLAINS, ICE_MOUNTAINS, DESERT_HILLS, FOREST_HILLS, EXTREME_HILLS_EDGE, JUNGLE, JUNGLE_HILLS};
```

After it found location in valid biome it starts placing the structure piece by piece.

## A quest to find seeds

To simulate biome generation of Minecraft [cubiomes](https://github.com/Cubitect/cubiomes) was used. Sadly it is only enough to find where stronghold starts generating, to get actual position of the silverfish spawner (portal room) you would have to simulate stronghold generation logic, but it's a good enough estimation for right now, although some manual filtration is needed to find locations where 2 silverfish spawners are actually activated from one player.

> [!TIP]
> For previewing old Minecraft seeds I recommend using [Amidst](https://github.com/toolbox4minecraft/amidst), [Cubiomes Viewer](https://github.com/Cubitect/cubiomes-viewer) or [Minemap](https://github.com/hube12/Minemap).

## Results

### Single version search

At first I had hopes that random walking of strongholds (+/-112 blocks) while they search for valid biomes may lead to two of them being close together. Without doing actual math I wrote the seed finder and apparently it is not possible to get them quite close enough together. After scanning some seeds here is the best one I found:

| Seed | Approximate distance, blocks | Version | X1 | Z1 | X2 | Z2
|-|-|-|-|-|-|-
| 7510906 | 812.86 | 1.5.1 | 4 | -524 | 484 | 132

In versions after stronghold counter was increased to 128 they are even farther apart.

### Inter-version search

So to find double silverfish spawner I had to think outside the box. Stronghold generation as well as world generation changed a lot from version to version. So my idea was to find a seed where stronghold generation from different versions shifts them just enough that portal rooms are close together but not in the same chunk. This way it is possible to generate one silverfish spawner in one version, then upgrade and generate the other one.

At first I chose two versions where stronghold count was increased from 3 to 128: `1.8.9` and `1.9`. I immediately started getting strongholds that are just ~50 blocks apart.

Then I tried version where world generation changed: `1.6.4` and `1.7` and bingo! Since the strongholds try to generate in the same place they are close together "by design", however invariants in the biome map make them shift around by a small amount. Now I started getting strongholds that are less then 30 blocks apart in nearly every seed.

Since I had no way of getting actual positions of portal rooms I had to filter though those seeds by hand and after ~10 seeds I finally found location, where portal rooms are just close enough for double silverfish spawner to be active:

| Seed | Approximate distance, blocks | Version 1 | X1 | Z1 | Version 2 | X2 | Z2
|-|-|-|-|-|-|-|-
| 382397 | 16 | 1.6.4 | 980 | 244 | 1.8.9 | 980 | 228

<p align="center">
  <img src="assets/double-silverfish-spawner.webp" alt="Double silverfish spawner" style="width:75%;" onerror="this.style.display='none';">
</p>

In fact with further improvements to this seed finder (actually calculating exact locations of silverfish spawners and cycling though all versions where stronghold or world generation changed) I am pretty sure it's possible to get 9 silverfish spawners close by, technically the limit is just the number of chunks that can generate spawner within reach of one player.

> [!TIP]
> If you input seed 0 into seed field Minecraft would actually generate random one. To generate actual seed 0 world, as Panda4994 explained his video "*[Seeds & Generation #01: Ways to Enter a Seed](https://youtu.be/OLS7CCgNcuY)*" you would need to enter something like `PDFYFCD` as suggested by [seedinfo](https://panda4994.github.io/seedinfo/seedinfo.html#0) tool.

If you want to learn more about seed finding I recommend [fnseedc](https://github.com/SeedFinding/fnseedc) repository.

## Setup with [VSCode](https://code.visualstudio.com/)

If you are using other IDE, you probably know that you are doing and able to compile C++ code yourself. Down below I will provide a simple setup.

This project includes the [`.vscode/extensions.json`](https://github.com/KK-mp4/double-silverfish-spawner-finder/blob/main/.vscode/extensions.json) file, meaning that when you open project it will prompt you with "*Do you want to install recommended extensions?*" notification. Click yes.

Now click "*Left Control + Shift + P*" to open quick actions tab and search for "*CMake: Configure*". Then scan for kits and if there is none you would have to install some C++ compiler. After that is done you can compile this project for your system and run it.

To edit array of versions where search will happen edit this line in [`include/config.hpp`](https://github.com/KK-mp4/double-silverfish-spawner-finder/blob/main/include/config.hpp) (cubiomes versions):

```cpp
inline constexpr int MC_VERSIONS[2] = {MC_1_6_4, MC_1_7};
```

## Contributors

<a href="https://github.com/KK-mp4/double-silverfish-spawner-finder/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=KK-mp4/double-silverfish-spawner-finder" alt="GitHub contributors" />
</a>

## [License](https://github.com/KK-mp4/double-silverfish-spawner-finder/blob/master/LICENSE.md)

This program is licensed under the MIT License. Please read the License file to know about the usage terms and conditions.
