# tomb4 changelog

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
