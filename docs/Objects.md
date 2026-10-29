# Object List {#objectlist}

This page lists each course and each implemented course object that appears on that track. Objects are listed in order of their @ref Kinoko::Field::ObjectId "ObjectId". Objects are created via the factory function @ref Kinoko::Field::ObjectDirector::createObject "ObjectDirector::createObject".

Objects that are not listed here are either blacklisted (they have no effect on Time Trial
physics) or fall back to @ref Kinoko::Field::ObjectNoImpl "ObjectNoImpl".

Some courses also construct manager classes that coordinate their objects; see @ref objectlist_managers "Course Managers" at the bottom of this page.

## Course Objects {#objectlist_courses}

<table class="object-grid">
<tr>
<td>

<h3>Luigi Circuit</h3>

|ID|Name|What|Class|
|---|---|---|---|
| `0x12E` | `dokan_sfc` | Green pipes | @ref Kinoko::Field::ObjectDokan "ObjectDokan" |

</td>
<td>

<h3>Moo Moo Meadows</h3>

|ID|Name|What|Class|
|---|---|---|---|
| `0x192` | `choropu` | Monty moles | @ref Kinoko::Field::ObjectChoropu "ObjectChoropu" |
| `0x193` | `cow` | A herd of cows | @ref Kinoko::Field::ObjectCowHerd "ObjectCowHerd" |

</td>
<td>

<h3>Mushroom Gorge</h3>

|ID|Name|What|Class|
|---|---|---|---|
| `0x142` | `kinokoT1` | Decorative mushrooms | @ref Kinoko::Field::ObjectCollidable "ObjectCollidable" |
| `0x191` | `kuribo` | Goomba | @ref Kinoko::Field::ObjectKuribo "ObjectKuribo" |
| `0x1F5` | `kinoko_ud` | Moving bouncy mushrooms | @ref Kinoko::Field::ObjectKinokoUd "ObjectKinokoUd" |
| `0x1F6` | `kinoko_bend` | Bouncy mushrooms oscillating up and down | @ref Kinoko::Field::ObjectKinokoBend "ObjectKinokoBend" |
| `0x1FA` | `kinoko_nm` | Green mushrooms | @ref Kinoko::Field::ObjectKinokoNm "ObjectKinokoNm" |

</td>
<td>

<h3>Toad's Factory</h3>

|ID|Name|What|Class|
|---|---|---|---|
| `0x0D3` | `w_woodbox` | Wooden box that follows a route | @ref Kinoko::Field::ObjectWoodboxW "ObjectWoodboxW" |
| `0x0D5` | `w_itemboxline` | Stompers and blocks after the first turn | @ref Kinoko::Field::ObjectItemboxLine "ObjectItemboxLine" |
| `0x0E3` | `bird` | Flock of flying birds | @ref Kinoko::Field::ObjectBird "ObjectBird" |
| `0x19D` | `Press` | Stompers | @ref Kinoko::Field::ObjectPress "ObjectPress" |
| `0x1F8` | `bulldozer_left` | Bulldozer | @ref Kinoko::Field::ObjectBulldozer "ObjectBulldozer" |
| `0x1F9` | `bulldozer_right` | Bulldozer | @ref Kinoko::Field::ObjectBulldozer "ObjectBulldozer" |
| `0x1FB` | `Crane` | Moving crane platform outside factory | @ref Kinoko::Field::ObjectCrane "ObjectCrane" |
| `0x25A` | `BeltEasy` | Conveyers outside the factory | @ref Kinoko::Field::ObjectBeltEasy "ObjectBeltEasy" |
| `0x25B` | `BeltCrossing` | First set of conveyer belts | @ref Kinoko::Field::ObjectBeltCrossing "ObjectBeltCrossing" |
| `0x25C` | `BeltCurveA` | Curved conveyer at end of factory | @ref Kinoko::Field::ObjectBeltCurveA "ObjectBeltCurveA" |

</td>
</tr>
<tr>
<td>

<h3>Mario Circuit</h3>

