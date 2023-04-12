# tomb4 changelog

## Unreleased:

### Original bug fixes

- Fix the random fires on screen bug.
- Fix Ahmet/Scale save crashes in Temple of Horus.
- Fix Lara snapping when moving off a ledge while wading.
- Fix floordata issues with triangular geometry, no collision tiles, and more generic situations.

### tomb4 bug fixes

- Fix original style bar drawing on certain GPUs.
- Fix Rail/Crowbar switch interaction.
- Fix inventory items drawing.
- Improve lighting in some edge cases.
- Fix first voncroy cut camera.
- Fix AI (VonCroy, Guide, etc.) interaction position.
- Fix water surface waves.

### new tomb4 features

- Improve how the hair interacts with water.

## [2.1.0](https://github.com/Trxyebeep/TOMB4/tree/V2.1.0) (Mar. 3 2023)

### Original bug fixes

- Fix fog wrongly affecting rope.
- Fix Lara performing the wrong walk animation when interacting with some objects.
- Fix multiple lara hand deadlocks after certain actions.
- Fix combine SFX not playing when combining the small waterskin with the big one.
- Fix some menuing sounds not playing underwater.

### tomb4 bug fixes

- Fix keyboard accelerators (ALT + key keyboard hotkeys).
- Fix joint lighting/rotation.
- Fix raising floor lighting in Tomb of Seth.
- Fix cursor in windowed mode.
- Fix the seperate option appearing on grenade gun and crashing.
- Fix block pulling test (Fixes the shelf in Sphinx Complex).
- Fix lightning effect corruption in Sacred Lake.
- Fix baddy death on monkeyswing.
- Fix object bound detection.
- Fix moveable blocks possibly wrongly blocking enemies.
- Fix fog bulb bound detection, stops fog from randomly disappearing.
- Fix shoot switch collision detection.
- Fix camera shaking strength.
- Fix VonCroy excess tilt while running.
- Fix Senet objects disappearing sometimes.
- Fix triggering track 061_in_the_pyramid_part_i.wav
- Fix not being able to have distance fog and volumetric fog in the same level.
- Fix crash on exit if sound disabled through setup.
- Fix dog not waking up.
- Fix PSX shadow not moving with Lara in some animations.
- Improve shadow disappearing on some tilted geometry.
- Fix possible matrix stack corruption.
- Fix crashing when reloading some saves.
- Fix enemy bar for horseman.
- Fix window creation.
- Fix skeleton behavior.
- Fix enemy jeep movement.
- Fix planet effect drawing.
- Fix possible issues with jeeps carrying baddies in train level.

### new tomb4 features

- Add new scaling system.
- Refactor bars to use the new scaling system, fixing overlap issues.
- Rework pickup display to use the new scaling system.
- Use floating points in sky drawing for more accurate positioning/movement.
- Rework Bike/Jeep speedometers to use the new scaling system, stops them from stretching in widescreen.
- Rework the main menu logo to use the new scaling system to stop stretching in widescreen.
- Implement a more accurate frame for PSX bars.
- Remove 16 bit texture bit support.
- Implement completely new manual (Non dx-reliant) drawing system for rooms and objects.
- Use the new scale system for lasersight sprite, making it visible and consistent on every resolution.
- Use proper PSX shadow sprite.
- Add customizable distance fog.
- Add lasersight to items cheat.


## [2.0.0](https://github.com/Trxyebeep/TOMB4/tree/V2.0.0) (Oct. 17 2022)

### Original bug fixes

- Fix window border.
- Fix pickups freezing in the corner during flybies.
- Fix a divide by 0 in FX fog code.
- Fix a rare crash when shattering objects due to an unintialized variable.
- Fix Lara saying "No" when interacting with the Plinths in Temple of Horus.
- Fix water sfx playing when Lara is shot.
- Fix enemies aiming higher than Lara during duck turn animations.
- Fix grenadegun unequip frame.
- Fix backdrop color in train level.
- Fix launching multiple instances hanging the game.
- Fix dynamic lights in the last level.

### tomb4 bug fixes

- Fix failing to use the sand in the element puzzle.
- Fix red color when looking up from water.
- Fix jeep smoke appearing at wrong times.
- Fix lara flying when dying on water surface.
- Fix enemy jeep sometimes disappearing when triggered.
- Fix bug where shotgun sometimes wouldn't down the skeletons.
- Fix deadlock when opening the pause menu while in binoculars.
- Fix multiple FMVs playback at the end of KV5.
- Fix issues with joint lighting.
- Fix savegames crashing.

### new tomb4 features

- Increase dynamic lights limit.
- Remove outdated mmx code.
- Decrease font size in the new options menu to fit more options.
- Upgrade input to DirectInput8.
- Raise internal memory limit from 5 to 15MBs.
- Make DOZY remove fire from Lara.
- Improve crouch tilting.
- Reposition the enemy health bar in lasersight.
- Add flyby skipper.
- Finish decompiling 100% of game code, make a new exe with the new icon.
- Remove the software renderer.
- Implement a new float matrix for smoother/more accurate animations and drawing.
- Implement reverb.


