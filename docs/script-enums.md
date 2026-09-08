# Script enums
[Source](https://library.sannybuilder.com/assets/sa/enums.txt)

```cpp
enum class ePedType {
	PLAYER1 = 0,
	PLAYER2 = 1,
	PLAYER_NETWORK = 2,
	PLAYER_UNUSED = 3,
	CIV_MALE = 4,
	CIV_FEMALE = 5,
	COP = 6,
	GANG1 = 7,
	GANG2 = 8,
	GANG3 = 9,
	GANG4 = 10,
	GANG5 = 11,
	GANG6 = 12,
	GANG7 = 13,
	GANG8 = 14,
	GANG9 = 15,
	GANG10 = 16,
	DEALER = 17,
	EMERGENCY = 18,
	FIREMAN = 19,
	CRIMINAL = 20,
	BUM = 21,
	PROSTITUTE = 22,
	SPECIAL = 23,
	MISSION1 = 24,
	MISSION2 = 25,
	MISSION3 = 26,
	MISSION4 = 27,
	MISSION5 = 28,
	MISSION6 = 29,
	MISSION7 = 30,
	MISSION8 = 31,
};

enum class eWeaponType {
	UNARMED = 0,
	BRASS_KNUCKLES = 1,
	GOLF_CLUB = 2,
	NIGHT_STICK = 3,
	KNIFE = 4,
	BASEBALL_BAT = 5,
	SHOVEL = 6,
	POOL_CUE = 7,
	KATANA = 8,
	CHAINSAW = 9,
	DILDO1 = 10,
	DILDO2 = 11,
	VIBE1 = 12,
	VIBE2 = 13,
	FLOWERS = 14,
	CANE = 15,
	GRENADE = 16,
	TEARGAS = 17,
	MOLOTOV = 18,
	ROCKET = 19,
	ROCKET_HS = 20,
	FREEFALL_BOMB = 21,
	PISTOL = 22,
	PISTOL_SILENCED = 23,
	DESERT_EAGLE = 24,
	SHOTGUN = 25,
	SAWNOFF = 26,
	SPAS12 = 27,
	MICRO_UZI = 28,
	MP5 = 29,
	AK47 = 30,
	M4 = 31,
	TEC9 = 32,
	RIFLE = 33,
	SNIPER = 34,
	ROCKET_LAUNCHER = 35,
	ROCKET_LAUNCHER_HS = 36,
	FLAMETHROWER = 37,
	MINIGUN = 38,
	SATCHEL = 39,
	DETONATOR = 40,
	SPRAY_CAN = 41,
	EXTINGUISHER = 42,
	CAMERA = 43,
	NIGHT_VISION = 44,
	INFRARED = 45,
	PARACHUTE = 46,
	HEALTH = 47,
	ARMOUR = 48,
	RAMMEDBYCAR = 49,
	RUNOVERBYCAR = 50,
	EXPLOSION = 51,
	UZI_DRIVEBY = 52,
	DROWNING = 53,
	FALL = 54,
	UNIDENTIFIED = 55,
	ANY_MELEE = 56,
	ANY_WEAPON = 57,
};

enum class eTown {
	LS
	SF
	LV
};

enum class eFade {
	OUT = 0,
	IN = 1,
	NONE = 2,
};

enum class eBodyPart {
	Torso
	Head
	Legs
	Shoes
	LowerLeftArm
	UpperLeftArm
	LowerRightArm
	UpperRightArm
	UpperBack
	LeftChest
	RightChest
	Stomach
	LowerBack
	Necklace
	WatchOrBracelet
	Glasses
	Hat
	SpecialCostume
};

enum class eFileMode {
	READ_TEXT = "rt",
	WRITE_TEXT = "wt",
	APPEND_TEXT = "at",
	READ_BINARY = "rb",
	WRITE_BINARY = "wb",
	APPEND_BINARY = "ab",
};

enum class eSeekOrigin {
	BEGIN = 0,
	CURRENT = 1,
	}; = 2,
};

enum class eButton {
	LEFT_STICK_X = 0,
	LEFT_STICK_Y = 1,
	RIGHT_STICK_X = 2,
	RIGHT_STICK_Y = 3,
	LEFT_SHOULDER1 = 4,
	LEFT_SHOULDER2 = 5,
	RIGHT_SHOULDER1 = 6,
	RIGHT_SHOULDER2 = 7,
	DPAD_UP = 8,
	DPAD_DOWN = 9,
	DPAD_LEFT = 10,
	DPAD_RIGHT = 11,
	START = 12,
	SELECT = 13,
	SQUARE = 14,
	TRIANGLE = 15,
	CROSS = 16,
	CIRCLE = 17,
	LEFTSHOCK = 18,
	RIGHTSHOCK = 19,
};

enum class eAnimGroup {
	MAN = "man",
	SHUFFLE = "shuffle",
	OLD_MAN = "oldman",
	GANG1 = "gang1",
	GANG2 = "gang2",
	OLD_FATMAN = "oldfatman",
	FATMAN = "fatman",
	JOGGER = "jogger",
	DRUNKMAN = "drunkman",
	BLINDMAN = "blindman",
	SWAT = "swat",
	WOMAN = "woman",
	SHOPPING = "shopping",
	BUSY_WOMAN = "busywoman",
	SEXY_WOMAN = "sexywoman",
	PRO = "pro",
	OLD_WOMAN = "oldwoman",
	FAT_WOMAN = "fatwoman",
	JOG_WOMAN = "jogwoman",
	OLD_FAT_WOMAN = "oldfatwoman",
	SKATE = "skate",
};

enum class eTimerDirection {
	UP = 0,
	DOWN = 1,
};

enum class eFont {
	GOTHIC = 0,
	SUBTITLES = 1,
	MENU = 2,
	PRICEDOWN = 3,
};

enum class eTextStyle {
	MIDDLE = 1,
	BOTTOM_RIGHT = 2,
	WHITE_MIDDLE = 3,
	MIDDLE_SMALLER = 4,
	MIDDLE_SMALLER_HIGHER = 5,
	WHITE_MIDDLE_SMALLER = 6,
	LIGHT_BLUE_TOP = 7,
};

enum class eCounterDisplay {
	NUMBER = 0,
	BAR = 1,
};

enum class eCarMission {
	NONE = 0,
	CRUISE = 1,
	RAM_PLAYER_FARAWAY = 2,
	RAM_PLAYER_CLOSE = 3,
	BLOCK_PLAYER_FARAWAY = 4,
	BLOCK_PLAYER_CLOSE = 5,
	BLOCK_PLAYER_HANDBRAKE_STOP = 6,
	WAIT_FOR_DELETION = 7,
	GOTO_COORDS = 8,
	GOTO_COORDS_STRAIGHT = 9,
	EMERGENCY_VEHICLE_STOP = 10,
	STOP_FOREVER = 11,
	GOTO_COORDS_ACCURATE = 12,
	GOTO_COORDS_STRAIGHT_ACCURATE = 13,
	GOTO_COORDS_AS_THE_CROW_SWIMS = 14,
	RAM_CAR_FARAWAY = 15,
	RAM_CAR_CLOSE = 16,
	BLOCK_CAR_FARAWAY = 17,
	BLOCK_CAR_CLOSE = 18,
	BLOCK_CAR_HANDBRAKE_STOP = 19,
	HELI_FLY_TO_COORS = 20,
	BOAT_ATTACK_PLAYER = 21,
	PLANE_FLY_TO_COORS = 22,
	HELI_ATTACK_PLAYER = 23,
	SLOWLY_DRIVE_TOWARDS_PLAYER1 = 24,
	SLOWLY_DRIVE_TOWARDS_PLAYER2 = 25,
	BLOCK_PLAYER_FORWARD_AND_BACK = 26,
	PROTECTION_REAR = 27,
	PROTECTION_FRONT = 28,
	ESCORT_LEFT = 29,
	ESCORT_RIGHT = 30,
	ESCORT_REAR = 31,
	ESCORT_FRONT = 32,
	GO_TO_COORDS_RACING = 33,
	FOLLOW_RECORDED_PATH = 34,
	PLANE_ATTACK_PLAYER = 35,
	PLANE_FLY_IN_DIRECTION = 36,
	PLANE_FOLLOW_ENTITY = 37,
	HELI_FLY_IN_DIRECTION = 38,
	HELI_FOLLOW_ENTITY = 39,
	HELI_POLICE_BEHAVIOUR = 40,
	HELI_NEWS_BEHAVIOUR = 41,
	HELI_FLY_AWAY_FROM_PLAYER = 42,
	APPROACH_PLAYER_FARAWAY = 43,
	APPROACH_PLAYER_CLOSE = 44,
	PARK_PERPENDICULAR = 45,
	PARK_PARALLEL = 46,
	HELI_LAND = 47,
	HELI_LAND_TOUCHING_DOWN = 48,
	PARK_PERPENDICULAR2 = 49,
	PARK_PARALLEL2 = 50,
	HELI_KEEP_ENTITY_IN_VIEW = 51,
	FOLLOW_CAR_FARAWAY = 52,
	FOLLOW_CAR_CLOSE = 53,
	KILL_PED_FARAWAY = 54,
	KILL_PED_CLOSE = 55,
	HELI_ATTACK_PLAYER_FLY_AWAY = 56,
	PLANE_CRASH_AND_BURN = 57,
	HELI_CRASH_AND_BURN = 58,
	PLANE_ATTACK_PLAYER_POLICE = 59,
	DO_DRIVEBY_CLOSE = 60,
	DO_DRIVEBY_FARAWAY = 61,
	PLANE_DOG_FIGHT_ENTITY = 62,
	PLANE_DOG_FIGHT_PLAYER = 63,
	BOAT_CIRCLE_PLAYER = 64,
	ESCORT_LEFT_FARAWAY = 65,
	ESCORT_RIGHT_FARAWAY = 66,
	ESCORT_REAR_FARAWAY = 67,
	ESCORT_FRONT_FARAWAY = 68,
	MISSION45 = 69,
	MISSION46 = 70,
	MISSION47 = 71,
	MISSION48 = 72,
	MISSION49 = 73,
	MISSION4A = 74,
	MISSION4B = 75,
	MISSION4C = 76,
	MISSION4D = 77,
	MISSION4E = 78,
	MISSION4F = 79,
	MISSION50 = 80,
};

enum class eFlareType {
	NONE = 0,
	SUN = 1,
	HEADLIGHTS = 2,
};

enum class eCameraMode {
	NONE = 0,
	TOP_DOWN = 1,
	GTA_CLASSIC = 2,
	BEHIND_CAR = 3,
	FOLLOW_PED = 4,
	AIMING = 5,
	DEBUG = 6,
	SNIPER = 7,
	ROCKET_LAUNCHER = 8,
	MODEL_VIEW = 9,
	BILL = 10,
	SYPHON = 11,
	CIRCLE = 12,
	CHEEZY_ZOOM = 13,
	WHEEL_CAM = 14,
	FIXED = 15,
	FIRST_PERSON = 16,
	FLYBY = 17,
	CAM_ON_A_STRING = 18,
	REACTION = 19,
	FOLLOW_PED_WITH_BIND = 20,
	CHRIS = 21,
	BEHIND_BOAT = 22,
	PLAYER_FALLEN_WATER = 23,
	CAM_ON_TRAIN_ROOF = 24,
	CAM_RUNNING_SIDE_TRAIN = 25,
	BLOOD_ON_THE_TRACKS = 26,
	IM_THE_PASSENGER_WOO_WOO = 27,
	SYPHON_CRIM_IN_FRONT = 28,
	PED_DEAD_BABY = 29,
	PILLOWS_PAPS = 30,
	LOOK_AT_CARS = 31,
	ARREST_CAM_ONE = 32,
	ARREST_CAM_TWO = 33,
	M16_FIRST_PERSON = 34,
	SPECIAL_FIXED_FOR_SYPHON = 35,
	FIGHT_CAM = 36,
	TOP_DOWN_PED = 37,
	LIGHTHOUSE = 38,
	SNIPER_RUNABOUT = 39,
	ROCKET_LAUNCHER_RUNABOUT = 40,
	FIRST_PERSON_RUNABOUT = 41,
	M16_FIRST_PERSON_RUNABOUT = 42,
	FIGHT_CAM_RUNABOUT = 43,
	EDITOR = 44,
	HELICANNON_FIRST_PERSON = 45,
	CAMERA = 46,
	ATTACH_CAM = 47,
	TWO_PLAYER = 48,
	TWO_PLAYER_IN_CAR_AND_SHOOTING = 49,
	TWO_PLAYER_SEPARATE_CARS = 50,
	ROCKET_LAUNCHER_HS = 51,
	ROCKET_LAUNCHER_RUNABOUT_HS = 52,
	AIM_WEAPON = 53,
	TWO_PLAYER_SEPARATE_CARS_TOP_DOWN = 54,
	AIM_WEAPON_FROM_CAR = 55,
	DW_HELI_CHASE = 56,
	DW_CAM_MAN = 57,
	DW_BIRDY = 58,
	DW_PLANE_SPOTTER = 59,
	DW_DOG_FIGHT = 60,
	DW_FISH = 61,
	DW_PLANE_CAM1 = 62,
	DW_PLANE_CAM2 = 63,
	DW_PLANE_CAM3 = 64,
	AIM_WEAPON_ATTACHED = 65,
};

enum class eHudObject {
	NONE = -1,
	Weapon
	Clock
	Score
	Armor
	Health
	Wanted
	ZoneName
	CarName
	Radar
	Blip
	Breath
	BREATH_BAR = 10,
};

enum class eExplosionType {
	GRENADE = 0,
	MOLOTOV = 1,
	ROCKET = 2,
	WEAK_ROCKET = 3,
	CAR = 4,
	QUICK_CAR = 5,
	BOAT = 6,
	AIRCRAFT = 7,
	MINE = 8,
	OBJECT = 9,
	TANK_FIRE = 10,
	SMALL = 11,
	RC_VEHICLE = 12,
};

enum class eRelationshipType {
	RESPECT = 0,
	LIKE = 1,
	NEUTRAL = 2,
	DISLIKE = 3,
	HATE = 4,
};

enum class eSwitchType {
	NONE = 0,
	INTERPOLATION = 1,
	JUMP_CUT = 2,
};

enum class eBlipDisplay {
	NEITHER = 0,
	MARKER_ONLY = 1,
	BLIP_ONLY = 2,
	BOTH = 3,
};

enum class eShadowTextureType {
	NONE = 0,
	CAR = 1,
	PED = 2,
	EXPLOSION = 3,
	HELI = 4,
	HEADLIGHTS = 5,
	BLOOD = 6,
};

enum class eBlipColor {
	RED = 0,
	GREEN = 1,
	BLUE = 2,
	WHITE = 3,
	YELLOW = 4,
	PURPLE = 5,
	CYAN = 6,
	THREAT = 7,
	DESTINATION = 8,
	BLUE_FIX = 337234175,
	PURPLE_FIX = -939472641,
	CYAN_FIX = 13172735,
};

enum class eTempAction {
	NONE = 0,
	WAIT = 1,
	REVERSE = 3,
	HANDBRAKE_TURN_LEFT = 4,
	HANDBRAKE_TURN_RIGHT = 5,
	HANDBRAKE_STRAIGHT = 6,
	TURN_LEFT = 7,
	TURN_RIGHT = 8,
	GO_FORWARD = 9,
	SWERVE_LEFT = 10,
	SWERVE_RIGHT = 11,
	STUCK_IN_TRAFFIC = 12,
	REVERSE_LEFT = 13,
	REVERSE_RIGHT = 14,
	PLANE_FLY_UP = 15,
	PLANE_FLY_STRAIGHT = 16,
	PLANE_SHARP_LEFT = 17,
	PLANE_SHARP_RIGHT = 18,
	HEAD_ON_COLLISION = 19,
	SWERVE_LEFT_STOP = 20,
	SWERVE_RIGHT_STOP = 21,
	REVERSE_STRAIGHT = 22,
	BOOST_USE_STEERING_ANGLE = 23,
};

enum class eCarDoor {
	HOOD = 0,
	TRUNK = 1,
	LEFT_FRONT_DOOR = 2,
	RIGHT_FRONT_DOOR = 3,
	LEFT_REAR_DOOR = 4,
	RIGHT_REAR_DOOR = 5,
};

enum class eMissionAudioSlot {
	SLOT1 = 1,
	SLOT2 = 2,
	SLOT3 = 3,
	SLOT4 = 4,
};

enum class eWeatherType {
	EXTRA_SUNNY_LA = 0,
	SUNNY_LA = 1,
	EXTRA_SUNNY_SMOG_LA = 2,
	SUNNY_SMOG_LA = 3,
	CLOUDY_LA = 4,
	SUNNY_SF = 5,
	EXTRA_SUNNY_SF = 6,
	CLOUDY_SF = 7,
	RAINY_SF = 8,
	FOGGY_SF = 9,
	SUNNY_VEGAS = 10,
	EXTRA_SUNNY_VEGAS = 11,
	CLOUDY_VEGAS = 12,
	EXTRA_SUNNY_COUNTRYSIDE = 13,
	SUNNY_COUNTRYSIDE = 14,
	CLOUDY_COUNTRYSIDE = 15,
	RAINY_COUNTRYSIDE = 16,
	EXTRA_SUNNY_DESERT = 17,
	SUNNY_DESERT = 18,
	SANDSTORM_DESERT = 19,
	UNDERWATER = 20,
	EXTRA_COLOURS1 = 21,
	EXTRA_COLOURS2 = 22,
};

enum class eScriptSound {
	SOUND_DISABLE_HELI_AUDIO = 1000,
	SoundEnableHeliAudio
	SoundCeilingVentLand
	SoundWarehouseDoorSlideStart
	SoundWarehouseDoorSlideStop
	SoundClaxonStart
	SoundClaxonStop
	SoundBlastDoorSlideStart
	SoundBlastDoorSlideStop
	SoundBonnetDent
	SoundBasketballBounce
	SoundBasketballHitHoop
	SoundBasketballScore
	SoundPoolBreak
	SoundPoolHitWhite
	SoundPoolBallHitBall
	SoundPoolHitCushion
	SoundPoolBallPot
	SoundPoolChalkCue
	SoundCraneEnter
	SoundCraneMoveStart
	SoundCraneMoveStop
	SoundCraneExit
	SoundCraneSmashPortacabin
	SoundContainerCollision
	SoundVideoPokerPayout
	SoundVideoPokerButton
	SoundWheelOfFortuneClacker
	SoundKeypadBeep
	SoundKeypadPass
	SoundKeypadFail
	SoundShootingRangeTargetShatter
	SoundShootingRangeTargetDrop
	SoundShootingRangeTargetMoveStart
	SoundShootingRangeTargetMoveStop
	SoundShutterDoorStart
	SoundShutterDoorStop
	SoundFreefallStart
	SoundFreefallStop
	SoundParachuteOpen
	SoundParachuteCollapse
	SoundDualShoot
	SoundDualThrust
	SoundDualExplosionShort
	SoundDualExplosionLong
	SoundDualMenuSelect
	SoundDualMenuDeselect
	SoundDualGameOver
	SoundDualPickupLight
	SoundDualPickupDark
	SoundDualTouchDark
	SoundDualTouchLight
	SoundAmmunationBuyWeapon
	SoundAmmunationBuyWeaponDenied
	SoundShopBuy
	SoundShopBuyDenied
	SoundRace321
	SoundRaceGo
	SoundPartMissionComplete
	SoundGogoPlayerFire
	SoundGogoEnemyFire
	SoundGogoExplosion
	SoundGogoTrackStart
	SoundGogoTrackStop
	SoundGogoSelect
	SoundGogoAccept
	SoundGogoDecline
	SoundGogoGameOver
	SoundDualTrackStart
	SoundDualTrackStop
	SoundBeeZap
	SoundBeePickup
	SoundBeeDrop
	SoundBeeSelect
	SoundBeeAccept
	SoundBeeDecline
	SoundBeeTrackStart
	SoundBeeTrackStop
	SoundBeeGameOver
	SoundFreezerOpen
	SoundFreezerClose
	SoundMeatTrackStart
	SoundMeatTrackStop
	SoundRouletteAddCash
	SoundRouletteRemoveCash
	SoundRouletteNoCash
	SoundRouletteSpin
	SoundBanditInsertCoin
	SoundBanditWheelStop
	SoundBanditWheelStart
	SoundBanditPayout
	SoundOfficeFireAlarmStart
	SoundOfficeFireAlarmStop
	SoundOfficeFireCoughingStart
	SoundOfficeFireCoughingStop
	SoundBikePackerClunk
	SoundBikeGangWheelSpin
	SoundAwardTrackStart
	SoundAwardTrackStop
	SoundMolotov
	SoundMeshGateOpenStart
	SoundMeshGateOpenStop
	SoundOglocDoorbell
	SoundOglocWindowRattleBang
	SoundStingerReload
	SoundHeavyDoorStart
	SoundHeavyDoorStop
	SoundShootControls
	SoundCargoPlaneDoorStart
	SoundCargoPlaneDoorStop
	SoundDaNangContainerOpen
	SoundDaNangHeavyDoorOpen
	SoundDaNangMuffledRefugees
	SoundGymBikeStart
	SoundGymBikeStop
	SoundGymBoxingBell
	SoundGymIncreaseDifficulty
	SoundGymRestWeights
	SoundGymRunningMachineStart
	SoundGymRunningMachineStop
	SoundOtbBetZero
	SoundOtbIncreaseBet
	SoundOtbLose
	SoundOtbPlaceBet
	SoundOtbWin
	SoundStingerFire
	SoundHeavyGateStart
	SoundHeavyGateStop
	SoundVerticalBirdLiftStart
	SoundVerticalBirdLiftStop
	SoundPunchPed
	SoundAmmunationGunCollision
	SoundCameraShot
	SoundBuyCarMod
	SoundBuyCarRespray
	SoundBaseballBatHitPed
	SoundStampPed
	SoundCheckpointAmber
	SoundCheckpointGreen
	SoundCheckpointRed
	SoundCarSmashCar
	SoundCarSmashGate
	SoundOtbTrackStart
	SoundOtbTrackStop
	SoundPedHitWaterSplash
	SoundRestaurantTrayCollision
	SoundPickupCrate
	SoundSweetsHorn
	SoundMagnetVehicleCollision
	SoundPropertyPurchased
	SoundPickupStandard
	SoundMechanicSlideOut
	SoundMechanicAttachCarBomb
	SoundGarageDoorStart
	SoundGarageDoorStop
	SoundCat2SecurityAlarm
	SoundCat2WoodenDoorBreach
	SoundMinitankFire
	SoundOtbNoCash
	SoundExplosion
	SoundRouletteBallBouncing
	SoundVerticalBirdAlarmStart
	SoundVerticalBirdAlarmStop
	SoundPedCollapse
	SoundAirHorn
	SoundShutterDoorSlowStart
	SoundShutterDoorSlowStop
	SoundBeeBuzz
	SoundRestaurantCjEat
	SoundRestaurantCjPuke
	SoundTempestPlayerShoot
	SoundTempestEnemyShoot
	SoundTempestExplosion
	SoundTempestPickup1
	SoundTempestPickup2
	SoundTempestPickup3
	SoundTempestWarp
	SoundTempestShieldGlow
	SoundTempestGameOver
	SoundTempestHighlight
	SoundTempestSelect
	SoundTempestTrackStart
	SoundTempestTrackStop
	SoundDrivingAwardTrackStart
	SoundDrivingAwardTrackStop
	SoundBikeAwardTrackStart
	SoundBikeAwardTrackStop
	SoundPilotAwardTrackStart
	SoundPilotAwardTrackStop
	SoundPedDeathCrunch
	SOUND_BANK_VIDEO_POKER = 1190,
};

enum class eFightStyle {
	DEFAULT = 4,
	Boxeo
	KungFu
	KickBoxing
	Bat
	Knife
	GolfClub
	Katana
	Chainsaw
	Dildo
	NotCombos
	YesCombos
	Elbows
};

enum class eMoveState {
	None
	Still
	TurnLeft
	TurnRight
	Walk
	Jog
	Run
	Sprint
};

enum class eModSlot {
	HOOD = 0,
	VENTS = 1,
	SPOILERS = 2,
	SIDE_SKIRTS = 3,
	FRONT_BULLBARS = 4,
	REAR_BULLBARS = 5,
	LIGHTS = 6,
	ROOF = 7,
	NITRO = 8,
	HYDRAULICS = 9,
	CAR_STEREO = 10,
	P11 = 11,
	WHEELS = 12,
	EXHAUSTS = 13,
	FRONT_BUMPER = 14,
	REAR_BUMPER = 15,
	P16 = 16,
};

enum class eCarLock {
	NOT_USED = 0,
	UNLOCKED = 1,
	LOCKED = 2,
	LOCKOUT_PLAYER_ONLY = 3,
	LOCKED_PLAYER_INSIDE = 4,
	LOCKED_INITIALLY = 5,
	FORCE_SHUT_DOORS = 6,
	SKIP_SHUT_DOORS = 7,
};

enum class ePickupType {
	NONE = 0,
	IN_SHOP = 1,
	ON_STREET = 2,
	ONCE = 3,
	ONCE_TIMEOUT = 4,
	ONCE_TIMEOUT_SLOW = 5,
	COLLECTABLE1 = 6,
	IN_SHOP_OUT_OF_STOCK = 7,
	MONEY = 8,
	MINE_INACTIVE = 9,
	MINE_ARMED = 10,
	NAUTICAL_MINE_INACTIVE = 11,
	NAUTICAL_MINE_ARMED = 12,
	FLOATING_PACKAGE = 13,
	FLOATING_PACKAGE_FLOATING = 14,
	ON_STREET_SLOW = 15,
	ASSET_REVENUE = 16,
	PROPERTY_LOCKED = 17,
	PROPERTY_FOR_SALE = 18,
	MONEY_DOESNTDISAPPEAR = 19,
	SNAPSHOT = 20,
	SECOND_PLAYER = 21,
	ONCE_FOR_MISSION = 22,
};

enum class eEntityStatus {
	Player
	PlayerPlaybackFromBuffer
	Simple
	Physics
	Abandoned
	Wrecked
	TrainMoving
	TrainNotMoving
	Heli
	Plane
	PlayerRemote
	PlayerDisabled
};

enum class eRadarSprite {
	NONE = 0,
	WHITE = 1,
	CENTRE = 2,
	MAP_HERE = 3,
	NORTH = 4,
	AIRYARD = 5,
	AMMUGUN = 6,
	BARBERS = 7,
	BIG_SMOKE = 8,
	BOAT_YARD = 9,
	BURGER_SHOT = 10,
	BULLDOZER = 11,
	CATALINA_PINK = 12,
	CESAR_VIAPANDO = 13,
	CHICKEN = 14,
	CJ = 15,
	CRASH1 = 16,
	DINER = 17,
	EMMET_GUN = 18,
	ENEMY_ATTACK = 19,
	FIRE = 20,
	GIRLFRIEND = 21,
	HOSPITAL = 22,
	LOGO_SYNDICATE = 23,
	MAD_DOG = 24,
	MAFIA_CASINO = 25,
	MC_STRAP = 26,
	MOD_GARAGE = 27,
	OG_LOC = 28,
	PIZZA = 29,
	POLICE = 30,
	PROPERTY_G = 31,
	PROPERTY_R = 32,
	RACE = 33,
	RYDER = 34,
	SAVE_GAME = 35,
	SCHOOL = 36,
	QMARK = 37,
	SWEET = 38,
	TATTOO = 39,
	THE_TRUTH = 40,
	WAYPOINT = 41,
	TORENO_RANCH = 42,
	TRIADS = 43,
	TRIADS_CASINO = 44,
	TSHIRT = 45,
	WOOZIE = 46,
	ZERO = 47,
	DATE_DISCO = 48,
	DATE_DRINK = 49,
	DATE_FOOD = 50,
	TRUCK = 51,
	CASH = 52,
	FLAG = 53,
	GYM = 54,
	IMPOUND = 55,
	LIGHT = 56,
	RUNWAY = 57,
	GANG_B = 58,
	GANG_P = 59,
	GANG_Y = 60,
	GANG_N = 61,
	GANG_G = 62,
	SPRAY = 63,
};

enum class eGarageName {
	AMUMIS = "amumis",
	BEACSV = "beacsv",
	BLOB1 = "blob1",
	BLOB2 = "blob2",
	BLOB6 = "blob6",
	BLOB69 = "blob69",
	BLOB7 = "blob7",
	BOD_L_AW_N = "bodLAwN",
	BRG_SFSE = "brgSFSE",
	BURBDO2 = "burbdo2",
	BURBDOO = "burbdoo",
	BURG_LK = "burg_lk",
	CARLAS1 = "carlas1",
	C_ESAFE1 = "CEsafe1",
	C_ESPRAY = "CEspray",
	CJSAFE = "cjsafe",
	CN2_GAR1 = "cn2gar1",
	CN2_SPRY = "CN2spry",
	C_NSPRAY = "CNspray",
	DHANGAR = "dhangar",
	DUF_LAS = "duf_LAS",
	FDORSFE = "fdorsfe",
	GHOSTDR = "ghostdr",
	HBGD_SFS = "hbgdSFS",
	IMP_LA = "imp_la",
	IMP_SF = "imp_sf",
	IMP_LV = "imp_lv",
	LASBOMB = "lasbomb",
	LCK_SFSE = "LCKSfse",
	MDS1_SFS = "mds1SFS",
	MDS_SF_SE = "mdsSFSe",
	MICHDR = "michdr",
	MODG_LAS = "modgLAS",
	MUL_LAN = "mul_lan",
	MODLAST = "modlast",
	SAV1_SFE = "sav1sfe",
	SAV1_SFW = "sav1sfw",
	SPR_L_AE = "sprLAe",
	SP_L_AW2 = "spLAw2",
	SPRSFSE = "sprsfse",
	SPRSFW = "sprsfw",
	SVGSFS1 = "svgsfs1",
	TBON = "tbon",
	TIMY1 = "timy1",
	V_ECMOD = "vEcmod",
	V_ESVGRG = "vEsvgrg",
	VG_ELOCK = "vgElock",
	VGSHNGR = "vgshngr",
	CN2_GAR2 = "cn2gar2",
};

enum class eGangType {
	BALLAS = 0,
	GROVE = 1,
	VAGOS = 2,
	RIFA = 3,
	DA_NANG_BOYS = 4,
	MAFIA = 5,
	TRIAD = 6,
	AZTECAS = 7,
	GANG9 = 8,
	GANG10 = 9,
};

enum class eWeaponSlot {
	UNARMED = 0,
	MELEE = 1,
	HANDGUN = 2,
	SHOTGUN = 3,
	SMG = 4,
	RIFLE = 5,
	SNIPER = 6,
	HEAVY = 7,
	THROWN = 8,
	SPECIAL = 9,
	GIFT = 10,
	PARACHUTE = 11,
	DETONATOR = 12,
};

enum class eGarageType {
	NONE = 0,
	MISSION = 1,
	BOMB_SHOP1 = 2,
	BOMB_SHOP2 = 3,
	BOMB_SHOP3 = 4,
	RESPRAY = 5,
	FOR_SCRIPT_TO_OPEN = 15,
	HIDEOUT_ONE = 16,
	HIDEOUT_TWO = 17,
	HIDEOUT_THREE = 18,
	FOR_SCRIPT_TO_OPEN_AND_CLOSE = 19,
	HIDEOUT_FOUR = 24,
	HIDEOUT_FIVE = 25,
	HIDEOUT_SIX = 26,
	HIDEOUT_SEVEN = 27,
	HIDEOUT_EIGHT = 28,
	HIDEOUT_NINE = 29,
	HIDEOUT_TEN = 30,
	HIDEOUT_ELEVEN = 31,
	HIDEOUT_TWELVE = 32,
	IMPOUND_LS = 33,
	IMPOUND_SF = 34,
	IMPOUND_LV = 35,
	LOCO_LOW_CO = 36,
	WHEEL_ARCH_ANGELS = 37,
	TRANSFENDER = 38,
	HIDEOUT_THIRTEEN = 39,
	HIDEOUT_FOURTEEN = 40,
	HIDEOUT_FIFTEEN = 41,
	HIDEOUT_SIXTEEN = 42,
	BURGLARY = 43,
	AT400_HANGAR = 44,
	VERDANT_MEADOWS_HANGAR = 45,
};

enum class eBombType {
	NONE = 0,
	TIMED = 1,
	ON_IGNITION = 2,
	REMOTE = 3,
	TIMED_ACTIVE = 4,
	ON_IGNITION_ACTIVE = 5,
};

enum class eRadioChannel {
	NO_RADIO = -1,
	PLAYBACK_FM = 0,
	K_ROSE = 1,
	KDST = 2,
	BOUNCE_FM = 3,
	SFUR = 4,
	RADIO_LOS_SANTOS = 5,
	RADIO_X = 6,
	CSR = 7,
	K_JAH_WEST = 8,
	MASTER_SOUNDS = 9,
	WCTR = 10,
	USER_TRACKS = 11,
	OFF = 12,
	NONE = 12,
};

enum class eCoronaType {
	NORMAL = 0,
	Star
	Moon
	Reflect
	Headlight
	None
	RING = 9,
};

enum class ePlayerMood {
	Calm
	PissedOff
	Angry
	Wisecracking
};

enum class eEntryexitsFlag {
	RUBBISH = 1,
	OLDSTYLE = 2,
	LINKED = 4,
	SHOP = 8,
	SHOPEXIT = 16,
	CAR_WARP = 32,
	BIKE_WARP = 64,
	MIRROR = 128,
	WARP_GROUP = 256,
	NO_WARP = 512,
	RANDOM_OPENTIME = 1024,
	DONT_WARP_PED = 2048,
	BURGLARY = 4096,
	IN_USE = 8192,
	ENABLED = 16384,
	TEMPORARY = 32768,
};

enum class eDefaultTaskAllocator {
	FOLLOW_ANY_MEANS = 0,
	FOLLOW_LIMITED = 1,
	STAND_STILL = 2,
	CHAT = 3,
	SIT_IN_LEADER_CAR = 4,
	RANDOM = 5,
};

enum class eKeyCode {
	MIN = 1,
	MAX = 254,
	NONE = -1,
	LEFT_BUTTON = 1,
	RIGHT_BUTTON = 2,
	CANCEL = 3,
	MIDDLE_BUTTON = 4,
	CUSTOM_BUTTON1 = 5,
	CUSTOM_BUTTON2 = 6,
	BACK = 8,
	TAB = 9,
	CLEAR = 12,
	RETURN = 13,
	SHIFT = 16,
	CTRL = 17,
	ALT = 18,
	PAUSE = 19,
	CAPITAL = 20,
	KANA = 21,
	JUNJA = 23,
	FINAL = 24,
	KANJI = 25,
	ESCAPE = 27,
	CONVERT = 28,
	NON_CONVERT = 29,
	ACCEPT = 30,
	MODE_CHANGE = 31,
	SPACE = 32,
	PRIOR = 33,
	NEXT = 34,
	}; = 35,
	HOME = 36,
	LEFT = 37,
	UP = 38,
	RIGHT = 39,
	DOWN = 40,
	SELECT = 41,
	PRINT = 42,
	EXECUTE = 43,
	SNAPSHOT = 44,
	INSERT = 45,
	DELETE = 46,
	HELP = 47,
	NUM0 = 48,
	NUM1 = 49,
	NUM2 = 50,
	NUM3 = 51,
	NUM4 = 52,
	NUM5 = 53,
	NUM6 = 54,
	NUM7 = 55,
	NUM8 = 56,
	NUM9 = 57,
	A = 65,
	B = 66,
	C = 67,
	D = 68,
	E = 69,
	F = 70,
	G = 71,
	H = 72,
	I = 73,
	J = 74,
	K = 75,
	L = 76,
	M = 77,
	N = 78,
	O = 79,
	P = 80,
	Q = 81,
	R = 82,
	S = 83,
	T = 84,
	U = 85,
	V = 86,
	W = 87,
	X = 88,
	Y = 89,
	Z = 90,
	LEFT_WIN = 91,
	RIGHT_WIN = 92,
	APPS = 93,
	SLEEP = 95,
	NUM_PAD0 = 96,
	NUM_PAD1 = 97,
	NUM_PAD2 = 98,
	NUM_PAD3 = 99,
	NUM_PAD4 = 100,
	NUM_PAD5 = 101,
	NUM_PAD6 = 102,
	NUM_PAD7 = 103,
	NUM_PAD8 = 104,
	NUM_PAD9 = 105,
	MULTIPLY = 106,
	ADD = 107,
	SEPARATOR = 108,
	SUBTRACT = 109,
	DECIMAL = 110,
	DIVIDE = 111,
	F1 = 112,
	F2 = 113,
	F3 = 114,
	F4 = 115,
	F5 = 116,
	F6 = 117,
	F7 = 118,
	F8 = 119,
	F9 = 120,
	F10 = 121,
	F11 = 122,
	F12 = 123,
	F13 = 124,
	F14 = 125,
	F15 = 126,
	F16 = 127,
	F17 = 128,
	F18 = 129,
	F19 = 130,
	F20 = 131,
	F21 = 132,
	F22 = 133,
	F23 = 134,
	F24 = 135,
	NUM_LOCK = 144,
	SCROLL = 145,
	OEM_FJJISHO = 146,
	OEM_FJMASSHOU = 147,
	OEM_FJTOUROKU = 148,
	OEM_FJLOYA = 149,
	OEM_FJROYA = 150,
	LEFT_SHIFT = 160,
	RIGHT_SHIFT = 161,
	LEFT_CONTROL = 162,
	RIGHT_CONTROL = 163,
	LEFT_MENU = 164,
	RIGHT_MENU = 165,
	BROWSER_BACK = 166,
	BROWSER_FORWARD = 167,
	BROWSER_REFRESH = 168,
	BROWSER_STOP = 169,
	BROWSER_SEARCH = 170,
	BROWSER_FAVORITES = 171,
	BROWSER_HOME = 172,
	VOLUME_MUTE = 173,
	VOLUME_DOWN = 174,
	VOLUME_UP = 175,
	MEDIA_NEXT_TRACK = 176,
	MEDIA_PREV_TRACK = 177,
	MEDIA_STOP = 178,
	MEDIA_PLAYPAUSE = 179,
	LAUNCH_MAIL = 180,
	LAUNCH_MEDIASELECT = 181,
	LAUNCH_APP1 = 182,
	LAUNCH_APP2 = 183,
	OEM1 = 186,
	OEM_PLUS = 187,
	OEM_COMMA = 188,
	OEM_MINUS = 189,
	OEM_PERIOD = 190,
	OEM2 = 191,
	OEM3 = 192,
	ABNTC1 = 193,
	ABNTC2 = 194,
	OEM4 = 219,
	OEM5 = 220,
	OEM6 = 221,
	OEM7 = 222,
	OEM8 = 223,
	OEM_AX = 225,
	OEM102 = 226,
	ICO_HELP = 227,
	ICO00 = 228,
	PROCESSKEY = 229,
	ICOCLEAR = 230,
	PACKET = 231,
	OEM_RESET = 233,
	OEM_JUMP = 234,
	OEM_PA1 = 235,
	OEM_PA2 = 236,
	OEM_PA3 = 237,
	OEM_WSCTRL = 238,
	OEM_CUSEL = 239,
	OEM_ATTN = 240,
	OEM_FINISH = 241,
	OEM_COPY = 242,
	OEM_AUTO = 243,
	OEM_ENLW = 244,
	OEM_BACK_TAB = 245,
	ATTN = 246,
	CRSEL = 247,
	EXSEL = 248,
	EREOF = 249,
	PLAY = 250,
	ZOOM = 251,
	NONAME = 252,
	PA1 = 253,
	OEM_CLEAR = 254,
};

enum class eCarAlarm {
	Off
	On
	Active
};

enum class eVehicleSubclass {
	Automobile
	Mtruck
	Quad
	Heli
	Plane
	Boat
	Train
	Fheli
	Fplane
	Bike
	Bmx
	Trailer
};

enum class eEntityType {
	Other
	Building
	Object
	Ped
	Vehicle
};

enum class eImGuiCond {
	NONE = 0,
	ALWAYS = 1,
	ONCE = 2,
	FIRST_USE_EVER = 4,
	APPEARING = 8,
};

enum class eWeaponFire {
	Melee
	InstantHit
	Projectile
	AreaEffect
	Camera
	Use
};

enum class eDrawEvent {
	BeforeDrawing
	AfterDrawing
	BeforeHud
	AfterHud
	BeforeRadar
	AfterRadar
	BeforeRadarOverlay
	AfterRadarOverlay
	BeforeBlips
	AfterBlips
	AfterFade
};

enum class eEaseMode {
	Quadratic
	Cubic
	Quartic
	Quintic
	Sinusoidal
	Exponential
	Circular
	Elastic
	Back
	Bounce
};

enum class eEaseWay {
	In
	Out
	InOut
};

enum class eChangeMoney {
	Set
	Add
	Remove
};

enum class eImGuiDir {
	Left
	Right
	Up
	Down
};

enum class ePadId {
	PAD1 = 0,
	PAD2 = 1,
};

enum class eImGuiCol {
	Text
	TextDisabled
	WindowBg
	ChildBg
	PopupBg
	Border
	BorderShadow
	FrameBg
	FrameBgHovered
	FrameBgActive
	TitleBg
	TitleBgActive
	TitleBgCollapsed
	MenuBarBg
	ScrollbarBg
	ScrollbarGrab
	ScrollbarGrabHovered
	ScrollbarGrabActive
	CheckMark
	SliderGrab
	SliderGrabActive
	Button
	ButtonHovered
	ButtonActive
	Header
	HeaderHovered
	HeaderActive
	Separator
	SeparatorHovered
	SeparatorActive
	ResizeGrip
	ResizeGripHovered
	ResizeGripActive
	Tab
	TabHovered
	TabActive
	TabUnfocused
	TabUnfocusedActive
	PlotLines
	PlotLinesHovered
	PlotHistogram
	PlotHistogramHovered
	TableHeaderBg
	TableBorderStrong
	TableBorderLight
	TableRowBg
	TableRowBgAlt
	TextSelectedBg
	DragDropTarget
	NavHighlight
	NavWindowingHighlight
	NavWindowingDimBg
	ModalWindowDimBg
};

enum class eImGuiStyleVar {
	Alpha
	DisabledAlpha
	WindowPadding
	WindowRounding
	WindowBorderSize
	WindowMinSize
	WindowTitleAlign
	ChildRounding
	ChildBorderSize
	PopupRounding
	PopupBorderSize
	FramePadding
	FrameRounding
	FrameBorderSize
	ItemSpacing
	ItemInnerSpacing
	IndentSpacing
	CellPadding
	ScrollbarSize
	ScrollbarRounding
	GrabMinSize
	GrabRounding
	TabRounding
	ButtonTextAlign
	SelectableTextAlign
};

enum class eTaskStatus {
	WAITING_TO_START_TASK = 0,
	PERFORMING_TASK = 1,
	DORMANT_TASK = 2,
	VACANT_STAGE = 3,
	GROUP_TASK_STAGE = 4,
	ATTRACTOR_SCRIPT_TASK_STAGE = 5,
	SECONDARY_TASK_STAGE = 6,
	FINISHED_TASK = 7,
};

enum class eDrivingMode {
	STOP_FOR_CARS = 0,
	SLOW_DOWN_FOR_CARS = 1,
	AVOID_CARS = 2,
	PLOUGH_THROUGH = 3,
	STOP_FOR_CARS_IGNORE_LIGHTS = 4,
	AVOID_CARS_OBEY_LIGHTS = 5,
	AVOID_CARS_STOP_FOR_PEDS_OBEY_LIGHTS = 6,
};

enum class eHudColors {
	Red
	Green
	BlueDark
	BlueLight
	White
	Black
	Orange
	Purple
	Grey
	RedDark
	GreenDark
	OrangeLight
	GreyDark
	Blue
	Yellow
};

enum class eCheats {
	Weapon1
	Weapon2
	Weapon3
	MoneyArmourHealth
	WantedLevelUp
	WantedLevelDown
	SunnyWeather
	ExtraSunnyWeather
	CloudyWeather
	RainyWeather
	FoggyWeather
	FastWeather
	FastTime
	SlowTime
	Mayhem
	EverybodyAttacksPlayer
	WeaponsForAll
	Tank
	StockCar
	StockCar2
	StockCar3
	StockCar4
	Hearse
	Lovefist
	Trashmaster
	Golfcart
	BlowUpCars
	OnlyRenderWheels
	StrongGrip
	Suicide
	TrafficLights
	MadDrivers
	PinkCars
	BlackCars
	BackToTheFuture
	FlyingFish
	Fat
	Muscle
	Skinny
	ElvisLives
	VillagePeople
	BeachParty
	Gangs
	GangLand
	Ninja
	LoveConquersAll
	AllCarsAreShit
	AllCarsAreGreat
	FlyingCars
	CJPhoneHome
	Flyboy
	Vortex
	VehicleOfDeath
	Nitro
	SuperPowerCar
	Midnight
	Dusk
	Storm
	Sandstorm
	Predator
	HighJump
	BulletProof
	Scuba
	Parachute
	Jetpack
	NotWanted
	Wanted
	SuperPunch
	NoFood
	Riot
	Funhouse
	Adrenaline
	Ammo
	Driveby
	Wasteland
	CountrysideInvasion
	RecruitMe
	RecruitMeAk47
	RecruitMeRocket
	Respect
	SexAppeal
	Stamina
	WeaponSkills
	VehicleSkills
	Apache
	Quad
	Tanker
	Dozer
	StuntPlane
	MonsterTruck
	Pimp
	TaxiNitro
	Kerala
};

enum class eAlign {
	Center
	Left
	Right
};

enum class eModelInfoType {
	ATOMIC = 1,
	TIME = 3,
	WEAPON = 4,
	CLUMP = 5,
	VEHICLE = 6,
	PED = 7,
	LOD = 8,
};

enum class eListType {
	int
	float
	string
};

enum class eWeaponSkill {
	Poor
	Std
	Pro
	Cop
};

enum class eWeaponState {
	Ready
	Firing
	Reloading
	OutOfAmmo
	MeleeMadeContact
};

enum class eAnimGrp {
	Default
	Door
	Bikes
	BikeV
	BikeH
	BikeD
	Wayfarer
	Bmx
	Mtb
	Choppa
	Quad
	Python
	PythonBad
	Colt45
	ColtCop
	Colt45Pro
	Sawnoff
	SawnoffPro
	Silenced
	Shotgun
	ShotgunBad
	Buddy
	BuddyBad
	Uzi
	UziBad
	Rifle
	RifleBad
	Sniper
	Grenade
	Flame
	Rocket
	SprayCan
	Goggles
	Melee1
	Melee2
	Melee3
	Melee4
	Bbbat1
	GClub1
	Knife1
	Sword1
	Dildo1
	Flowers1
	Csaw1
	KickStd
	PistlWhp
	Medic
	Beach
	Sunbathe
	PlayIdles
	Riot
	Strip
	Gangs
	Attractors
	Player
	Fat
	Muscular
	PlayerRocket
	PlayerRocketF
	PlayerRocketM
	Player2Armed
	Player2ArmedF
	Player2ArmedM
	PlayerBbbat
	PlayerBbbatF
	PlayerBbbatM
	PlayerCsaw
	PlayerCsawF
	PlayerCsawM
	PlayerSneak
	PlayerJetpack
	Swim
	Drivebys
	BikeDbz
	CopDbz
	QuadDbz
	FatTired
	HandSignal
	HandSignalL
	LHand
	RHand
	Carry
	Carry05
	Carry105
	IntHouse
	IntOffice
	IntShop
	StealthKn
	StdCarAmims
	LowCarAmims
	TrkCarAnims
	StdBikeAnims
	SportBikeAnims
	VespaBikeAnims
	HarleyBikeAnims
	DirtBikeAnims
	WayfBikeAnims
	BmxBikeAnims
	MtbBikeAnims
	ChoppaBikeAnims
	QuadBikeAnims
	VanCarAnims
	RustPlaneAnims
	CoachCarAnims
	BusCarAnims
	DozerCarAnims
	KartCarAnims
	ConvCarAnims
	MtrkCarAnims
	TrainCarrAnims
	StdTallCarAmims
	HoverCarAnims
	TankCarAnims
	BFInjCarAmims
	LearPlaneAnims
	HarrPlaneAnims
	StdCarUpright
	NvadaPlaneAnims
	Man
	Shuffle
	OldMan
	Gang1
	Gang2
	OldFatMan
	FatMan
	Jogger
	DrunkMan
	BlindMan
	Swat
	Woman
	Shopping
	BusyWoman
	SexyWoman
	Pro
	OldWoman
	FatWoman
	JogWoman
	OldFatWoman
	Skate
};

enum class ePedState {
	NONE = 0,
	IDLE = 1,
	LOOK_ENTITY = 2,
	LOOK_HEADING = 3,
	WANDER_RANGE = 4,
	WANDER_PATH = 5,
	SEEK_POSITION = 6,
	SEEK_ENTITY = 7,
	FLEE_POSITION = 8,
	FLEE_ENTITY = 9,
	PURSUE = 10,
	FOLLOW_PATH = 11,
	SNIPER_MODE = 12,
	ROCKETLAUNCHER_MODE = 13,
	DUMMY = 14,
	PAUSE = 15,
	ATTACK = 16,
	FIGHT = 17,
	FACE_PHONE = 18,
	MAKE_PHONECALL = 19,
	CHAT = 20,
	MUG = 21,
	AIMGUN = 22,
	AI_CONTROL = 23,
	SEEK_CAR = 24,
	SEEK_BOAT_POSITION = 25,
	FOLLOW_ROUTE = 26,
	CPR = 27,
	SOLICIT = 28,
	BUY_ICE_CREAM = 29,
	INVESTIGATE_EVENT = 30,
	EVADE_STEP = 31,
	ON_FIRE = 32,
	SUNBATHE = 33,
	FLASH = 34,
	JOG = 35,
	ANSWER_MOBILE = 36,
	HANG_OUT = 37,
	STATES_NO_AI = 38,
	ABSEIL_FROM_HELI = 39,
	SIT = 40,
	JUMP = 41,
	FALL = 42,
	GETUP = 43,
	STAGGER = 44,
	EVADE_DIVE = 45,
	STATES_CAN_SHOOT = 46,
	ENTER_TRAIN = 47,
	EXIT_TRAIN = 48,
	ARREST_PLAYER = 49,
	DRIVING = 50,
	PASSENGER = 51,
	TAXI_PASSENGER = 52,
	OPEN_DOOR = 53,
	DIE = 54,
	DEAD = 55,
	DIE_BY_STEALTH = 56,
	CARJACK = 57,
	DRAGGED_FROM_CAR = 58,
	ENTER_CAR = 59,
	STEAL_CAR = 60,
	EXIT_CAR = 61,
	HANDS_UP = 62,
	ARRESTED = 63,
	DEPLOY_STINGER = 64,
};

enum class ePedStat {
	PLAYER = 0,
	COP = 1,
	MEDIC = 2,
	FIREMAN = 3,
	GANG1 = 4,
	GANG2 = 5,
	GANG3 = 6,
	GANG4 = 7,
	GANG5 = 8,
	GANG6 = 9,
	GANG7 = 10,
	GANG8 = 11,
	GANG9 = 12,
	GANG10 = 13,
	STREET_GUY = 14,
	SUIT_GUY = 15,
	SENSIBLE_GUY = 16,
	GEEK_GUY = 17,
	OLD_GUY = 18,
	TOUGH_GUY = 19,
	STREET_GIRL = 20,
	SUIT_GIRL = 21,
	SENSIBLE_GIRL = 22,
	GEEK_GIRL = 23,
	OLD_GIRL = 24,
	TOUGH_GIRL = 25,
	TRAMP_MALE = 26,
	TRAMP_FEMALE = 27,
	TOURIST = 28,
	PROSTITUTE = 29,
	CRIMINAL = 30,
	BUSKER = 31,
	TAXIDRIVER = 32,
	PSYCHO = 33,
	STEWARD = 34,
	SPORTSFAN = 35,
	SHOPPER = 36,
	OLDSHOPPER = 37,
	BEACH_GUY = 38,
	BEACH_GIRL = 39,
	SKATER = 40,
	STD_MISSION = 41,
	COWARD = 42,
};

enum class eStringFind {
	First
	Last
};

enum class eCarAnimGroup {
	STANDARD_CAR = 0,
	LOW_CAR = 1,
	TRUCK = 2,
	STANDARD_BIKE = 3,
	SPORT_BIKE = 4,
	VESPA_BIKE = 5,
	HARLEY_BIKE = 6,
	DIRT_BIKE = 7,
	WAYFARER_BIKE = 8,
	BMX_BIKE = 9,
	MTB_BIKE = 10,
	CHOPPA_BIKE = 11,
	QUAD_BIKE = 12,
	VAN = 13,
	RUSTLER = 14,
	COACH = 15,
	BUS = 16,
	DOZER = 17,
	KART = 18,
	CONVERTIBLE = 19,
	MTRUCK = 20,
	TRAIN_CARRIAGE = 21,
	STANDARD_HIGH = 22,
	HOVER = 23,
	TANK = 24,
	BF_INJECTION = 25,
	SHAMAL = 26,
	HARRIER = 27,
	STANDARD_UPRIGHT = 28,
	NEVADA = 29,
};

enum class eTaskId {
	SIMPLE_PLAYER_ON_FOOT = 0,
	SIMPLE_PLAYER_IN_CAR = 1,
	COMPLEX_MEDIC_TREAT_INJURED_PED = 100,
	COMPLEX_TREAT_ACCIDENT = 101,
	SIMPLE_GIVE_CPR = 102,
	COMPLEX_COP_ARREST_PED = 103,
	COMPLEX_COP_HASSLE_PED = 104,
	COMPLEX_HASSLED_BY_COP = 105,
	COMPLEX_PRESENT_ID_TO_COP = 106,
	COMPLEX_DRIVE_FIRE_TRUCK = 107,
	COMPLEX_USE_SWAT_ROPE = 108,
	COMPLEX_USE_WATER_CANNON = 109,
	COMPLEX_EXTINGUISH_FIRE_ON_FOOT = 110,
	NONE = 200,
	SIMPLE_UNINTERRUPTABLE = 201,
	SIMPLE_PAUSE = 202,
	SIMPLE_STAND_STILL = 203,
	SIMPLE_SET_STAY_IN_SAME_PLACE = 204,
	SIMPLE_GET_UP = 205,
	COMPLEX_GET_UP_AND_STAND_STILL = 206,
	SIMPLE_FALL = 207,
	COMPLEX_FALL_AND_GET_UP = 208,
	COMPLEX_FALL_AND_STAY_DOWN = 209,
	SIMPLE_JUMP = 210,
	COMPLEX_JUMP = 211,
	SIMPLE_DIE = 212,
	SIMPLE_DROWN = 213,
	SIMPLE_DIE_IN_CAR = 214,
	COMPLEX_DIE_IN_CAR = 215,
	SIMPLE_DROWN_IN_CAR = 216,
	COMPLEX_DIE = 217,
	SIMPLE_DEAD = 218,
	SIMPLE_TIRED = 219,
	SIMPLE_SIT_DOWN = 220,
	SIMPLE_SIT_IDLE = 221,
	SIMPLE_STAND_UP = 222,
	COMPLEX_SIT_DOWN_THEN_IDLE_THEN_STAND_UP = 223,
	COMPLEX_OBSERVE_TRAFFIC_LIGHTS = 224,
	COMPLEX_OBSERVE_TRAFFIC_LIGHTS_AND_ACHIEVE_HEADING = 225,
	NOT_USED = 226,
	COMPLEX_CROSS_ROAD_LOOK_AND_ACHIEVE_HEADING = 227,
	SIMPLE_TURN180 = 228,
	SIMPLE_HAIL_TAXI = 229,
	COMPLEX_HIT_RESPONSE = 230,
	COMPLEX_HIT_BY_GUN_RESPONSE = 231,
	UNUSED_SLOT = 232,
	COMPLEX_USE_EFFECT = 233,
	COMPLEX_WAIT_AT_ATTRACTOR = 234,
	COMPLEX_USE_ATTRACTOR = 235,
	COMPLEX_WAIT_FOR_DRY_WEATHER = 236,
	COMPLEX_WAIT_FOR_BUS = 237,
	SIMPLE_WAIT_FOR_BUS = 238,
	SIMPLE_WAIT_FOR_PIZZA = 239,
	COMPLEX_IN_AIR_AND_LAND = 240,
	SIMPLE_IN_AIR = 241,
	SIMPLE_LAND = 242,
	COMPLEX_BE_IN_GROUP = 243,
	COMPLEX_SEQUENCE = 244,
	SIMPLE_CALL_FOR_BACKUP = 245,
	COMPLEX_USE_PAIRED_ATTRACTOR = 246,
	COMPLEX_USE_ATTRACTOR_PARTNER = 247,
	COMPLEX_ATTRACTOR_PARTNER_WAIT = 248,
	COMPLEX_USE_SCRIPTED_ATTRACTOR = 249,
	COMPLEX_ON_FIRE = 250,
	SIMPLE_BE_DAMAGED = 251,
	SIMPLE_TRIGGER_EVENT = 252,
	SIMPLE_RAGDOLL = 253,
	SIMPLE_CLIMB = 254,
	SIMPLE_PLAYER_ON_FIRE = 255,
	COMPLEX_PARTNER = 256,
	COMPLEX_STARE_AT_PED = 257,
	COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR = 258,
	COMPLEX_USE_EFFECT_RUNNING = 259,
	COMPLEX_USE_EFFECT_SPRINTING = 260,
	COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_RUN = 261,
	COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_SPRINT = 262,
	SIMPLE_CHOKING = 263,
	SIMPLE_IK_CHAIN = 264,
	SIMPLE_IK_MANAGER = 265,
	SIMPLE_IK_LOOK_AT = 266,
	COMPLEX_CLIMB = 267,
	COMPLEX_IN_WATER = 268,
	SIMPLE_TRIGGER_LOOK_AT = 269,
	SIMPLE_CLEAR_LOOK_AT = 270,
	SIMPLE_SET_CHAR_DECISION_MAKER = 271,
	SIMPLE_IK_POINT_R_ARM = 272,
	SIMPLE_IK_POINT_L_ARM = 273,
	COMPLEX_BE_STILL = 274,
	COMPLEX_USE_SEQUENCE = 275,
	SIMPLE_SET_KINDA_STAY_IN_SAME_PLACE = 276,
	COMPLEX_FALL_TO_DEATH = 277,
	WAIT_FOR_MATCHING_LEADER_AREA_CODES = 278,
	SIMPLE_LOOK_AT_ENTITY_OR_COORD = 300,
	SIMPLE_SAY = 301,
	SIMPLE_SHAKE_FIST = 302,
	SIMPLE_FACIAL = 303,
	COMPLEX_CHAINED_FACIAL = 304,
	COMPLEX_FACIAL = 305,
	SIMPLE_AFFECT_SECONDARY_BEHAVIOUR = 306,
	SIMPLE_HOLD_ENTITY = 307,
	SIMPLE_PICKUP_ENTITY = 308,
	SIMPLE_PUTDOWN_ENTITY = 309,
	COMPLEX_GO_PICKUP_ENTITY = 310,
	SIMPLE_DUCK_WHILE_SHOTS_WHIZZING = 311,
	SIMPLE_ANIM = 400,
	SIMPLE_NAMED_ANIM = 401,
	SIMPLE_TIMED_ANIM = 402,
	SIMPLE_HIT_BACK = 403,
	SIMPLE_HIT_FRONT = 404,
	SIMPLE_HIT_LEFT = 405,
	SIMPLE_HIT_RIGHT = 406,
	SIMPLE_HIT_BY_GUN_BACK = 407,
	SIMPLE_HIT_BY_GUN_FRONT = 408,
	SIMPLE_HIT_BY_GUN_LEFT = 409,
	SIMPLE_HIT_BY_GUN_RIGHT = 410,
	SIMPLE_HIT_WALL = 411,
	SIMPLE_COWER = 412,
	SIMPLE_HANDS_UP = 413,
	SIMPLE_HIT_BEHIND = 414,
	SIMPLE_DUCK = 415,
	SIMPLE_CHAT = 416,
	COMPLEX_SUNBATHE = 417,
	SIMPLE_SUNBATHE = 418,
	SIMPLE_DETONATE = 419,
	SIMPLE_USE_ATM = 420,
	SIMPLE_SCRATCH_HEAD = 421,
	SIMPLE_LOOK_ABOUT = 422,
	SIMPLE_ABSEIL = 423,
	SIMPLE_ANIM_LOOPED_MIDDLE = 424,
	SIMPLE_HANDSIGNAL_ANIM = 425,
	COMPLEX_HANDSIGNAL_ANIM = 426,
	SIMPLE_DUCK_FOREVER = 427,
	SIMPLE_START_SUNBATHING = 428,
	SIMPLE_IDLE_SUNBATHING = 429,
	SIMPLE_STOP_SUNBATHING = 430,
	SIMPLE_HIT_HEAD = 500,
	SIMPLE_EVASIVE_STEP = 501,
	COMPLEX_EVASIVE_STEP = 502,
	SIMPLE_EVASIVE_DIVE = 503,
	COMPLEX_EVASIVE_DIVE_AND_GET_UP = 504,
	COMPLEX_HIT_PED_WITH_CAR = 505,
	SIMPLE_KILL_PED_WITH_CAR = 506,
	SIMPLE_HURT_PED_WITH_CAR = 507,
	COMPLEX_WALK_ROUND_CAR = 508,
	COMPLEX_WALK_ROUND_BUILDING_ATTEMPT = 509,
	COMPLEX_WALK_ROUND_OBJECT = 510,
	COMPLEX_MOVE_BACK_AND_JUMP = 511,
	COMPLEX_EVASIVE_COWER = 512,
	COMPLEX_DIVE_FROM_ATTACHED_ENTITY_AND_GET_UP = 513,
	COMPLEX_WALK_ROUND_FIRE = 514,
	COMPLEX_STUCK_IN_AIR = 515,
	COMPLEX_INVESTIGATE_DEAD_PED = 600,
	COMPLEX_REACT_TO_GUN_AIMED_AT = 601,
	COMPLEX_WAIT_FOR_BACKUP = 602,
	COMPLEX_GET_OUT_OF_WAY_OF_CAR = 603,
	COMPLEX_EXTINGUISH_FIRES = 604,
	COMPLEX_ENTER_CAR_AS_PASSENGER = 700,
	COMPLEX_ENTER_CAR_AS_DRIVER = 701,
	COMPLEX_STEAL_CAR = 702,
	COMPLEX_DRAG_PED_FROM_CAR = 703,
	COMPLEX_LEAVE_CAR = 704,
	COMPLEX_LEAVE_CAR_AND_DIE = 705,
	COMPLEX_LEAVE_CAR_AND_FLEE = 706,
	COMPLEX_LEAVE_CAR_AND_WANDER = 707,
	COMPLEX_SCREAM_IN_CAR_THEN_LEAVE = 708,
	SIMPLE_CAR_DRIVE = 709,
	COMPLEX_CAR_DRIVE_TO_POINT = 710,
	COMPLEX_CAR_DRIVE_WANDER = 711,
	COMPLEX_ENTER_CAR_AS_PASSENGER_TIMED = 712,
	COMPLEX_ENTER_CAR_AS_DRIVER_TIMED = 713,
	COMPLEX_LEAVE_ANY_CAR = 714,
	COMPLEX_ENTER_BOAT_AS_DRIVER = 715,
	COMPLEX_LEAVE_BOAT = 716,
	COMPLEX_ENTER_ANY_CAR_AS_DRIVER = 717,
	COMPLEX_ENTER_CAR_AS_PASSENGER_WAIT = 718,
	SIMPLE_CAR_DRIVE_TIMED = 719,
	COMPLEX_SHUFFLE_SEATS = 720,
	COMPLEX_CAR_DRIVE_POINT_ROUTE = 721,
	COMPLEX_CAR_OPEN_DRIVER_DOOR = 722,
	SIMPLE_CAR_SET_TEMP_ACTION = 723,
	COMPLEX_CAR_DRIVE_MISSION = 724,
	COMPLEX_CAR_DRIVE = 725,
	COMPLEX_CAR_DRIVE_MISSION_FLEE_SCENE = 726,
	COMPLEX_ENTER_LEADER_CAR_AS_PASSENGER = 727,
	COMPLEX_CAR_OPEN_PASSENGER_DOOR = 728,
	COMPLEX_CAR_DRIVE_MISSION_KILL_PED = 729,
	COMPLEX_LEAVE_CAR_AS_PASSENGER_WAIT = 730,
	COMPLEX_GO_TO_CAR_DOOR_AND_STAND_STILL = 800,
	SIMPLE_CAR_ALIGN = 801,
	SIMPLE_CAR_OPEN_DOOR_FROM_OUTSIDE = 802,
	SIMPLE_CAR_OPEN_LOCKED_DOOR_FROM_OUTSIDE = 803,
	SIMPLE_BIKE_PICK_UP = 804,
	SIMPLE_CAR_CLOSE_DOOR_FROM_INSIDE = 805,
	SIMPLE_CAR_CLOSE_DOOR_FROM_OUTSIDE = 806,
	SIMPLE_CAR_GET_IN = 807,
	SIMPLE_CAR_SHUFFLE = 808,
	SIMPLE_CAR_WAIT_TO_SLOW_DOWN = 809,
	SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE = 810,
	SIMPLE_CAR_SET_PED_IN_AS_PASSENGER = 811,
	SIMPLE_CAR_SET_PED_IN_AS_DRIVER = 812,
	SIMPLE_CAR_GET_OUT = 813,
	SIMPLE_CAR_JUMP_OUT = 814,
	SIMPLE_CAR_FORCE_PED_OUT = 815,
	SIMPLE_CAR_SET_PED_OUT = 816,
	SIMPLE_CAR_QUICK_DRAG_PED_OUT = 817,
	SIMPLE_CAR_QUICK_BE_DRAGGED_OUT = 818,
	SIMPLE_CAR_SET_PED_QUICK_DRAGGED_OUT = 819,
	SIMPLE_CAR_SLOW_DRAG_PED_OUT = 820,
	SIMPLE_CAR_SLOW_BE_DRAGGED_OUT = 821,
	SIMPLE_CAR_SET_PED_SLOW_DRAGGED_OUT = 822,
	COMPLEX_CAR_SLOW_BE_DRAGGED_OUT = 823,
	COMPLEX_CAR_SLOW_BE_DRAGGED_OUT_AND_STAND_UP = 824,
	COMPLEX_CAR_QUICK_BE_DRAGGED_OUT = 825,
	SIMPLE_BIKE_JACKED = 826,
	SIMPLE_SET_PED_AS_AUTO_DRIVER = 827,
	SIMPLE_GO_TO_POINT_NEAR_CAR_DOOR_UNTIL_DOOR_NOT_IN_USE = 828,
	SIMPLE_WAIT_UNTIL_PED_OUT_CAR = 829,
	COMPLEX_GO_TO_BOAT_STEERING_WHEEL = 830,
	COMPLEX_GET_ON_BOAT_SEAT = 831,
	SIMPLE_CREATE_CAR_AND_GET_IN = 832,
	SIMPLE_WAIT_UNTIL_PED_IN_CAR = 833,
	SIMPLE_CAR_FALL_OUT = 834,
	SIMPLE_GO_TO_POINT = 900,
	COMPLEX_GO_TO_POINT_SHOOTING = 901,
	SIMPLE_ACHIEVE_HEADING = 902,
	COMPLEX_GO_TO_POINT_AND_STAND_STILL = 903,
	COMPLEX_GO_TO_POINT_AND_STAND_STILL_AND_ACHIEVE_HEADING = 904,
	COMPLEX_FOLLOW_POINT_ROUTE = 905,
	COMPLEX_FOLLOW_NODE_ROUTE = 906,
	COMPLEX_SEEK_ENTITY = 907,
	COMPLEX_FLEE_POINT = 908,
	COMPLEX_FLEE_ENTITY = 909,
	COMPLEX_SMART_FLEE_POINT = 910,
	COMPLEX_SMART_FLEE_ENTITY = 911,
	COMPLEX_WANDER = 912,
	COMPLEX_FOLLOW_LEADER_IN_FORMATION = 913,
	COMPLEX_FOLLOW_SEXY_PED = 914,
	COMPLEX_GO_TO_ATTRACTOR = 915,
	COMPLEX_LEAVE_ATTRACTOR = 916,
	COMPLEX_AVOID_OTHER_PED_WHILE_WANDERING = 917,
	COMPLEX_GO_TO_POINT_ANY_MEANS = 918,
	COMPLEX_WALK_ROUND_SHOP = 919,
	COMPLEX_TURN_TO_FACE_ENTITY = 920,
	COMPLEX_AVOID_BUILDING = 921,
	COMPLEX_SEEK_ENTITY_ANY_MEANS = 922,
	COMPLEX_FOLLOW_LEADER_ANY_MEANS = 923,
	COMPLEX_GO_TO_POINT_AIMING = 924,
	COMPLEX_TRACK_ENTITY = 925,
	SIMPLE_GO_TO_POINT_FINE = 926,
	COMPLEX_FLEE_ANY_MEANS = 927,
	COMPLEX_FLEE_SHOOTING = 928,
	COMPLEX_SEEK_ENTITY_SHOOTING = 929,
	UNUSED1 = 930,
	COMPLEX_FOLLOW_PATROL_ROUTE = 931,
	COMPLEX_GOTO_DOOR_AND_OPEN = 932,
	COMPLEX_SEEK_ENTITY_AIMING = 933,
	SIMPLE_SLIDE_TO_COORD = 934,
	COMPLEX_INVESTIGATE_DISTURBANCE = 935,
	COMPLEX_FOLLOW_PED_FOOTSTEPS = 936,
	COMPLEX_FOLLOW_NODE_ROUTE_SHOOTING = 937,
	COMPLEX_USE_ENTRYEXIT = 938,
	COMPLEX_AVOID_ENTITY = 939,
	SMART_FLEE_ENTITY_WALKING = 940,
	COMPLEX_KILL_PED_ON_FOOT = 1000,
	COMPLEX_KILL_PED_ON_FOOT_MELEE = 1001,
	COMPLEX_KILL_PED_ON_FOOT_ARMED = 1002,
	COMPLEX_DESTROY_CAR = 1003,
	COMPLEX_DESTROY_CAR_MELEE = 1004,
	COMPLEX_DESTROY_CAR_ARMED = 1005,
	COMPLEX_REACT_TO_ATTACK = 1006,
	SIMPLE_BE_KICKED_ON_GROUND = 1007,
	SIMPLE_BE_HIT = 1008,
	SIMPLE_BE_HIT_WHILE_MOVING = 1009,
	COMPLEX_SIDE_STEP_AND_SHOOT = 1010,
	SIMPLE_DRIVEBY_SHOOT = 1011,
	SIMPLE_DRIVEBY_WATCH_FOR_TARGET = 1012,
	COMPLEX_DO_DRIVEBY = 1013,
	KILL_ALL_THREATS = 1014,
	KILL_PED_GROUP_ON_FOOT = 1015,
	SIMPLE_FIGHT = 1016,
	SIMPLE_USE_GUN = 1017,
	SIMPLE_THROW = 1018,
	SIMPLE_FIGHT_CTRL = 1019,
	SIMPLE_GUN_CTRL = 1020,
	SIMPLE_THROW_CTRL = 1021,
	SIMPLE_GANG_DRIVEBY = 1022,
	COMPLEX_KILL_PED_ON_FOOT_TIMED = 1023,
	COMPLEX_KILL_PED_ON_FOOT_STAND_STILL = 1024,
	UNUSED2 = 1025,
	KILL_PED_ON_FOOT_WHILE_DUCKING = 1026,
	SIMPLE_STEALTH_KILL = 1027,
	COMPLEX_KILL_PED_ON_FOOT_STEALTH = 1028,
	COMPLEX_KILL_PED_ON_FOOT_KINDA_STAND_STILL = 1029,
	COMPLEX_KILL_PED_AND_REENTER_CAR = 1030,
	COMPLEX_ROAD_RAGE = 1031,
	KILL_PED_FROM_BOAT = 1032,
	SIMPLE_SET_CHAR_IGNORE_WEAPON_RANGE_FLAG = 1033,
	SEEK_COVER_UNTIL_TARGET_DEAD = 1034,
	SIMPLE_ARREST_PED = 1100,
	COMPLEX_ARREST_PED = 1101,
	SIMPLE_BE_ARRESTED = 1102,
	COMPLEX_POLICE_PURSUIT = 1103,
	COMPLEX_BE_COP = 1104,
	COMPLEX_KILL_CRIMINAL = 1105,
	COMPLEX_COP_IN_CAR = 1106,
	SIMPLE_INFORM_GROUP = 1200,
	COMPLEX_GANG_LEADER = 1201,
	COMPLEX_PARTNER_DEAL = 1202,
	COMPLEX_PARTNER_GREET = 1203,
	COMPLEX_PARTNER_CHAT = 1204,
	COMPLEX_GANG_HASSLE_VEHICLE = 1205,
	COMPLEX_WALK_WITH_PED = 1206,
	COMPLEX_GANG_FOLLOWER = 1207,
	COMPLEX_WALK_ALONGSIDE_PED = 1208,
	COMPLEX_PARTNER_SHOVE = 1209,
	COMPLEX_SIGNAL_AT_PED = 1210,
	COMPLEX_PASS_OBJECT = 1211,
	COMPLEX_GANG_HASSLE_PED = 1212,
	COMPLEX_WAIT_FOR_PED = 1213,
	SIMPLE_DO_HAND_SIGNAL = 1214,
	COMPLEX_BE_IN_COUPLE = 1215,
	COMPLEX_GOTO_VEHICLE_AND_LEAN = 1216,
	COMPLEX_LEAN_ON_VEHICLE = 1217,
	COMPLEX_CHAT = 1218,
	COMPLEX_GANG_JOIN_RESPOND = 1219,
	ZONE_RESPONSE = 1300,
	SIMPLE_TOGGLE_PED_THREAT_SCANNER = 1301,
	FINISHED = 1302,
	SIMPLE_JETPACK = 1303,
	SIMPLE_SWIM = 1304,
	COMPLEX_SWIM_AND_CLIMB_OUT = 1305,
	SIMPLE_DUCK_TOGGLE = 1306,
	WAIT_FOR_MATCHING_AREA_CODES = 1307,
	SIMPLE_ON_ESCALATOR = 1308,
	COMPLEX_PROSTITUTE_SOLICIT = 1309,
	INTERIOR_USE_INFO = 1400,
	INTERIOR_GOTO_INFO = 1401,
	INTERIOR_BE_IN_HOUSE = 1402,
	INTERIOR_BE_IN_OFFICE = 1403,
	INTERIOR_BE_IN_SHOP = 1404,
	INTERIOR_SHOPKEEPER = 1405,
	INTERIOR_LIE_IN_BED = 1406,
	INTERIOR_SIT_ON_CHAIR = 1407,
	INTERIOR_SIT_AT_DESK = 1408,
	INTERIOR_LEAVE = 1409,
	INTERIOR_SIT_IN_RESTAURANT = 1410,
	INTERIOR_RESERVED2 = 1411,
	INTERIOR_RESERVED3 = 1412,
	INTERIOR_RESERVED4 = 1413,
	INTERIOR_RESERVED5 = 1414,
	INTERIOR_RESERVED6 = 1415,
	INTERIOR_RESERVED7 = 1416,
	INTERIOR_RESERVED8 = 1417,
	GROUP_FOLLOW_LEADER_ANY_MEANS = 1500,
	GROUP_FOLLOW_LEADER_WITH_LIMITS = 1501,
	GROUP_KILL_THREATS_BASIC = 1502,
	GROUP_KILL_PLAYER_BASIC = 1503,
	GROUP_STARE_AT_PED = 1504,
	GROUP_FLEE_THREAT = 1505,
	GROUP_PARTNER_DEAL = 1506,
	GROUP_PARTNER_GREET = 1507,
	GROUP_HASSLE_SEXY_PED = 1508,
	GROUP_HASSLE_THREAT = 1509,
	GROUP_USE_MEMBER_DECISION = 1510,
	GROUP_EXIT_CAR = 1511,
	GROUP_ENTER_CAR = 1512,
	GROUP_ENTER_CAR_AND_PERFORM_SEQUENCE = 1513,
	GROUP_RESPOND_TO_LEADER_COMMAND = 1514,
	GROUP_HAND_SIGNAL = 1515,
	GROUP_DRIVEBY = 1516,
	GROUP_HASSLE_THREAT_PASSIVE = 1517,
	COMPLEX_USE_MOBILE_PHONE = 1600,
	SIMPLE_PHONE_TALK = 1601,
	SIMPLE_PHONE_IN = 1602,
	SIMPLE_PHONE_OUT = 1603,
	COMPLEX_USE_GOGGLES = 1604,
	SIMPLE_GOGGLES_ON = 1605,
	SIMPLE_GOGGLES_OFF = 1606,
	SIMPLE_INFORM_RESPECTED_FRIENDS = 1700,
	COMPLEX_USE_SCRIPTED_BRAIN = 1800,
	SIMPLE_FINISH_BRAIN = 1801,
	SIMPLE_PLAYER_IN_CAR2 = 1802,
};

enum class eVehicleDummy {
	HEADLIGHT = 0,
	TAILLIGHT = 1,
	HEADLIGHTS2 = 2,
	TAILLIGHTS2 = 3,
	PED_FRONTSEAT = 4,
	PED_BACKSEAT = 5,
	EXHAUST = 6,
	ENGINE = 7,
	PETROLCAP = 8,
	AILERON_POS = 9,
	PED_ARM = 10,
	RUDDER_POS = 11,
	WINGTIP_POS = 12,
	MISCPOS_A = 13,
	MISCPOS_B = 14,
};

enum class eCarNodeDoor {
	DOOR_RF = 8,
	DOOR_RR = 9,
	DOOR_LF = 10,
	DOOR_LR = 11,
};

enum class eCarNode {
	CHASSIS = 0,
	WHEEL_RF = 1,
	WHEEL_RM = 2,
	WHEEL_RB = 3,
	WHEEL_LF = 4,
	WHEEL_LM = 5,
	WHEEL_LB = 6,
	DOOR_RF = 7,
	DOOR_RR = 8,
	DOOR_LF = 9,
	DOOR_LR = 10,
	BUMP_FRONT = 11,
	BUMP_REAR = 12,
	WING_RF = 13,
	WING_LF = 14,
	BONNET = 15,
	BOOT = 16,
	WINDSCREEN = 17,
	EXHAUST = 18,
	MISC_A = 19,
	MISC_B = 20,
	MISC_C = 21,
	MISC_D = 22,
	MISC_E = 23,
};

enum class eRwCombine {
	Replace
	PreConcat
	PostConcat
};

enum class ePedBone {
	PELVIS1 = 1,
	PELVIS = 2,
	SPINE1 = 3,
	UPPER_TORSO = 4,
	NECK = 5,
	HEAD2 = 6,
	HEAD1 = 7,
	HEAD = 8,
	RIGHT_UPPER_TORSO = 21,
	RIGHT_SHOULDER = 22,
	RIGHT_ELBOW = 23,
	RIGHT_WRIST = 24,
	RIGHT_HAND = 25,
	RIGHT_THUMB = 26,
	LEFT_UPPER_TORSO = 31,
	LEFT_SHOULDER = 32,
	LEFT_ELBOW = 33,
	LEFT_WRIST = 34,
	LEFT_HAND = 35,
	LEFT_THUMB = 36,
	LEFT_HIP = 41,
	LEFT_KNEE = 42,
	LEFT_ANKLE = 43,
	LEFT_FOOT = 44,
	RIGHT_HIP = 51,
	RIGHT_KNEE = 52,
	RIGHT_ANKLE = 53,
	RIGHT_FOOT = 54,
};

enum class eSurfaceType {
	DEFAULT = 0,
	TARMAC = 1,
	TARMAC_FUCKED = 2,
	TARMAC_REALLYFUCKED = 3,
	PAVEMENT = 4,
	PAVEMENT_FUCKED = 5,
	GRAVEL = 6,
	FUCKED_CONCRETE = 7,
	PAINTED_GROUND = 8,
	GRASS_SHORT_LUSH = 9,
	GRASS_MEDIUM_LUSH = 10,
	GRASS_LONG_LUSH = 11,
	GRASS_SHORT_DRY = 12,
	GRASS_MEDIUM_DRY = 13,
	GRASS_LONG_DRY = 14,
	GOLFGRASS_ROUGH = 15,
	GOLFGRASS_SMOOTH = 16,
	STEEP_SLIDYGRASS = 17,
	STEEP_CLIFF = 18,
	FLOWERBED = 19,
	MEADOW = 20,
	WASTEGROUND = 21,
	WOODLANDGROUND = 22,
	VEGETATION = 23,
	MUD_WET = 24,
	MUD_DRY = 25,
	DIRT = 26,
	DIRTTRACK = 27,
	SAND_DEEP = 28,
	SAND_MEDIUM = 29,
	SAND_COMPACT = 30,
	SAND_ARID = 31,
	SAND_MORE = 32,
	SAND_BEACH = 33,
	CONCRETE_BEACH = 34,
	ROCK_DRY = 35,
	ROCK_WET = 36,
	ROCK_CLIFF = 37,
	WATER_RIVERBED = 38,
	WATER_SHALLOW = 39,
	CORNFIELD = 40,
	HEDGE = 41,
	WOOD_CRATES = 42,
	WOOD_SOLID = 43,
	WOOD_THIN = 44,
	GLASS = 45,
	GLASS_WINDOWS_LARGE = 46,
	GLASS_WINDOWS_SMALL = 47,
	EMPTY1 = 48,
	EMPTY2 = 49,
	GARAGE_DOOR = 50,
	THICK_METAL_PLATE = 51,
	SCAFFOLD_POLE = 52,
	LAMP_POST = 53,
	METAL_GATE = 54,
	METAL_CHAIN_FENCE = 55,
	GIRDER = 56,
	SURFACE_FIRE_HYDRANT = 57,
	CONTAINER = 58,
	NEWS_VENDOR = 59,
	WHEELBASE = 60,
	CARDBOARDBOX = 61,
	PED = 62,
	CAR = 63,
	CAR_PANEL = 64,
	CAR_MOVINGCOMPONENT = 65,
	TRANSPARENT_CLOTH = 66,
	RUBBER = 67,
	PLASTIC = 68,
	TRANSPARENT_STONE = 69,
	WOOD_BENCH = 70,
	CARPET = 71,
	FLOORBOARD = 72,
	STAIRSWOOD = 73,
	P_SAND = 74,
	P_SAND_DENSE = 75,
	P_SAND_ARID = 76,
	P_SAND_COMPACT = 77,
	P_SAND_ROCKY = 78,
	P_SANDBEACH = 79,
	P_GRASS_SHORT = 80,
	P_GRASS_MEADOW = 81,
	P_GRASS_DRY = 82,
	P_WOODLAND = 83,
	P_WOODDENSE = 84,
	P_ROADSIDE = 85,
	P_ROADSIDEDES = 86,
	P_FLOWERBED = 87,
	P_WASTEGROUND = 88,
	P_CONCRETE = 89,
	P_OFFICEDESK = 90,
	P711_SHELF1 = 91,
	P711_SHELF2 = 92,
	P711_SHELF3 = 93,
	P_RESTUARANTTABLE = 94,
	P_BARTABLE = 95,
	P_UNDERWATERLUSH = 96,
	P_UNDERWATERBARREN = 97,
	P_UNDERWATERCORAL = 98,
	P_UNDERWATERDEEP = 99,
	P_RIVERBED = 100,
	P_RUBBLE = 101,
	P_BEDROOMFLOOR = 102,
	P_KIRCHENFLOOR = 103,
	P_LIVINGRMFLOOR = 104,
	P_CORRIDORFLOOR = 105,
	P711_FLOOR = 106,
	P_FASTFOODFLOOR = 107,
	P_SKANKYFLOOR = 108,
	P_MOUNTAIN = 109,
	P_MARSH = 110,
	P_BUSHY = 111,
	P_BUSHYMIX = 112,
	P_BUSHYDRY = 113,
	P_BUSHYMID = 114,
	P_GRASSWEEFLOWERS = 115,
	P_GRASSDRYTALL = 116,
	P_GRASSLUSHTALL = 117,
	P_GRASSGRNMIX = 118,
	P_GRASSBRNMIX = 119,
	P_GRASSLOW = 120,
	P_GRASSROCKY = 121,
	P_GRASSSMALLTREES = 122,
	P_DIRTROCKY = 123,
	P_DIRTWEEDS = 124,
	P_GRASSWEEDS = 125,
	P_RIVEREDGE = 126,
	P_POOLSIDE = 127,
	P_FORESTSTUMPS = 128,
	P_FORESTSTICKS = 129,
	P_FORRESTLEAVES = 130,
	P_DESERTROCKS = 131,
	P_FORRESTDRY = 132,
	P_SPARSEFLOWERS = 133,
	P_BUILDINGSITE = 134,
	P_DOCKLANDS = 135,
	P_INDUSTRIAL = 136,
	P_INDUSTJETTY = 137,
	P_CONCRETELITTER = 138,
	P_ALLEYRUBISH = 139,
	P_JUNKYARDPILES = 140,
	P_JUNKYARDGRND = 141,
	P_DUMP = 142,
	P_CACTUSDENSE = 143,
	P_AIRPORTGRND = 144,
	P_CORNFIELD = 145,
	P_GRASSLIGHT = 146,
	P_GRASSLIGHTER = 147,
	P_GRASSLIGHTER2 = 148,
	P_GRASSMID1 = 149,
	P_GRASSMID2 = 150,
	P_GRASSDARK = 151,
	P_GRASSDARK2 = 152,
	P_GRASSDIRTMIX = 153,
	P_RIVERBEDSTONE = 154,
	P_RIVERBEDSHALLOW = 155,
	P_RIVERBEDWEEDS = 156,
	P_SEAWEED = 157,
	DOOR = 158,
	PLASTICBARRIER = 159,
	PARKGRASS = 160,
	STAIRSSTONE = 161,
	STAIRSMETAL = 162,
	STAIRSCARPET = 163,
	FLOORMETAL = 164,
	FLOORCONCRETE = 165,
	BIN_BAG = 166,
	THIN_METAL_SHEET = 167,
	METAL_BARREL = 168,
	PLASTIC_CONE = 169,
	PLASTIC_DUMPSTER = 170,
	METAL_DUMPSTER = 171,
	WOOD_PICKET_FENCE = 172,
	WOOD_SLATTED_FENCE = 173,
	WOOD_RANCH_FENCE = 174,
	UNBREAKABLE_GLASS = 175,
	HAY_BALE = 176,
	GORE = 177,
	RAILTRACK = 178,
};

enum class ePlatform {
	None
	Android
	PSP
	IOS
	FOS
	XBox
	PS2
	PS3
	Mac
	Windows
};

enum class eVehicleClass {
	IGNORE = -1,
	NORMAL = 0,
	POOR_FAMILY = 1,
	RICH_FAMILY = 2,
	EXECUTIVE = 3,
	WORKER = 4,
	BIG = 5,
	TAXI = 6,
	MOPED = 7,
	MOTORBIKE = 8,
	LEISURE_BOAT = 9,
	WORKER_BOAT = 10,
	BICYCLE = 11,
};

enum class eCarLights {
	NO_OVERRIDE = 0,
	FORCE_OFF = 1,
	FORCE_ON = 2,
};

enum class eAudioStreamAction {
	STOP = 0,
	PLAY = 1,
	PAUSE = 2,
	RESUME = 3,
};

enum class eAudioStreamState {
	STOPPED = -1,
	PLAYING = 1,
	PAUSED = 2,
};

enum class eAudioStreamType {
	None
	Sfx
	Music
	UserInterface
};

enum class eDriveMode {
	NORMAL = 0,
	ACCURATE = 1,
	STRAIGHTLINE = 2,
	RACING = 3,
};

enum class eComponentStates {
	Disable
	Ok
	Dam
};

enum class eBlendValues {
	Zero
	One
	Srccolor
	Invsrccolor
	Srcalpha
	Invsrcalpha
	Destalpha
	Invdestalpha
	Destcolor
	Invdestcolor
	Srcalphasat
	Bothsrcalpha
	Bothinvsrcalpha
	Blendfactor
	Invblendfactor
	Srccolor2
	Invsrccolor2
};

enum class eVehicleDoors {
	Rf
	Rr
	Lf
	Lr
};

enum class eEntityTypes {
	Nothing
	Building
	Vehicle
	Ped
	Object
	Dummy
	Notinpools
};

enum class eEntityClasses {
	Entity
	Building
	Treadable
	Dummy
	Dummy86C198
	Physical
	Object
	Hand
	Projectile
	CutsceneObject
	Ped
	CivilianPed
	CopPed
	EmergencyPed
	PlayerPed
	Vehicle
	Automobile
	Heli
	Mtruck
	Plane
	Quad
	Trailer
	Bike
	Bmx
	Boat
	Train
};

enum class eVehicleClasses {
	Automobile
	Mtruck
	Quad
	Heli
	Plane
	Boat
	Train
	Fheli
	Fplane
	Bike
	Bmx
	Trailer
};

enum class eLightTypesCar {
	HeadLightLeft
	HeadLightRight
	RearLightRight
	RearLightLeft
};

enum class eLightTypes {
	Default
	Directional
	Dark
	Global
};

enum class eShadowTypes {
	Shadow
	Light
};

enum class eShadowTextures {
	Car
	Ped
	Explosion
	Heli
	Headlight
	Bloodpool
	Headlight1
	Bike
	Rcbaron
	Lamp
};

enum class eTrafficLightColors {
	Green
	Yellow
	Red
};

enum class ePositionTypes {
	Local
	Global
	World
};

enum class eProjectileTypes {
	Grenade
	Teargas
	Molotov
	Rocket
	RocketHs
	FreefallBomb
	RocketFlare
};

enum class eParamTypes {
	Int
	Float
};

enum class eDecisionMakerType {
	Empty
	Normal
	Tough
	Weak
	Steal
	Mission
};

enum class eDriveByType {
	FixedLeft
	FixedRight
	FixedStartLeft
	FixedStartRight
	AiSide
	FixedFront
	FixedBack
	AiFrontAndBack
	Ai
};

enum class eRouteMode {
	Once
	Return
	BackForward
	Loop
};

enum class eStatId {
	PROGRESS_MADE = 0,
	TOTAL_PROGRESS = 1,
	LONGEST_BASKETBALL = 2,
	DIST_FOOT = 3,
	DIST_CAR = 4,
	DIST_BIKE = 5,
	DIST_BOAT = 6,
	DIST_GOLF_CART = 7,
	DIST_HELICOPTER = 8,
	DIST_PLANE = 9,
	LONGEST_WHEELIE_DIST = 10,
	LONGEST_STOPPIE_DIST = 11,
	LONGEST2_WHEEL_DIST = 12,
	WEAPON_BUDGET = 13,
	FASHION_BUDGET = 14,
	PROPERTY_BUDGET = 15,
	SPRAYING_BUDGET = 16,
	LONGEST_WHEELIE_TIME = 17,
	LONGEST_STOPPIE_TIME = 18,
	LONGEST2_WHEEL_TIME = 19,
	FOOD_BUDGET = 20,
	FAT = 21,
	STAMINA = 22,
	BODY_MUSCLE = 23,
	MAX_HEALTH = 24,
	SEX_APPEAL = 25,
	DIST_SWIMMING = 26,
	DIST_CYCLE = 27,
	DIST_TREADMILL = 28,
	DIST_EXERCISE_BIKE = 29,
	TATTOO_BUDGET = 30,
	HAIRDRESSING_BUDGET = 31,
	NOT_USED_FLOAT6 = 32,
	NOT_USED_FLOAT1 = 33,
	NOT_USED_FLOAT2 = 34,
	MONEY_SPENT_GAMBLING = 35,
	MONEY_MADE_PIMPING = 36,
	MONEY_WON_GAMBLING = 37,
	BIGGEST_GAMBLING_WIN = 38,
	BIGGEST_GAMBLING_LOSS = 39,
	LARGEST_BURGLARY_SWAG = 40,
	MONEY_MADE_BURGLARY = 41,
	NOT_USED_FLOAT3 = 42,
	NOT_USED_FLOAT4 = 43,
	LONGEST_TREADMILL_TIME = 44,
	LONGEST_EXERCISE_BIKE_TIME = 45,
	HEAVIEST_WEIGHT_BENCH_PRESS = 46,
	HEAVIEST_WEIGHT_DUMBELLS = 47,
	BEST_TIME_HOTRING = 48,
	BEST_TIME_BMX = 49,
	NOT_USED_INT11 = 50,
	LONGEST_CHASE_TIME = 51,
	LAST_CHASE_TIME = 52,
	WAGE_BILL = 53,
	STRIP_CLUB_BUDGET = 54,
	CAR_MOD_BUDGET = 55,
	TIME_SPENT_SHOPPING = 56,
	NOT_USED_INT1 = 57,
	NOT_USED_INT3 = 58,
	NOT_USED_INT4 = 59,
	NOT_USED_INT19 = 60,
	NOT_USED_INT5 = 61,
	TOTAL_SHOPPING_BUDGET = 62,
	TIME_SPENT_UNDERWATER = 63,
	RESPECT_TOTAL = 64,
	RESPECT_GIRLFRIEND = 65,
	RESPECT_CLOTHES = 66,
	RESPECT_FITNESS = 67,
	RESPECT = 68,
	WEAPONTYPE_PISTOL_SKILL = 69,
	WEAPONTYPE_PISTOL_SILENCED_SKILL = 70,
	WEAPONTYPE_DESERT_EAGLE_SKILL = 71,
	WEAPONTYPE_SHOTGUN_SKILL = 72,
	WEAPONTYPE_SAWNOFF_SHOTGUN_SKILL = 73,
	WEAPONTYPE_SPAS12_SHOTGUN_SKILL = 74,
	WEAPONTYPE_MICRO_UZI_SKILL = 75,
	WEAPONTYPE_MP5_SKILL = 76,
	WEAPONTYPE_AK47_SKILL = 77,
	WEAPONTYPE_M4_SKILL = 78,
	WEAPONTYPE_SNIPERRIFLE_SKILL = 79,
	SEX_APPEAL_CLOTHES = 80,
	GAMBLING = 81,
	PEOPLE_KILLED_BY_OTHERS = 120,
	PEOPLE_KILLED_BY_PLAYER = 121,
	CARS_DESTROYED = 122,
	BOATS_DESTROYED = 123,
	HELICOPTERS_DESTROYED = 124,
	PROPERTY_DESTROYED = 125,
	ROUNDS_FIRED = 126,
	EXPLOSIVES_USED = 127,
	BULLETS_HIT = 128,
	TYRES_POPPED = 129,
	HEADS_POPPED = 130,
	WANTED_STARS_ATTAINED = 131,
	WANTED_STARS_EVADED = 132,
	TIMES_ARRESTED = 133,
	DAYS_PASSED = 134,
	TIMES_DIED = 135,
	TIMES_SAVED = 136,
	TIMES_CHEATED = 137,
	SPRAYINGS = 138,
	MAX_JUMP_DISTANCE = 139,
	MAX_JUMP_HEIGHT = 140,
	MAX_JUMP_FLIPS = 141,
	MAX_JUMP_SPINS = 142,
	BEST_STUNT = 143,
	UNIQUE_JUMPS_FOUND = 144,
	UNIQUE_JUMPS_DONE = 145,
	MISSIONS_ATTEMPTED = 146,
	MISSIONS_PASSED = 147,
	TOTAL_MISSIONS = 148,
	TAXI_MONEY_MADE = 149,
	PASSENGERS_DELIVERED_IN_TAXI = 150,
	LIVES_SAVED = 151,
	CRIMINALS_CAUGHT = 152,
	FIRES_EXTINGUISHED = 153,
	PIZZAS_DELIVERED = 154,
	ASSASSINATIONS = 155,
	LATEST_DANCE_SCORE = 156,
	VIGILANTE_LEVEL = 157,
	AMBULANCE_LEVEL = 158,
	FIREFIGHTER_LEVEL = 159,
	DRIVING_SKILL = 160,
	TRUCK_MISSIONS_PASSED = 161,
	TRUCK_MONEY_MADE = 162,
	RECRUITED_GANG_MEMBERS_KILLED = 163,
	ARMOUR = 164,
	ENERGY = 165,
	PHOTOS_TAKEN = 166,
	KILL_FRENZIES_ATTEMPTED = 167,
	KILL_FRENZIES_PASSED = 168,
	FLIGHT_TIME = 169,
	TIMES_DROWNED = 170,
	NUM_GIRLS_PIMPED = 171,
	BEST_POSITION_HOTRING = 172,
	FLIGHT_TIME_JETPACK = 173,
	SHOOTING_RANGE_SCORE = 174,
	VALET_CARS_PARKED = 175,
	KILLS_SINCE_LAST_CHECKPOINT = 176,
	TOTAL_LEGITIMATE_KILLS = 177,
	BLOODRING_KILLS = 178,
	BLOODRING_TIME = 179,
	NO_MORE_HURRICANES = 180,
	CITIES_PASSED = 181,
	POLICE_BRIBES = 182,
	CARS_STOLEN = 183,
	CURRENT_GIRLFRIENDS = 184,
	BAD_DATES = 185,
	GIRLS_DATED = 186,
	TIMES_SCORED_WITH_GIRL = 187,
	DATES = 188,
	GIRLS_DUMPED = 189,
	NOT_USED_INT9 = 190,
	HOUSES_BURGLED = 191,
	SAFES_CRACKED = 192,
	NOT_USED_INT2 = 193,
	STOLEN_ITEMS_SOLD = 194,
	EIGHT_BALLS_IN_POOL = 195,
	WINS_IN_POOL = 196,
	LOSSES_IN_POOL = 197,
	VISITS_TO_GYM = 198,
	NOT_USED_INT6 = 199,
	MEALS_EATEN = 200,
	QUARRY_MONEY_MADE = 201,
	QUARRY_COMPLETION_TIME = 202,
	NOT_USED_INT7 = 203,
	NOT_USED_INT8 = 204,
	HIGHEST_PED_KILLS_ON_SPREE = 205,
	HIGHEST_POLICE_KILLS_ON_SPREE = 206,
	HIGHEST_CIV_VEHICLES_DESTROYED_ON_SPREE = 207,
	HIGHEST_COP_VEHICLES_DESTROYED_ON_SPREE = 208,
	HIGHEST_TANKS_DESTROYED_ON_SPREE = 209,
	PIMPING_LEVEL = 210,
	KICKSTART_BEST_SCORE = 211,
	BEST_LAP_TIME_HOTRING = 212,
	VEHICLES_EXPORTED = 213,
	VEHICLES_IMPORTED = 214,
	HIGHEST_SCORE_IN_BASKETBALL_CHALLENGE = 215,
	FIRES_STARTED = 216,
	DRUGS_SOLD = 217,
	DRUGS_BOUGHT = 218,
	BEST_LAP_TIME_DIRTTRACK = 219,
	BEST_TIME_DIRTTRACK = 220,
	BEST_POSITION_DIRTTRACK = 221,
	BEST_TIME_NRG = 222,
	FLYING_SKILL = 223,
	RESPECT_MISSION = 224,
	UNDERWATER_BREATH_STAMINA = 225,
	GORE_RATING_LEVEL = 226,
	SEX_RATING_LEVEL = 227,
	RESPECT_MISSION_TOTAL = 228,
	BIKE_SKILL = 229,
	CYCLE_SKILL = 230,
	SNAPSHOTS_TAKEN = 231,
	TOTAL_SNAPSHOTS = 232,
	LUCK = 233,
	TERRITORIES_TAKEN_OVER = 234,
	TERRITORIES_LOST = 235,
	NUMBER_TERRITORIES_HELD = 236,
	HIGHEST_NUMBER_TERRITORIES_HELD = 237,
	NUMBER_GANG_MEMBERS_RECRUITED = 238,
	NUMBER_ENEMY_GANG_MEMBERS_KILLED = 239,
	NUMBER_FRIENDLY_GANG_MEMBERS_KILLED = 240,
	HORSESHOES_COLLECTED = 241,
	TOTAL_HORSESHOES = 242,
	OYSTERS_COLLECTED = 243,
	TOTAL_OYSTERS = 244,
	CALORIES = 245,
	P2_RAMPAGE_BEST_TIME = 246,
	P2_CARS_RAMPAGE_BEST_TIME = 247,
	P2_PEDS_RAMPAGE_BEST_TIME = 248,
	P2_HELI_RAMPAGE_BEST_TIME = 249,
	P2_BIKE_RAMPAGE_BEST_TIME = 250,
	KEEPIE_UPPY_BEST_TIME = 251,
	GIRLFRIEND_DENISE = 252,
	GIRLFRIEND_MICHELLE = 253,
	GIRLFRIEND_HELENA = 254,
	GIRLFRIEND_BARBARA = 255,
	GIRLFRIEND_KATIE = 256,
	GIRLFRIEND_MILLIE = 257,
	RACETOUR0_BEST_POSITION = 258,
	RACETOUR0_BEST_TIME = 259,
	RACETOUR1_BEST_POSITION = 260,
	RACETOUR1_BEST_TIME = 261,
	RACETOUR2_BEST_POSITION = 262,
	RACETOUR2_BEST_TIME = 263,
	RACETOUR3_BEST_POSITION = 264,
	RACETOUR3_BEST_TIME = 265,
	RACETOUR4_BEST_POSITION = 266,
	RACETOUR4_BEST_TIME = 267,
	RACETOUR5_BEST_POSITION = 268,
	RACETOUR5_BEST_TIME = 269,
	RACETOUR6_BEST_POSITION = 270,
	RACETOUR6_BEST_TIME = 271,
	RACETOUR7_BEST_POSITION = 272,
	RACETOUR7_BEST_TIME = 273,
	RACETOUR8_BEST_POSITION = 274,
	RACETOUR8_BEST_TIME = 275,
	RACETOUR9_BEST_POSITION = 276,
	RACETOUR9_BEST_TIME = 277,
	RACETOUR10_BEST_POSITION = 278,
	RACETOUR10_BEST_TIME = 279,
	RACETOUR11_BEST_POSITION = 280,
	RACETOUR11_BEST_TIME = 281,
	RACETOUR12_BEST_POSITION = 282,
	RACETOUR12_BEST_TIME = 283,
	RACETOUR13_BEST_POSITION = 284,
	RACETOUR13_BEST_TIME = 285,
	RACETOUR14_BEST_POSITION = 286,
	RACETOUR14_BEST_TIME = 287,
	RACETOUR15_BEST_POSITION = 288,
	RACETOUR15_BEST_TIME = 289,
	RACETOUR16_BEST_POSITION = 290,
	RACETOUR16_BEST_TIME = 291,
	RACETOUR17_BEST_POSITION = 292,
	RACETOUR17_BEST_TIME = 293,
	RACETOUR18_BEST_POSITION = 294,
	RACETOUR18_BEST_TIME = 295,
	RACETOUR19_BEST_TIME = 296,
	RACETOUR20_BEST_TIME = 297,
	RACETOUR21_BEST_TIME = 298,
	RACETOUR22_BEST_TIME = 299,
	RACETOUR23_BEST_TIME = 300,
	RACETOUR24_BEST_TIME = 301,
	PASSED_SWEET2 = 302,
	PASSED_STRAP3 = 303,
	PASSED_ZERO1 = 304,
	PASSED_SYNDICATE_AND_WOOZIE = 305,
	PASSED_CASINO7 = 306,
	PASSED_HEIST8 = 307,
	PASSED_MANSION2 = 308,
	STARTED_RIOT1 = 309,
	PASSED_RYDER2 = 310,
	PASSED_FARLIE3 = 311,
	PASSED_HEIST1 = 312,
	PASSED_SYNDICATE2 = 313,
	PASSED_LAFIN1 = 314,
	PASSED_CAT2 = 315,
	PASSED_SYNDICATE1 = 316,
	PASSED_CASINO4 = 317,
	PASSED_CAT1 = 318,
	PASSED_BCRASH1 = 319,
	PASSED_DESERT1 = 320,
	PASSED_GARAGE1 = 321,
	PASSED_BCESAR4 = 322,
	PASSED_LAFIN2 = 323,
	PASSED_GARAGE2 = 324,
	PASSED_DESERT4 = 325,
	PASSED_DESERT12 = 326,
	PASSED_CASINO3 = 327,
	PASSED_CASINO6 = 328,
	PASSED_CASINO10 = 329,
	PASSED_DESERT3 = 330,
	PASSED_DESERT5 = 331,
	PASSED_DESERT8 = 332,
	PASSED_DESERT10 = 333,
	PASSED_MANSION3 = 334,
	PASSED_RIOT1 = 335,
	PASSED_SCRASH1 = 336,
	PASSED_STRAP4 = 337,
	PASSED_TRUTH2 = 338,
	PASSED_VCRASH2 = 339,
	STARTED_BADLANDS = 340,
	STARTED_CAT2 = 341,
	STARTED_CRASH1 = 342,
};

enum class eCheckpointType {
	TUBE_ARROW = 0,
	TUBE_END = 1,
	TUBE = 2,
	TORUS = 3,
	TORUS_NO_FADE = 4,
	TORUS_ROT = 5,
	TORUS_THROUGH = 6,
	TORUS_UP_DOWN = 7,
	TORUS_DOWN = 8,
	NUM = 9,
};

enum class eWheelId {
	FrontLeft
	RearLeft
	FrontRight
	RearRight
	Any
	BIKE_FRONT = 0,
	BIKE_REAR = 1,
};

enum class eSpeechId {
	UNKNOWN = -1,
	NoSpeech
	AbuseGangBallas
	AbuseGangLsv
	AbuseGangVla
	AbuseGangFamilies
	AbuseTriad
	AbuseMafia
	AbuseRifa
	AbuseDaNang
	AcceptDateCall
	AcceptDateRequest
	AfterSex
	Apology
	Arrest
	ArrestCrim
	Arrested
	AttackByPlayerLike
	AttackGangBallas
	AttackGangLsv
	AttackGangVla
	AttackPlayer
	BarChat
	Blocked
	BoozeReceive
	BoozeRequest
	BumBackOff
	BumBackOff2
	BumLatch
	Bump
	CarCrash
	CarDrivebyBurnRubber
	CarDrivebyTooFast
	CarFast
	CarFire
	CarFlipped
	CarGetIn
	CarHitPed
	CarJump
	CarPolicePursuit
	CarSlow
	CarWaitForMe
	CarSingalong
	CbChat
	ChaseFoot
	Chased
	Chat
	CoffeeAccept
	CoffeeDecline
	ConvDislCar
	ConvDislClothes
	ConvDislHair
	ConvDislPhys
	ConvDislShoes
	ConvDislSmell
	ConvDislTattoo
	ConvDislWeather
	ConvIgnored
	ConvLikeCar
	ConvLikeClothes
	ConvLikeHair
	ConvLikePhys
	ConvLikeShoes
	ConvLikeSmell
	ConvLikeTattoo
	ConvLikeWeather
	CoverMe
	CrashBike
	CrashCar
	CrashGeneric
	CriminalPlead
	DanceImprHigh
	DanceImprLow
	DanceImprMed
	DanceImprNot
	Dodge
	DrugAggressiveHigh
	DrugAggressiveLow
	DrugDealerDislike
	DrugDealerHate
	DrugReasonableHigh
	DrugReasonableLow
	DruggedChat
	DruggedIgnore
	DrugsBuy
	DrugsSell
	Duck
	EyeingPed
	EyeingPedThreat
	EyeingPlayer
	Fight
	FollowArrive
	FollowConstant
	FollowReply
	GambBjHit
	GambBjLose
	GambBjStay
	GambBjWin
	GambBjStick
	GambBjDouble
	GambBjSplit
	GambCasinoWin
	GambCasinoLose
	GambCongrats
	GambRoulChat
	GambSlotWin
	GangFull
	GangbangNo
	GangbangYes
	GenericHiMale
	GenericHiFemale
	GenericInsultMale
	GenericInsultFemale
	GivingHead
	GoodCitizen
	GunCool
	GunRun
	HavingSex
	HavingSexMuffled
	JackedCar
	JackedGeneric
	JackedOnStreet
	JackingBike
	JackingCarFem
	JackingCarMale
	JackingGeneric
	JoinGangNo
	JoinGangYes
	JoinMeAsk
	JoinMeRejected
	LikeCarReply
	LikeClothesReply
	LikeDismissFemale
	LikeDismissMale
	LikeDismissReply
	LikeHairReply
	LikeNegativeFemale
	LikeNegativeMale
	LikePhysReply
	LikeShoesReply
	LikeSmellReply
	LikeTattooReply
	MeetGfriendAgainMaybe
	MeetGfriendAgainNo
	MeetGfriendAgainYes
	MoveIn
	Mugged
	Mugging
	OrderAttackMany
	OrderAttackSingle
	OrderDisbandMany
	OrderDisbandOne
	OrderFollowFarMany
	OrderFollowFarOne
	OrderFollowNearMany
	OrderFollowNearOne
	OrderFollowVnearMany
	OrderFollowVnearOne
	OrderKeepUpMany
	OrderKeepUpOne
	OrderWaitMany
	OrderWaitOne
	PconvAgreeBad
	PconvAgreeGood
	PconvAnsNo
	PconvDismiss
	PconvGreetFem
	PconvGreetMale
	PconvPartFem
	PconvPartMale
	PconvQuestion
	PconvStateBad
	PconvStateGood
	PickupCash
	PoliceBoat
	PoliceHelicopter
	PoliceOverboard
	PullGun
	Rope
	RunFromFight
	Saved
	Search
	Shocked
	Shoot
	ShootBallas
	ShootGeneric
	ShootLsv
	ShootVla
	ShootFamilies
	ShopBrowse
	ShopBuy
	ShopSell
	ShopLeave
	Solicit
	SolicitGenNo
	SolicitGenYes
	SolicitProNo
	SolicitProYes
	SolicitThanks
	SolicitUnreasonable
	Solo
	SpliffReceive
	SpliffRequest
	StealthAlertSound
	StealthAlertSight
	StealthAlertGeneric
	StealthDefSighting
	StealthNothingThere
	Surrounded
	TakeTurfLasColinas
	TakeTurfLosFlores
	TakeTurfEastBeach
	TakeTurfEastLs
	TakeTurfJefferson
	TakeTurfGlenPark
	TakeTurfIdlewood
	TakeTurfGanton
	TakeTurfLittleMexico
	TakeTurfWillowfield
	TakeTurfPlayaDelSeville
	TakeTurfTemple
	Target
	TaxiBail
	TaxiHail
	TaxiHitPed
	TaxiStart
	TaxiSuccess
	TaxiTip
	Trapped
	ValetBad
	ValetGood
	ValetParkCar
	Van
	Victim
	WeatherDislReply
	WeatherLikeReply
	WhereYouFromNeg
	WhereYouFromPos
	Spanked
	Spanking
	SpankingMuffled
	GreetingGfriend
	PartingGfriend
	UhHuh
	ClearAttachedPeds
	AgreeToDoDriveby
	AgreeToLetDrive
	MichelleTakeCar
	AcceptSex
	DeclineSex
	GiveNum classberYes {
	GiveNum classberNo {
	WhereYouFrom
	Gangbang
	WhereYouFromPosReply
	DriveThroughTaunt
	AttackCar
	TipCar
	ChaseCar
	EnemyGangWasted
	PlayerWasted
	Chase
	GfriendReqDateDesperate
	GfriendReqDateNormal
	GfriendReqMealDesperate
	GfriendReqMealNormal
	GfriendReqDriveDesperate
	GfriendReqDriveNormal
	GfriendReqDanceDesperate
	GfriendReqDanceNormal
	GfriendReqSexDesperate
	GfriendReqSexNormal
	GfriendBored1
	GfriendBored2
	GfriendStory
	GfriendLikeMealDest
	GfriendLikeClubDest
	GfriendOfferDance
	GfriendEnjoyedMealHigh
	GfriendEnjoyedEventLow
	GfriendEnjoyedClubHigh
	GfriendTakeHomeHappy
	GfriendTakeHomeAngry
	GfriendCoffee
	GfriendMoan
	GfriendMoanMuffled
	GfriendHead
	GfriendClimaxHigh
	GfriendClimaxHighMuffled
	GfriendClimaxLow
	GfriendSexGood
	GfriendSexGoodMuffled
	GfriendSexBad
	GfriendMeetAgain
	GfriendJealous
	GfriendJealousReply
	GfriendGoodbyeHappy
	GfriendGoodbyeAngry
	GfriendLeftBehind
	GfriendHello
	GfriendGoodbye
	GfriendPickupLocation
	GfriendParkUp
	GfriendParkLocationHate
	GfriendGiftLike
	GfriendChangeRadioFave
	GfriendChangeRadioBack
	GfriendDoADriveby
	GfriendStartAFight
	GfriendRejectDate
	GfriendRequestToDriveCar
	GfriendDropPlayerDriveAway
	GfriendDislikeCurrentZone
	GfriendLikeCurrentZone
	GfriendHitByPlayerWarning
	GfriendDumpPlayerLive
	GfriendDumpPlayerPhone
	GfriendSexAppealTooLow
	GfriendPhysiqueCritique
	GfriendIntro
	GfriendNegResponse
	GfriendPosResponse
	BoxingCheer
	BoughtEnough
	GiveProduct
	NoMoney
	PlayerSick
	RemoveTattoo
	ShopClosed
	ShowChangingroom
	ShopChat
	TakeASeat
	ThanksForCustom
	WelcomeToShop
	WhatWant
	PhotoCarl
	PhotoCheese
	Singing
	StomachRumble
	Breathing
	PainStart
	PainCough
	PainDeathDrown
	PainDeathHigh
	PainDeathLow
	PainHigh
	PainLow
	PainOnFire
	PainPanic
	PainSprayed
	PainCjBoxing
	PainCjGrunt
	PainCjPantIn
	PainCjPantOut
	PainCjPuke
	PainCjStrain
	PainCjStrainExhale
	PainCjSwimGasp
	PainCjDrowning
	PainCjHighFall
};

enum class eCarDoorState {
	Intact
	SwingingFree
	Bashed
	BashedAndSwingingFree
	Missing
};

enum class eSeatId {
	FrontRight
	RearLeft
	RearRight
	Passenger4
	Passenger5
	Passenger6
	Passenger7
	Passenger8
	BIKE_REAR = 0,
};

enum class eSetPieceType {
	None
	TwoCopCarsInAlley
	CarBlockingPlayerFromSide
	CarRammingPlayerFromSide
	CreateCopperOnFoot
	CreateTwoCoppersOnFoot
	TwoCarsBlockingPlayerFromSide
	TwoCarsRammingPlayerFromSide
};

enum class eCoordAppearance {
	NORMAL = 0,
	FRIEND = 1,
	ENEMY = 2,
};

enum class eEvent {
	NONE = -1,
	VEHICLE_COLLISION = 1,
	PED_COLLISION_WITH_PED = 2,
	PED_COLLISION_WITH_PLAYER = 3,
	PLAYER_COLLISION_WITH_PED = 4,
	OBJECT_COLLISION = 5,
	BUILDING_COLLISION = 6,
	DRAGGED_OUT_CAR = 7,
	KNOCK_OFF_BIKE = 8,
	DAMAGE = 9,
	DEATH = 10,
	DEAD_PED = 11,
	POTENTIAL_GET_RUN_OVER = 12,
	POTENTIAL_WALK_INTO_PED = 13,
	VEHICLE_HIT_AND_RUN = 14,
	SHOT_FIRED = 15,
	COP_CAR_BEING_STOLEN = 16,
	PED_ENTERED_MY_VEHICLE = 17,
	REVIVE = 18,
	CHAT_PARTNER = 19,
	SEXY_PED = 20,
	SEXY_VEHICLE = 21,
	PED_TO_CHASE = 25,
	PED_TO_FLEE = 26,
	ATTRACTOR = 27,
	VEHICLE_TO_STEAL = 28,
	VEHICLE_THREAT = 30,
	GUN_AIMED_AT = 31,
	SCRIPT_COMMAND = 32,
	IN_AIR = 33,
	VEHICLE_DIED = 35,
	ACQUAINTANCE_PED_HATE = 36,
	ACQUAINTANCE_PED_DISLIKE = 37,
	ACQUAINTANCE_PED_LIKE = 38,
	ACQUAINTANCE_PED_RESPECT = 39,
	VEHICLE_DAMAGE_WEAPON = 41,
	SPECIAL = 42,
	GOT_KNOCKED_OVER_BY_CAR = 43,
	POTENTIAL_WALK_INTO_OBJECT = 44,
	CAR_UPSIDE_DOWN = 45,
	POTENTIAL_WALK_INTO_FIRE = 46,
	GROUP_EVENT = 47,
	SCRIPTED_ATTRACTOR = 48,
	SHOT_FIRED_WHIZZED_BY = 49,
	LOW_ANGER_AT_PLAYER = 50,
	HIGH_ANGER_AT_PLAYER = 51,
	HEALTH_REALLY_LOW = 52,
	HEALTH_LOW = 53,
	LEADER_ENTERED_CAR_AS_DRIVER = 54,
	LEADER_EXITED_CAR_AS_DRIVER = 55,
	POTENTIAL_WALK_INTO_VEHICLE = 56,
	LEADER_QUIT_ENTERING_CAR_AS_DRIVER = 57,
	ON_FIRE = 58,
	FIRE_NEARBY = 59,
	POTENTIAL_WALK_INTO_BUILDING = 60,
	SOUND_LOUD = 61,
	SOUND_QUIET = 62,
	ACQUAINTANCE_PED_HATE_BADLY_LIT = 63,
	WATER_CANNON = 64,
	SEEN_PANICKED_PED = 65,
	IN_WATER = 66,
	AREA_CODES = 67,
	PLAYER_WANTED_LEVEL = 69,
	CREATE_PARTNER_TASK = 70,
	PLAYER_COMMAND_TO_GROUP = 71,
	SEEN_COP = 72,
	VEHICLE_DAMAGE_COLLISION = 73,
	ON_ESCALATOR = 74,
	DANGER = 75,
	PLAYER_COMMAND_TO_GROUP_GATHER = 76,
	LEADER_ENTRY_EXIT = 77,
	VEHICLE_ON_FIRE = 79,
	INTERIOR_USE_INFO = 81,
	SIGNAL_AT_PED = 88,
	PASS_OBJECT = 89,
	NEW_GANG_MEMBER = 90,
	LEAN_ON_VEHICLE = 92,
	STUCK_IN_AIR = 93,
	DONT_JOIN_GROUP = 94,
};

enum class eDecisionMakerCharTemplate {
	GANG = 65536,
	COP = 65537,
	RANDOM_NORM = 65538,
	RANDOM_TOUGH = 65539,
	RANDOM_WEAK = 65540,
	FIREMAN = 65541,
	EMPTY = 65542,
	INDOORS = 65543,
};

enum class eDecisionMakerGroupTemplate {
	RANDOM = 65544,
	RANDOM2 = 65545,
};

enum class eZoneType {
	Business
	Desert
	Entertainment
	Countryside
	ResidentialRich
	ResidentialAverage
	ResidentialPoor
	Gangland
	Beach
	Shopping
	Park
	Industry
	EntertainmentBusy
	ShoppingBusy
	ShoppingPosh
	ResidentialRichSecluded
	Airport
	GolfClub
	OutOfTownFactory
	AirportRunway
};

enum class eRaceSet {
	OTHER = 0,
	BLACK = 1,
	WHITE = 2,
	BLACK_WHITE = 3,
	ORIENTAL = 4,
	BLACK_ORIENTAL = 5,
	ORIENTAL_WHITE = 6,
	BLACK_ORIENTAL_WHITE = 7,
	HISPANIC = 8,
	BLACK_HISPANIC = 9,
	HISPANIC_WHITE = 10,
	BLACK_HISPANIC_WHITE = 11,
	HISPANIC_ORIENTAL = 12,
	BLACK_HISPANIC_ORIENTAL = 13,
	HISPANIC_ORIENTAL_WHITE = 14,
	BLACK_HISPANIC_ORIENTAL_WHITE = 15,
};

enum class eFightMoves {
	ClearAll
	Unknown1
	Unknown2
	Unknown3
	Unknown4
	Unknown5
	All
};

enum class eFacing {
	Forward
	Left
	Back
	Right
};

enum class eCarPlateDesign {
	SanFierro
	LasVenturas
	LosSantos
};

enum class eDecisionMakerGroupType {
	MissionNorm
};

enum class eLevel {
	GENERIC = 0,
	LOS_ANGELES = 1,
	SAN_FRANCISCO = 2,
	LAS_VEGAS = 3,
};

enum class eSampGameState {
	None
	WaitConnect
	AwaitJoin
	Connected
	Restarting
	Disconnected
};

enum class eSampRpc {
	SCR_SET_PLAYER_NAME = 11,
	ScrSetPlayerPos
	ScrSetPlayerPosFindZ
	ScrSetPlayerHealth
	ScrTogglePlayerControllable
	ScrPlaySound
	ScrSetPlayerWorldBounds
	ScrGivePlayerMoney
	ScrSetPlayerFacingAngle
	ScrResetPlayerMoney
	ScrResetPlayerWeapons
	ScrGivePlayerWeapon
	ClickPlayer
	ScrSetCarParamsEx
	ClientJoin
	EnterCar
	EnterEditObject
	ScrCancelEdit
	ScrSetPlayerTime
	ScrToggleClock
	ScriptCash
	ScrWorldPlayerAdd
	ScrSetPlayerShopName
	ScrSetPlayerSkillLevel
	ScrSetPlayerDrunkLevel
	ScrCreate3DTextLabel
	ScrDisableCheckpoint
	ScrSetRaceCheckpoint
	ScrDisableRaceCheckpoint
	ScrGameModeRestart
	ScrPlayAudioStream
	ScrStopAudioStream
	ScrRemoveBuildingForPlayer
	ScrCreateObject
	ScrSetObjectPos
	ScrSetObjectRot
	ScrDestroyObject
	SERVER_COMMAND = 50,
	SPAWN = 52,
	Death
	NpcJoin
	ScrDeathMessage
	ScrSetPlayerMapIcon
	ScrRemoveCarComponent
	ScrUpdate3DTextLabel
	ScrChatBubble
	ScrSomeUpdate
	ScrShowDialog
	DialogResponse
	ScrDestroyPickup
	SCR_LINK_CAR_TO_INTERIOR = 65,
	ScrSetPlayerArmour
	ScrSetPlayerArmedWeapon
	ScrSetSpawnInfo
	ScrSetPlayerTeam
	ScrPutPlayerInCar
	ScrRemovePlayerFromCar
	ScrSetPlayerColor
	ScrDisplayGameText
	ScrForceClassSelection
	ScrAttachObjectToPlayer
	ScrInitMenu
	ScrShowMenu
	ScrHideMenu
	ScrCreateExplosion
	ScrShowPlayerNametagForPlayer
	ScrAttachCameraToObject
	ScrInterpolateCamera
	ClickTextDraw
	ScrSetObjectMaterial
	ScrGangZoneStopFlash
	ScrApplyAnimation
	ScrClearAnimations
	ScrSetPlayerSpecialAction
	ScrSetPlayerFightingStyle
	ScrSetPlayerVelocity
	ScrSetCarVelocity
	SCR_CLIENT_MESSAGE = 93,
	ScrSetWorldTime
	ScrCreatePickup
	ScmEvent
	WeaponPickupDestroy
	SCR_MOVE_OBJECT = 99,
	CHAT = 101,
	SrvNetStats
	ClientCheck
	ScrEnableStuntBonusForPlayer
	ScrTextDrawSetString
	DamageCar
	ScrSetCheckpoint
	ScrGangZoneCreate
	SCR_PLAY_CRIME_REPORT = 112,
	ScrSetPlayerAttachedObject
	GIVE_TAKE_DAMAGE = 115,
	EditAttachedObject
	EditObject
	RequestInteriorChange
	MapMarker
	ScrGangZoneDestroy
	ScrGangZoneFlash
	ScrStopObject
	ScrSetNumberPlate
	ScrTogglePlayerSpectating
	SCR_PLAYER_SPECTATE_PLAYER = 126,
	ScrPlayerSpectateCar
	RequestClass
	RequestSpawn
	PICKED_UP_PICKUP = 131,
	MenuSelect
	ScrSetPlayerWantedLevel
	ScrShowTextDraw
	ScrTextDrawHideForPlayer
	CarDestroyed
	ScrServerJoin
	ScrServerQuit
	ScrInitGame
	MenuQuit
	SCR_REMOVE_PLAYER_MAP_ICON = 144,
	ScrSetPlayerAmmo
	ScrSetGravity
	ScrSetCarHealth
	ScrAttachTrailerToCar
	ScrDetachTrailerFromCar
	SCR_SET_WEATHER = 152,
	ScrSetPlayerSkin
	ExitCar
	UpdateScoresPingsIps
	ScrSetPlayerInterior
	ScrSetPlayerCameraPos
	ScrSetPlayerCameraLookAt
	ScrSetCarPos
	ScrSetCarZAngle
	ScrSetCarParamsForPlayer
	ScrSetCameraBehindPlayer
	ScrWorldPlayerRemove
	ScrWorldCarAdd
	ScrWorldCarRemove
	ScrWorldPlayerDeath
};

enum class eSampPacket {
	INTERNAL_PING = 6,
	Ping
	PingOpenConnections
	ConnectedPong
	RequestStaticData
	ConnectionRequest
	AuthKey
	BROADCAST_PINGS = 14,
	SecuredConnectionResponse
	SecuredConnectionConfirmation
	RpcMapping
	SET_RANDOM_NUMBER_SEED = 19,
	Rpc
	RpcReply
	DETECT_LOST_CONNECTIONS = 23,
	OpenConnectionRequest
	OpenConnectionReply
	ConnectionCookie
	RSA_PUBLIC_KEY_MISMATCH = 28,
	ConnectionAttemptFailed
	NewIncomingConnection
	NoFreeIncomingConnections1
	NoFreeIncomingConnections2
	ConnectionLost
	ConnectionRequestAccepted
	InitializeEncryption
	ConnectionBanned
	InvalidPassword
	ModifiedPacket
	Pong
	Timestamp
	ReceivedStaticData
	RemoteDisconnectionNotification
	RemoteConnectionLost
	RemoteNewIncomingConnection
	RemoteExistingConnection
	RemoteStaticData
	ADVERTISE_SYSTEM = 56,
	DRIVER_SYNC = 200,
	RconCommand
	RconResponse
	AimSync
	WeaponsUpdate
	StatsUpdate
	BulletSync
	OnFootSync
	MarkerSync
	UnoccupiedCarSync
	TrailerSync
	PassengerSync
	SpectatorSync
};

enum class eSampBitStreamDataType {
	Byte
	Bool
	Short
	Int
	Float
	Array
	Bitstream
	INT8 = 0,
	INT16 = 2,
	int32
};

enum class eSampPriority {
	System
	High
	Medium
	Low
};

enum class eSampReliability {
	UNRELIABLE = 6,
	UnreliableSequenced
	Reliable
	ReliableOrdered
	ReliableSequenced
};

enum class eSampSendRateType {
	ONFOOT = 1,
	Incar
	Aims
};

enum class eSampScmEvent {
	PAINTJOB = 1,
	Upgrade
	Color
	ModShopEnterExit
};

enum class eSampSpecialAction {
	None
	Duck
	UseJetPack
	EnterVehicle
	ExitVehicle
	Dance1
	Dance2
	Dance3
	Dance4
	HANDS_UP = 10,
	UseCellphone
	Sitting
	StopUseCellphone
	DRINK_BEER = 20,
	SmokeCiggy
	DrinkWine
	DrinkSprunk
	Cuffed
	Carry
	URINATE = 68,
};

enum class eSampDialogStyle {
	Msgbox
	Input
	List
	Password
	Tablist
	TablistHeaders
};

enum class eSampRakNetHookParam {
	Bitstream
	PacketId
	Priority
	Reliability
	Orderingchannel
	Shifttimestamp
};

enum class eSampD3DPrimitiveType {
	POINT_LIST = 1,
	LineList
	LineStrip
	TriangleList
	TriangleStrip
	TriangleFan
};

enum class eSampDisconnectReason {
	TimeoutOrCrashed
	Quit
	KickedOrBanned
};

enum class eDataTypeSize {
	BYTE = 1,
	BOOL = 1,
	SHORT = 2,
	INT = 4,
	FLOAT = 4,
};

enum class eSampCursorMode {
	Disabled
	LockkeysNocursor
	LockCamAndControl
	LockCam
	LockCamNoCursor
};

enum class eSampDialogButton {
	Right
	LeftOrCenter
};

enum class eSampBodyPart {
	TORSO = 3,
	Groin
	LeftArm
	RightArm
	LeftLeg
	RightLeg
	Head
};

enum class eSampTextStyle {
	FCR_NONE = 0,
	FCR_BOLD = 1,
	FCR_ITALICS = 2,
	FCR_BORDER = 4,
	FCR_SHADOW = 8,
	FCR_UNDERLINE = 16,
	FCR_STRIKEOUT = 32,
};

enum class eSampEditObjectResponse {
	Cancel
	Final
	Update
};

enum class eSampWeaponDamage {
	MELEE_UZI_TEC9 = 1087583028,
	CHAINSAW = 1104706274,
	FLOWERS = 1083430667,
	PISTOL_MP5 = 1090781184,
	SILENCER = 1095971636,
	DESERT_EAGLE_MINIGUN = 1111018701,
	SHOTGUN_3METERS = 1111883777,
	SAWN_OFF_3METERS = 1110153626,
	SPAZ_3METERS = 1109288551,
	AK47_M4_CAR_RAM = 1092511335,
	RIFLE = 1103495169,
	SNIPER = 1109721088,
	SPRAY_PAINT_FIRE_EXTINGUISHER = 1051260355,
	BURNING = 1054582246,
};

enum class eSampChatDisplayMode {
	Hidden
	NoShadow
	Standard
};

enum class eSampTextDrawAlignment {
	Left
	Center
	Right
};

enum class eSfDownloadState {
	IN_PROGRESS = -1,
	Complete
	OUT_OF_MEMORY = -2147024882,
	DOWNLOAD_FAILURE = -2146697208,
};

enum class eLanguage {
	ENGLISH = 0,
	FRENCH = 1,
	GERMAN = 2,
	ITALIAN = 3,
	SPANISH = 4,
};

enum class eCharSearchFilter {
	ANY = -1,
	ANY_NPC = 0,
	ANY_ALIVE_NPC = 1,
};

enum class eSwimState {
	Idle
	Breast
	Crawl
	Dive
	Underwater
	Jump
	JustEnteredWater
};

enum class eControllerAction {
	PedFireWeapon
	PedFireWeaponAlt
	PedCycleWeaponRight
	PedCycleWeaponLeft
	GoForward
	GoBack
	GoLeft
	GoRight
	PedSniperZoomIn
	PedSniperZoomOut
	VehicleEnterExit
	CameraChange
	PedJump
	PedSprint
	PedLookBehind
	PedDuck
	PedAnswerPhone
	PedSneak
	VehicleFireWeapon
	VehicleFireWeaponAlt
	VehicleSteerLeft
	VehicleSteerRight
	VehicleSteerUp
	VehicleSteerDown
	VehicleAccelerate
	VehicleBrake
	VehicleRadioStationUp
	VehicleRadioStationDown
	Unknown28
	VehicleHorn
	ToggleSubmission
	VehicleHandbrake
	Ped1stPersonLookLeft
	Ped1stPersonLookRight
	VehicleLookLeft
	VehicleLookRight
	VehicleLookBehind
	VehicleMouseLook
	VehicleTurretLeft
	VehicleTurretRight
	VehicleTurretUp
	VehicleTurretDown
	PedCycleTargetLeft
	PedCycleTargetRight
	PedCenterCamera
	PedLockTarget
	NetworkTalk
	ConversationYes
	ConversationNo
	GroupControlForward
	GroupControlBack
	Ped1stPersonLookUp
	Ped1stPersonLookDown
	Unknown53
	ToggleDpad
	SwitchDebugCamOn
	TakeScreenShot
	ShowMousePointer
};

enum class eControllerAltKey {
	Primary
	Secondary
	Tertiary
	Quaternary
};

enum class eHoldOrientation {
	PED_HEADING = 1,
	BONE = 16,
};

enum class ePedBoneId {
	NULL = 0,
	TORSO = 1,
	HEAD = 2,
	UPPER_ARM_L = 3,
	UPPER_ARM_R = 4,
	HAND_L = 5,
	HAND_R = 6,
	UPPER_LEG_L = 7,
	UPPER_LEG_R = 8,
	FOOT_L = 9,
	FOOT_R = 10,
	KNEE_R = 11,
	KNEE_L = 12,
	LOWER_ARM_L = 13,
	LOWER_ARM_R = 14,
	UPPER_CLAVICLE_L = 15,
	UPPER_CLAVICLE_R = 16,
	NECK = 17,
	JAW = 18,
};

enum class eButtonInVehicle {
	SteerLeftOrRight
	ROLL_LEFT_OR_RIGHT = 0,
	PitchUpOrDown
	TurretLeftOrRight
	TurretUpOrDown
	SecondaryFire
	LookLeft
	YAW_LEFT = 5,
	Handbrake
	LookRight
	YAW_RIGHT = 7,
	NextRadioStation
	PreviousRadioStation
	ConversationNo
	ConversationYes
	TRIP_SKIP = 11,
	CHANGE_CAMERA = 13,
	Reverse
	Exit
	Accelerate
	Shoot
	Horn
	SubMission
	TOGGLE_PLANE_WHEELS = 19,
};

enum class eButtonOnfoot {
	MoveLeftOrRight
	MoveForwardOrBackwards
	SpecialCtrlLeftOrRight
	SpecialCtrlUpOrDown
	Action
	PreviousWeapon
	Aim
	NextWeapon
	GroupCtrlBack
	GroupCtrlForward
	ConversationNo
	ConversationYes
	CHANGE_CAMERA = 13,
	JumpOrZoomIn
	EnterVehicle
	SprintOrZoomOut
	Attack
	Crouch
	LookBehind
};

enum class eTaskCommand {
	TASK_TOGGLE_DUCK = 1259,
	TASK_PAUSE = 1465,
	TASK_STAND_STILL = 1466,
	TASK_FALL_AND_GET_UP = 1467,
	TASK_JUMP = 1468,
	TASK_TIRED = 1469,
	TASK_DIE = 1470,
	TASK_LOOK_AT_CHAR_OBSERVER = 1471,
	TASK_LOOK_AT_VEHICLE = 1472,
	TASK_SAY = 1473,
	TASK_SHAKE_FIST = 1474,
	TASK_COWER = 1475,
	TASK_HANDS_UP = 1476,
	TASK_DUCK = 1477,
	TASK_USE_ATM = 1479,
	TASK_SCRATCH_HEAD = 1480,
	TASK_LOOK_ABOUT = 1481,
	TASK_ENTER_CAR_AS_PASSENGER = 1482,
	TASK_ENTER_CAR_AS_DRIVER = 1483,
	TASK_LEAVE_CAR = 1485,
	TASK_LEAVE_CAR_AND_FLEE = 1487,
	TASK_CAR_DRIVE_TO_COORD_DRIVER = 1489,
	TASK_CAR_DRIVE_WANDER = 1490,
	TASK_GO_STRAIGHT_TO_COORD = 1491,
	TASK_ACHIEVE_HEADING = 1492,
	TASK_FOLLOW_POINT_ROUTE = 1496,
	TASK_GOTO_CHAR_WALKING = 1497,
	TASK_FLEE_POINT = 1498,
	TASK_FLEE_CHAR = 1499,
	TASK_SMART_FLEE_POINT = 1500,
	TASK_SMART_FLEE_CHAR = 1501,
	TASK_WANDER_STANDARD = 1502,
	TASK_KILL_CHAR_ON_FOOT_KILLER = 1506,
	TASK_FOLLOW_PATH_NODES_TO_COORD = 1525,
	TASK_GO_TO_COORD_ANY_MEANS = 1539,
	TASK_PLAY_ANIM = 1541,
	PERFORM_SEQUENCE_TASK = 1560,
	TASK_LEAVE_CAR_IMMEDIATELY = 1570,
	TASK_LEAVE_ANY_CAR = 1587,
	TASK_KILL_CHAR_ON_FOOT_WHILE_DUCKING = 1588,
	TASK_AIM_GUN_AT_CHAR = 1589,
	TASK_GO_TO_COORD_WHILE_SHOOTING = 1591,
	TASK_STAY_IN_SAME_PLACE = 1592,
	TASK_TURN_CHAR_TO_FACE_CHAR = 1593,
	TASK_LOOK_AT_OBJECT = 1621,
	TASK_AIM_GUN_AT_COORD = 1639,
	TASK_SHOOT_AT_COORD = 1640,
	TASK_DESTROY_CAR = 1650,
	TASK_DIVE_AND_GET_UP = 1651,
	TASK_SHUFFLE_TO_NEXT_CAR_SEAT = 1654,
	TASK_CHAT_WITH_CHAR = 1655,
	TASK_TOGGLE_PED_THREAT_SCANNER = 1672,
	TASK_EVERYONE_LEAVE_CAR = 1675,
	TASK_DIVE_FROM_ATTACHMENT_AND_GET_UP = 1701,
	TASK_GOTO_CHAR_OFFSET = 1704,
	TASK_LOOK_AT_COORD = 1705,
	TASK_SIT_DOWN = 1712,
	TASK_TURN_CHAR_TO_FACE_COORD = 1722,
	TASK_DRIVE_POINT_ROUTE = 1723,
	TASK_GO_TO_COORD_WHILE_AIMING = 1730,
	TASK_CAR_TEMP_ACTION = 1735,
	TASK_CAR_MISSION = 1761,
	TASK_GO_TO_OBJECT = 1762,
	TASK_WEAPON_ROLL = 1763,
	TASK_CHAR_ARREST_CHAR = 1764,
	TASK_PICK_UP_OBJECT = 1802,
	TASK_DRIVE_BY = 1811,
	TASK_USE_MOBILE_PHONE = 1833,
	TASK_WARP_CHAR_INTO_CAR_AS_DRIVER = 1834,
	TASK_WARP_CHAR_INTO_CAR_AS_PASSENGER = 1835,
	TASK_USE_ATTRACTOR = 1868,
	TASK_SHOOT_AT_CHAR = 1869,
	TASK_FLEE_CHAR_ANY_MEANS = 1873,
	TASK_DEAD = 1890,
	TASK_GOTO_CAR = 1906,
	TASK_CLIMB = 1935,
	PERFORM_SEQUENCE_TASK_FROM_PROGRESS = 1952,
	TASK_GOTO_CHAR_AIMING = 1955,
	TASK_KILL_CHAR_ON_FOOT_TIMED = 1957,
	TASK_JETPACK = 1959,
	TASK_SET_CHAR_DECISION_MAKER = 1980,
	TASK_COMPLEX_PICKUP_OBJECT = 1993,
	TASK_CHAR_SLIDE_TO_COORD = 1997,
	TASK_SWIM_TO_COORD = 2017,
	TASK_DRIVE_POINT_ROUTE_ADVANCED = 2023,
	TASK_CHAR_SLIDE_TO_COORD_AND_PLAY_ANIM = 2052,
	TASK_PLAY_ANIM_NON_INTERRUPTABLE = 2066,
	TASK_FOLLOW_PATROL_ROUTE = 2071,
	TASK_GREET_PARTNER = 2083,
	TASK_DIE_NAMED_ANIM = 2089,
	TASK_FOLLOW_FOOTSTEPS = 2128,
	TASK_WALK_ALONGSIDE_CHAR = 2137,
	TASK_KINDA_STAY_IN_SAME_PLACE = 2139,
	TASK_PLAY_ANIM_WITH_FLAGS = 2186,
	TASK_USE_CLOSEST_MAP_ATTRACTOR = 2208,
	TASK_SET_IGNORE_WEAPON_RANGE_FLAG = 2463,
	TASK_PICK_UP_SECOND_OBJECT = 2464,
	TASK_PLAY_ANIM_SECONDARY = 2586,
	TASK_HAND_GESTURE = 2589,
	TASK_FOLLOW_PATH_NODES_TO_COORD_WITH_RADIUS = 2606,
};

enum class eCutsceneTrackStatus {
	STOPPED = 0,
	LOADING = 1,
	LOADED = 2,
	PLAYING = 3,
	STOPPING = 4,
};

enum class eCarPanel {
	FRONT_LEFT = 0,
	FRONT_RIGHT = 1,
	REAR_LEFT = 2,
	REAR_RIGHT = 3,
	WINDSCREEN = 4,
	FRONT_BUMPER = 5,
	REAR_BUMPER = 6,
};
```