|ID|Name|What|Class|
|---|---|---|---|
| `0x066` | `DummyPole` | Solid pole | @ref Kinoko::Field::ObjectCollidable "ObjectCollidable" |
| `0x130` | `castletree1c` | Collidable tree | @ref Kinoko::Field::ObjectCollidable "ObjectCollidable" |
| `0x191` | `kuribo` | Goomba | @ref Kinoko::Field::ObjectKuribo "ObjectKuribo" |
| `0x196` | `wanwan` | Chain Chomp | @ref Kinoko::Field::ObjectWanwan "ObjectWanwan" |
| `0x217` | `Mdush` | Ramp near the Chain Chomp | @ref Kinoko::Field::ObjectKCL "ObjectKCL" |

</td>
<td>

<h3>Coconut Mall</h3>

|ID|Name|What|Class|
|---|---|---|---|
| `0x066` | `DummyPole` | Solid pole | @ref Kinoko::Field::ObjectCollidable "ObjectCollidable" |
| `0x0CC` | `CarA1` | Last moving car | @ref Kinoko::Field::ObjectCarA "ObjectCarA" |
| `0x0E7` | `CarA2` | Middle moving car | @ref Kinoko::Field::ObjectCarA "ObjectCarA" |
| `0x0E8` | `CarA3` | First moving car | @ref Kinoko::Field::ObjectCarA "ObjectCarA" |
| `0x145` | `PalmTree` | Palm trees | @ref Kinoko::Field::ObjectCollidable "ObjectCollidable" |
| `0x146` | `parasol` | Umbrellas | @ref Kinoko::Field::ObjectCollidable "ObjectCollidable" |
| `0x25E` | `escalator` | A single escalator | @ref Kinoko::Field::ObjectEscalator "ObjectEscalator" |
| `0x260` | `escalator_group` | Two escalators with a Pianta in the middle | @ref Kinoko::Field::ObjectEscalatorGroup "ObjectEscalatorGroup" |

</td>
<td>

<h3>DK Summit</h3>

|ID|Name|What|Class|
|---|---|---|---|
| `0x19C` | `heyho` | Snowboarding Shy Guy | @ref Kinoko::Field::ObjectHeyho "ObjectHeyho" |

</td>
<td>

<h3>Wario's Gold Mine</h3>

|ID|Name|What|Class|
|---|---|---|---|
| `0x0CD` | `basabasa` | Bat spawner | @ref Kinoko::Field::ObjectBasabasa "ObjectBasabasa" |
| `0x19B` | `TruckWagon` | Minecart spawner | @ref Kinoko::Field::ObjectTruckWagon "ObjectTruckWagon" |

</td>
</tr>
<tr>
<td>

<h3>Daisy Circuit</h3>

|ID|Name|What|Class|
|---|---|---|---|
| `0x066` | `DummyPole` | Solid pole | @ref Kinoko::Field::ObjectCollidable "ObjectCollidable" |
| `0x144` | `pylon01` | Traffic cone | @ref Kinoko::Field::ObjectPylon "ObjectPylon" |

</td>
<td>

<h3>Koopa Cape</h3>

|ID|Name|What|Class|
|---|---|---|---|
| `0x191` | `kuribo` | Goomba | @ref Kinoko::Field::ObjectKuribo "ObjectKuribo" |
| `0x1A6` | `Epropeller` | Rotating Koopa shell lasers | @ref Kinoko::Field::ObjectPropeller "ObjectPropeller" |

</td>
<td>

<h3>Maple Treeway</h3>

|ID|Name|What|Class|
|---|---|---|---|
| `0x0E2` | `hanachan` | Wigglers | @ref Kinoko::Field::ObjectHanachan "ObjectHanachan" |
| `0x20E` | `ami` | Swinging net | @ref Kinoko::Field::ObjectAmi "ObjectAmi" |

</td>
<td>

<h3>Grumble Volcano</h3>

|ID|Name|What|Class|
|---|---|---|---|
| `0x0D6` | `VolcanoBall1` | Falling fireball | <small>@ref Kinoko::Field::ObjectVolcanoBallLauncher "ObjectVolcanoBallLauncher"</small> |
| `0x1A8` | `FireSnake_v` | Bouncing fireballs | @ref Kinoko::Field::ObjectFireSnakeV "ObjectFireSnakeV" |
| `0x1F7` | `VolcanoRock1` | Moving platforms | @ref Kinoko::Field::ObjectVolcanoRock "ObjectVolcanoRock" |
| `0x1FC` | `VolcanoPiece` | Shaking and falling roads | @ref Kinoko::Field::ObjectVolcanoPiece "ObjectVolcanoPiece" |
| `0x212` | `FlamePole_v` | Fire geyser | @ref Kinoko::Field::ObjectFlamePoleV "ObjectFlamePoleV" |

