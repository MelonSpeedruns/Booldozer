#pragma once

#include "DOMNodeBase.hpp"
#include "EntityDOMNode.hpp"
#include "io/JmpIO.hpp"
#include "io/StaticMapDataIO.hpp"
#include "ResUtil.hpp"

#include <Archive.hpp>
#include <filesystem>

extern std::string const LEntityFileNames[];

class LRoomDOMNode;

// DOM node representing an entire map, including rooms and objects.
class LMapDOMNode : public LDOMNodeBase
{
	std::shared_ptr<Archive::Rarc> mMapArchive;
	LJmpIO JmpIOManagers[LEntityType_Max];
	LStaticMapDataIO mStaticMapIO;

	bool LoadEntityNodes(LJmpIO* jmp_io, LEntityType type);

	bool ReadStaticData(std::filesystem::path filePath);
	bool LoadStaticData(std::vector<std::shared_ptr<LRoomDOMNode>> rooms);

	bool SaveMirrorData();

public:
	typedef LDOMNodeBase Super;

	LMapDOMNode();
	~LMapDOMNode();

	bool LoadMap(std::filesystem::path file_path);
	bool AppendMap(std::shared_ptr<LMapDOMNode> map);
	bool SaveMapToFiles(std::filesystem::path folder_path);
	bool SaveMapToArchive(std::filesystem::path file_path);
	std::shared_ptr<LRoomDOMNode> GetRoomByNumber(int32_t number);
	std::shared_ptr<LRoomDOMNode> GetRoomByID(int32_t id);
	std::shared_ptr<LRoomDOMNode> GetRoomByIndex(int32_t index);
	std::weak_ptr<Archive::Rarc> GetArchive() { return mMapArchive; };

/*=== Type operations ===*/
	// Returns whether this node is of the given type, or derives from a node of that type.
	virtual bool IsNodeType(EDOMNodeType type) const override
	{
		if (type == EDOMNodeType::Map)
			return true;

		return Super::IsNodeType(type);
	}
};
