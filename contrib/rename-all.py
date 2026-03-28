#
# Should be ran from the `/source` directory
# replace the values below you want to rename
#

import asyncio
from asyncio.subprocess import create_subprocess_exec

async def process_one_rename(old: str, new: str):
    process = await create_subprocess_exec(
        "python",
        "../contrib/unscoped-enum-to-scoped-rename.py",
        "tComponent",
        old,
        new,
    )
    await process.wait()

async def main():
    for old, new in (
        ("COMPONENT_NA", "ENGINE"),
        ("COMPONENT_WHEEL_LF", "WHEEL_FRONT_LEFT"),
        ("COMPONENT_WHEEL_RF", "WHEEL_FRONT_RIGHT"),
        ("COMPONENT_WHEEL_LR", "WHEEL_REAR_LEFT"),
        ("COMPONENT_WHEEL_RR", "WHEEL_REAR_RIGHT"),
        ("COMPONENT_BONNET", "DOOR_BONNET"),
        ("COMPONENT_BOOT", "DOOR_BOOT"),
        ("COMPONENT_DOOR_LF", "DOOR_FRONT_LEFT"),
        ("COMPONENT_DOOR_RF", "DOOR_FRONT_RIGHT"),
        ("COMPONENT_DOOR_LR", "DOOR_REAR_LEFT"),
        ("COMPONENT_DOOR_RR", "DOOR_REAR_RIGHT"),
        ("COMPONENT_WING_LF", "PANEL_FRONT_LEFT"),
        ("COMPONENT_WING_RF", "PANEL_FRONT_RIGHT"),
        ("COMPONENT_WING_LR", "PANEL_REAR_LEFT"),
        ("COMPONENT_WING_RR", "PANEL_REAR_RIGHT"),
        ("COMPONENT_WINDSCREEN", "PANEL_WINDSCREEN"),
        ("COMPONENT_BUMP_FRONT", "PANEL_FRONT_BUMPER"),
        ("COMPONENT_BUMP_REAR", "PANEL_REAR_BUMPER"),
    ):
        print(f"Renaming {old} to {new}...")
        await process_one_rename(old, new)

if __name__ == "__main__":
    asyncio.run(main())