</td>
</tr>
<tr>
<td>

<h3>Dry Dry Ruins</h3>

|ID|Name|What|Class|
|---|---|---|---|
| `0x0CD` | `basabasa` | Bat spawner | @ref Kinoko::Field::ObjectBasabasa "ObjectBasabasa" |
| `0x199` | `sanbo` | Pokey | @ref Kinoko::Field::ObjectSanbo "ObjectSanbo" |
| `0x208` | `dc_pillar` | Falling pillars | @ref Kinoko::Field::ObjectPillar "ObjectPillar" |
| `0x209` | `dc_sandcone` | Sand piles | @ref Kinoko::Field::ObjectSandcone "ObjectSandcone" |

</td>
<td>

<h3>Moonview Highway</h3>

|ID|Name|What|Class|
|---|---|---|---|
| `0x0D0` | `kart_truck` | Trucks | @ref Kinoko::Field::ObjectCarTGE "ObjectCarTGE" |
| `0x0D1` | `car_body` | Cars | @ref Kinoko::Field::ObjectCarTGE "ObjectCarTGE" |

@see @ref Kinoko::Field::ObjectHighwayManager "ObjectHighwayManager"

</td>
<td>

<h3>Bowser's Castle</h3>

|ID|Name|What|Class|
|---|---|---|---|
| `0x0CF` | `koopaBall` | Exploding fireball | @ref Kinoko::Field::ObjectKoopaBall "ObjectKoopaBall" |
| `0x0DB` | `dossunc` | Thwomps | @ref Kinoko::Field::ObjectDossunc "ObjectDossunc" |
| `0x1A1` | `WLFireringGC` | Rotating fire ring | @ref Kinoko::Field::ObjectFireRing "ObjectFireRing" |
| `0x1A5` | `koopaFirebar` | Rotating fire bars | @ref Kinoko::Field::ObjectFirebar "ObjectFirebar" |
| `0x1FD` | `FlamePole` | Geyser humps | @ref Kinoko::Field::ObjectFlamePoleFoot "ObjectFlamePoleFoot" |
| `0x1FE` | `TwistedWay` | Moving section at beginning of castle | @ref Kinoko::Field::ObjectTwistedWay "ObjectTwistedWay" |
| `0x212` | `FlamePole_v` | Fire geyser | @ref Kinoko::Field::ObjectFlamePoleV "ObjectFlamePoleV" |

</td>
<td>

<h3>Rainbow Road</h3>

|ID|Name|What|Class|
|---|---|---|---|
| `0x066` | `DummyPole` | Solid pole | @ref Kinoko::Field::ObjectCollidable "ObjectCollidable" |
| `0x204` | `aurora` | Wavy trickable road | @ref Kinoko::Field::ObjectAurora "ObjectAurora" |

</td>
</tr>
<tr>
<td>

<h3>GCN Peach Beach</h3>

|ID|Name|What|Class|
|---|---|---|---|
| `0x002` | `Psea` | Rising/falling water | @ref Kinoko::Field::ObjectPsea "ObjectPsea" |
| `0x138` | `peachtreeGCc` | Collidable trees | @ref Kinoko::Field::ObjectCollidable "ObjectCollidable" |
| `0x197` | `poihana` | Cataquacks | @ref Kinoko::Field::ObjectPoihana "ObjectPoihana" |

</td>
<td>

<h3>DS Yoshi Falls</h3>

None

</td>
<td>

<h3>SNES Ghost Valley 2</h3>

|ID|Name|What|Class|
|---|---|---|---|
| `0x13C` | `ObakeBlockSFCc` | Falling block | @ref Kinoko::Field::ObjectObakeBlock "ObjectObakeBlock" |
| `0x16F` | `ObakeBlock2SFCc` | Falling block | @ref Kinoko::Field::ObjectObakeBlock "ObjectObakeBlock" |
| `0x170` | `ObakeBlock3SFCc` | Falling block | @ref Kinoko::Field::ObjectObakeBlock "ObjectObakeBlock" |

@see @ref Kinoko::Field::ObjectObakeManager "ObjectObakeManager"

</td>
<td>

<h3>N64 Mario Raceway</h3>

