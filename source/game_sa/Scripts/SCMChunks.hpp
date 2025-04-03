#pragma once

#pragma pack(push, 1)
struct tSCMChunkHeader {
    uint8  m_InstrGoTo[3]; // 02 00 01 (GOTO <int32>)
    uint32 m_NextChunkOffset;
    uint8  m_ChunkIndex;

	template<typename ChunkT = tSCMChunkHeader> requires std::is_base_of_v<tSCMChunkHeader, ChunkT>
	ChunkT* As() {
		assert(m_ChunkIndex == ChunkT::Index);
		return (ChunkT*)this;
	}

	template<typename ChunkT = tSCMChunkHeader> requires std::is_base_of_v<tSCMChunkHeader, ChunkT>
	const ChunkT* As() const {
		assert(m_ChunkIndex == ChunkT::Index);
		return (ChunkT*)this;
	}
};
VALIDATE_SIZE(tSCMChunkHeader, 0x8);

// m_ChunkIndex = 's' -- for San Andreas
struct tSCMGlobalVarChunk : tSCMChunkHeader {
    static constexpr uint8 Index = 's';

	uint8 m_GlobalVarSpace[];

	template<typename T>
		requires std::is_integral_v<T> || notsa::is_any_of_type_v<T, float, CVector, CVector2D>
	const T& ReadVariable(size_t offset) {
		return *reinterpret_cast<T*>(m_GlobalVarSpace[offset - 8]);
	}
};
VALIDATE_SIZE(tSCMGlobalVarChunk, 0x8 /* + m_NextChunkOffset - 8 */);

// m_ChunkIndex = 0
struct tSCMUsedObjectsChunk : tSCMChunkHeader {
	using UsedObjectName = char[24];

    static constexpr uint8 Index = 0;

	uint32 m_NumberOfUsedObjects;
	UsedObjectName m_UsedObjectNames[];

	auto GetObjectNames() const {
		return std::span<const UsedObjectName>{ m_UsedObjectNames, m_NumberOfUsedObjects };
	}
};
VALIDATE_SIZE(tSCMUsedObjectsChunk, 0xC /* + m_NumberOfUsedObjects * 24 */);

// m_ChunkIndex = 1
struct tSCMScriptFileInfoChunk : tSCMChunkHeader {
    static constexpr uint8 Index = 1;

	uint32 m_MainScriptSize;
	uint32 m_LargestMissionScriptSize;
	uint16 m_NumberOfMissionScripts;
	uint16 m_NumberOfExclusiveMissionScripts;
	uint32 m_LargestNumberOfMissionScriptLocalVars;

	uint32 m_MissionScriptOffsets[];

	auto GetMissionOffsets() const {
		return std::span<const uint32>{ m_MissionScriptOffsets, m_NumberOfMissionScripts };
	}
};
VALIDATE_SIZE(tSCMScriptFileInfoChunk, 0x18 /* + 4 * m_NumberOfMissionScripts */);

// m_ChunkIndex = 2
struct tSCMStreamedScriptFileInfoChunk : tSCMChunkHeader {
	struct StreamScriptDefinition {
		char   m_FileName[20];
		uint32 m_FileOffset;
		uint32 m_Size;
	};

    static constexpr uint8 Index = 2;

	uint32 m_LargestStreamScriptSize;
	uint32 m_NumberOfStreamedScripts;

	StreamScriptDefinition m_StreamedScriptDefs[];

	auto GetStreamedScripts() const {
		return std::span<const StreamScriptDefinition>{ m_StreamedScriptDefs, m_NumberOfStreamedScripts };
	}
};
VALIDATE_SIZE(tSCMStreamedScriptFileInfoChunk, 0x10 /* + 28 * m_NumberOfStreamedScripts */);
VALIDATE_SIZE(tSCMStreamedScriptFileInfoChunk::StreamScriptDefinition, 28);

// m_ChunkIndex = 3
struct tSCMUnknownChunk3 : tSCMChunkHeader {
    static constexpr uint8 Index = 3;

	uint32 unk;
};
VALIDATE_SIZE(tSCMUnknownChunk3, 0xC);

// m_ChunkIndex = 4
struct tSCMExtraInfoChunk : tSCMChunkHeader {
    static constexpr uint8 Index = 4;

	uint32 m_GlobalVarSpaceSize;
	uint32 m_BuildNumber; // ?
};
VALIDATE_SIZE(tSCMExtraInfoChunk, 0x10);
#pragma pack(pop)
