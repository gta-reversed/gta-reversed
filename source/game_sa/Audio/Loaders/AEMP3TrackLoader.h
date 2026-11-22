#pragma once
#include "AETrackLoader.h"

class CAEMP3TrackLoader : public CAETrackLoader {
public:
    static void InjectHooks();

    CAEMP3TrackLoader();
    ~CAEMP3TrackLoader();

    bool Initialise();
    bool LoadStreamPackTable(void);
    bool LoadTrackLookupTable(void);
    tTrackInfo* GetTrackInfo(uint32 trackId);

    // Checks for DVD stream.
    bool IsCurrentAudioStreamAvailable();

private:
    // NOTSA
    CAEMP3TrackLoader* Constructor() {
        this->CAEMP3TrackLoader::CAEMP3TrackLoader();
        return this;
    }

    void Deconstructor() { this->~CAEMP3TrackLoader(); }
};
