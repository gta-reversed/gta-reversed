#pragma once

class CCustomBuildingPipeline {
public:
    static RxPipeline*& ObjPipeline;

public:
    static void InjectHooks();

    static bool        CreatePipe();
    static void        DestroyPipe();
    static RpAtomic*   CustomPipeAtomicSetup(RpAtomic* atomic);
    static RxPipeline* CreateCustomObjPipe();

private:
    static RpMaterial* CustomPipeMaterialSetup(RpMaterial* material, void* data);
    static void CustomPipeRenderCB(RwResEntry* repEntry, void* object, RwUInt8 type, RwUInt32 flags);
    static RwBool CustomPipeReinstanceCB(void* object, RwResEntry* resEntry, RxD3D9AllInOneInstanceCallBack instanceCallback);
};