|ID|Name|What|Class|
|---|---|---|---|
| `0x140` | `MarioGo64c` | The "GO" sign | @ref Kinoko::Field::ObjectCollidable "ObjectCollidable" |
| `0x1AA` | `puchi_pakkun` | Piranha Plants | @ref Kinoko::Field::ObjectDokan "ObjectPuchiPakkun" |

</td>
</tr>
<tr>
<td>

<h3>N64 Sherbet Land</h3>

|ID|Name|What|Class|
|---|---|---|---|
| `0x0D7` | `penguin_s` | Small sliding penguins | @ref Kinoko::Field::ObjectPenguinS "ObjectPenguinS" |
| `0x0D8` | `penguin_m` | Larger walking penguins | @ref Kinoko::Field::ObjectPenguin "ObjectPenguin" |

</td>
<td>

<h3>GBA Shy Guy Beach</h3>

|ID|Name|What|Class|
|---|---|---|---|
| `0x0E5` | `crab` | Crabs | @ref Kinoko::Field::ObjectCrab "ObjectCrab" |
| `0x0EA` | `HeyhoBallGBA` | Cannonballs | @ref Kinoko::Field::ObjectHeyhoBall "ObjectHeyhoBall" |
| `0x14A` | `HeyhoTreeGBAc` | Collidable trees | @ref Kinoko::Field::ObjectCollidable "ObjectCollidable" |
| `0x16E` | `r_parasol` | Umbrellas | @ref Kinoko::Field::ObjectParasolR "ObjectParasolR" |

@see @ref Kinoko::Field::ObjectHeyhoShipManager "ObjectHeyhoShipManager"

</td>
<td>

<h3>DS Delfino Square</h3>

|ID|Name|What|Class|
|---|---|---|---|
| `0x070` | `woodbox` | Wooden/steel boxes | @ref Kinoko::Field::ObjectWoodbox "ObjectWoodbox" |
| `0x1FF` | `TownBridgeDSc` | Drawbridge | @ref Kinoko::Field::ObjectTownBridge "ObjectTownBridge" |

</td>
<td>

<h3>GCN Waluigi Stadium</h3>

|ID|Name|What|Class|
|---|---|---|---|
| `0x0CB` | `WLwallGC` | Moving Piranha Plant wall | @ref Kinoko::Field::ObjectWLWallGC "ObjectWLWallGC" |
| `0x13F` | `WLdokanGC` | Giant Piranha Plant pipes | @ref Kinoko::Field::ObjectKCL "ObjectKCL" |
| `0x195` | `WLfirebarGC` | Rotating fire bars | @ref Kinoko::Field::ObjectFirebar "ObjectFirebar" |
| `0x1A1` | `WLFireringGC` | Rotating fire ring | @ref Kinoko::Field::ObjectFireRing "ObjectFireRing" |

</td>
</tr>
<tr>
<td>

<h3>DS Desert Hills</h3>

|ID|Name|What|Class|
|---|---|---|---|
| `0x072` | `sunDS` | The sun | @ref Kinoko::Field::ObjectSunDS "ObjectSunDS" |
| `0x199` | `sanbo` | Pokey | @ref Kinoko::Field::ObjectSanbo "ObjectSanbo" |
| `0x1A4` | `FireSnake` | Bouncing fireballs | @ref Kinoko::Field::ObjectFireSnake "ObjectFireSnake" |

@see @ref Kinoko::Field::ObjectSunManager "ObjectSunManager"

</td>
<td>

<h3>GBA Bowser Castle 3</h3>

|ID|Name|What|Class|
|---|---|---|---|
| `0x0DB` | `dossunc` | Thwomps | @ref Kinoko::Field::ObjectDossunc "ObjectDossunc" |
| `0x0DD` | `boble` | Lava bubbles | @ref Kinoko::Field::ObjectBoble "ObjectBoble" |

</td>
<td>

<h3>N64 DK's Jungle Parkway</h3>

|ID|Name|What|Class|
|---|---|---|---|
| `0x158` | `DKtreeA64c` | Collidable trees | @ref Kinoko::Field::ObjectCollidable "ObjectCollidable" |
| `0x15A` | `DKTreeB64c` | Collidable trees | @ref Kinoko::Field::ObjectCollidable "ObjectCollidable" |

</td>
<td>

<h3>GCN Mario Circuit</h3>

