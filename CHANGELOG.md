# tomb4 changelog

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