## [1.2.1](https://github.com/Trxyebeep/TOMB4/tree/V1.2.1) (May 28 2022)

### tomb4 bug fixes

- Fix exit game crash.
- Fix joystick key mapping crash.
- Fix joystick input.


## [1.2.0](https://github.com/Trxyebeep/TOMB4/tree/V1.2.0) (April 28 2022)

### Original bug fixes

- Fix weapon hotkeys allowing weapons while ducked, and underwater
- Fix problems with medipack hotkeys
- Fix Minotaur not dropping Lara when she climbs on two hands, fix softlock on other ladder
- Fix Lara going through the floor when opening underwater doors
- Fix loadscreens when reloading from underwater

### tomb4 bug fixes

- Fix loading text position
- Fix enemy jeep triggering
- Fix super close tri drawing (title issue)
- Fix Obelisk puzzle
- Fix wraith trail effect
- Fix softlock when getting on bike
- Fix some bike and jeep collision issues

### new tomb4 features

- Fix bike light to be more like PSX so it affects statics
- Add pages system to options menu
- Correct loadbar position for PSX and TR5 modes
- Make loading text customizable
- Customize the mono screen
- Add TR5 loading bar option
- Customize Lara look transparency
- Add ammo counter
- Add hotkeys to change ammo type
- Customize combat camera tilt
- Add option to show healthbar in the inventory
- Customize static lighting


## [1.1.1](https://github.com/Trxyebeep/TOMB4/tree/V1.1.1) (March 28 2022)

### Original bug fixes

- Fix fog affecting inventory items after saving.
- Fix Lara's position when interacting with the fullblock switch.

### tomb4 bug fixes

- Fix some Jeep and Bike collision behavior.
- Fix Guide/Scorpions being stuck in Tomb of Seth. (InitialiseRaisingBlock)
- Fix VonCroy not gesturing for Lara.

## [1.1.0](https://github.com/Trxyebeep/TOMB4/tree/V1.1.0) (March 26 2022)

### Original bug fixes

- Fix fog affecting bars.
- Fix shatter fragments not rotating.
- Fix wrong inventory/pause menu background on 32bit color mode.
- Fix wrong fog application of semitransparent faces.
- Fix water door sfx playing when Lara is shot by a baddie.

### tomb4 bug fixes

- Fix loading text overlapping with the bar.
- Fix crossbow bolts not hitting their targets.
- Fix bike not exploding from falling long heights.
- Fix flame bugs.
- Fix the crawl tilting menu option.

### new tomb4 features

- implement new mono screen to fix the white stripes.
- prevent flycheat when on vehicles.
- add more accurate PSX shadow option.
- Implement dynamic lighting for static meshes.


## [1.0.0](https://github.com/Trxyebeep/TOMB4/tree/V1.0.0) (March 13 2022)

### Original bug fixes

- Correct the Bike headlight position.
- Make bike light follow interpolated frames for smoother movement.
- Fix grenades not colliding with Lara.
- Disallow saving with F5 when Lara is dead.
- Fix objects lighting after flipmaps.
- Fix Lara's elbows wrong rotations when equipping guns/holding flares.
- Fix Desert Railroad's distance fog disappearing after saving and reloading.
- Make Hair follow interpolated frames for smoother movement.
- Reposition Lara's mesh spheres to fix the floating hair.
- Add Hair collision with Lara's neck.
- Fix stiff hair when wading.
- Add Hair collision with the floor.
- Remove the need to hold action during corner-shimmy animations.
- Remove the need to press duck in order to climb into crawl spaces.
- Remove multiple frame/animation number restrictions from Lara's moveset to enable more fluid movement
- Fix the Uzis and Revolver holstering.
- Fix flares going through the floor when thrown.
- Fix wrong alpha when drawing objects with multiple meshes in the inventory.
- Fix bugs with bridges.
- Fix crash when using the grenade launcher on inactive enemies.
- Add missing speech heads to the cutscene at the beginning of Race For The Iris (both paths).
- Fix Lara flashing random colors at the edges of shadow bulbs.
- Correct environment mapping on Gold Lara.
- Fix bike speed meter not showing when Fog is disabled.
- Fix bike speed meter colors and make it more PSX like.
- Fix Jeep speed meter colors and improve it.
- Fix meshswap frames in Seth's armor cutscene.
- Remove the amulet after Seth's armor cutscene.

### tomb4 additions

- Add GameOver menu at death.
- Add Discord Rich Presence.
- Add cutscene skipper.
- Add Footprints.
- Add Binoculars mesh and animation for Lara when looking at the mirror.
- Add subtractive blending mode to restore black smoke.
- Add enemy bars.
- Add tilting for Lara when crawling on sloped floors.
- Add cheats
- changed fly cheat to add flexibility.
- Add TR5 style bars.
- Add PSX style bars.
- Customizable bar positions.
- Add proper FOV calculations from TR2Main (Thank you Arsunt!)
- Add PSX "Loading" text to loading screens.
- Add circle shadow, and PSX style shadow.
- Move health bar to the center of the screen when aiming with the lasersight.