|ID|Name|What|Class|
|---|---|---|---|
| `0x134` | `marioTreeGCc` | Collidable trees | @ref Kinoko::Field::ObjectCollidable "ObjectCollidable" |
| `0x191` | `kuribo` | Goomba | @ref Kinoko::Field::ObjectKuribo "ObjectKuribo" |
| `0x194` | `pakkun_f` | Piranha Plants | @ref Kinoko::Field::ObjectPakkunF "ObjectPakkunF" |
| `0x196` | `wanwan` | Chain Chomp | @ref Kinoko::Field::ObjectWanwan "ObjectWanwan" |
| `0x1A2` | `pakkun_dokan` | Piranha Plant pipes | @ref Kinoko::Field::ObjectCollidable "ObjectCollidable" |

</td>
</tr>
<tr>
<td>

<h3>SNES Mario Circuit 3</h3>

|ID|Name|What|Class|
|---|---|---|---|
| `0x12E` | `dokan_sfc` | Green pipes | @ref Kinoko::Field::ObjectDokan "ObjectDokan" |
| `0x15D` | `oilSFC` | Oil slick | @ref Kinoko::Field::ObjectOilSFC "ObjectOilSFC" |

</td>
<td>

<h3>DS Peach Gardens</h3>

|ID|Name|What|Class|
|---|---|---|---|
| `0x066` | `DummyPole` | Solid pole | @ref Kinoko::Field::ObjectCollidable "ObjectCollidable" |
| `0x0E9` | `Hwanwan` | Walking Chain Chomps | <small>@ref Kinoko::Field::ObjectHwanwanManager "ObjectHwanwanManager"</small> |
| `0x19A` | `choropu2` | Monty moles | @ref Kinoko::Field::ObjectChoropu "ObjectChoropu" |

</td>
<td>

<h3>GCN DK Mountain</h3>

|ID|Name|What|Class|
|---|---|---|---|
| `0x0E3` | `bird` | Flock of flying birds | @ref Kinoko::Field::ObjectBird "ObjectBird" |
| `0x198` | `DKrockGC` | Falling/rolling rocks | @ref Kinoko::Field::ObjectRock "ObjectRock" |
| `0x202` | `DKturibashiGCc` | Wooden bridge | @ref Kinoko::Field::ObjectTuribashi "ObjectTuribashi" |

</td>
<td>

<h3>N64 Bowser's Castle</h3>

|ID|Name|What|Class|
|---|---|---|---|
| `0x0DB` | `dossunc` | Thwomps | @ref Kinoko::Field::ObjectDossunc "ObjectDossunc" |
| `0x18B` | `KoopaFigure64` | Bowser statues | @ref Kinoko::Field::ObjectKoopaFigure64 "ObjectKoopaFigure64" |
| `0x212` | `FlamePole_v` | Fire geyser | @ref Kinoko::Field::ObjectFlamePoleV "ObjectFlamePoleV" |
| `0x216` | `FlamePole_v_big` | Big fire geyser | @ref Kinoko::Field::ObjectFlamePoleV "ObjectFlamePoleV" |

</td>
</tr>
</table>

## Course Managers {#objectlist_managers}

These manager classes are constructed by @ref Kinoko::Field::ObjectDirector::createObjects "ObjectDirector::createObjects" depending on the course and the objects present.

|Course|Class|Purpose|
|---|---|---|
| Moonview Highway | @ref Kinoko::Field::ObjectHighwayManager "ObjectHighwayManager" | Synchronizes a squish cooldown across all cars and trucks |
| DS Desert Hills | @ref Kinoko::Field::ObjectSunManager "ObjectSunManager" | Coordinates @ref Kinoko::Field::ObjectSunDS "ObjectSunDS" throwing @ref Kinoko::Field::ObjectFireSnake "ObjectFireSnake" |
| GBA Shy Guy Beach | @ref Kinoko::Field::ObjectHeyhoShipManager "ObjectHeyhoShipManager" | Coordinates  @ref Kinoko::Field::ObjectHeyhoShip "ObjectHeyhoShip" shooting out @ref Kinoko::Field::ObjectHeyhoBall "ObjectHeyhoBall" |
| SNES Ghost Valley 2 | @ref Kinoko::Field::ObjectObakeManager "ObjectObakeManager" | Maintains a spatial cache of all blocks for faster collision queries |
