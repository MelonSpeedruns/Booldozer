#include "DOM/MapCollisionDOMNode.hpp"
#include "DOM/RoomDOMNode.hpp"
#include "UIUtil.hpp"
#include "GenUtil.hpp"
#include "imgui.h"
#include "../lib/bStream/bstream.h"

LMapCollisionDOMNode::LMapCollisionDOMNode(std::string name) : Super(name)
{
	mType = EDOMNodeType::MapCollision;
	SetIsRendered(false);
}

std::string LMapCollisionDOMNode::GetName()
{
	return "Collision"; //uh
}

void LMapCollisionDOMNode::RenderHierarchyUI(std::shared_ptr<LDOMNodeBase> self, LEditorSelection* mode_selection)
{
	LUIUtility::RenderCheckBox(this);
	ImGui::SameLine();
	ImGui::Text("Collision");
	if(ImGui::IsItemClicked(0)){
		mode_selection->ClearSelection();
		mode_selection->AddToSelection(self);
	}
	if(mWasRendered != GetIsRendered()){
		mWasRendered = GetIsRendered();
		mDirty = true;
	}
}

void LMapCollisionDOMNode::RenderDetailsUI(float dt)
{
	ImGui::Button("Import OBJ");
}

bool LMapCollisionDOMNode::Load(bStream::CMemoryStream* stream)
{
    
	mGridScale = glm::vec3(stream->readFloat(), stream->readFloat(), stream->readFloat());
	mMinBounds = glm::vec3(stream->readFloat(), stream->readFloat(), stream->readFloat());
	mAxisLengths = glm::vec3(stream->readFloat(), stream->readFloat(), stream->readFloat());

	uint32_t vertexDataOffset = stream->readUInt32();
	uint32_t normalDataOffset = stream->readUInt32();
	uint32_t triangleDataOffset = stream->readUInt32();
	uint32_t triangleGroupOffset = stream->readUInt32();
	uint32_t gridIndexOffset = stream->readUInt32();
	
	//	duplicated for some reason
	gridIndexOffset = stream->readUInt32();

	uint32_t endOffset = stream->readUInt32();

	stream->seek(vertexDataOffset);

	for (size_t i = 0; i < (normalDataOffset - vertexDataOffset) / 0x0C; i++)
	{
		mPositionData.push_back(glm::vec3(stream->readFloat(), stream->readFloat(), stream->readFloat()));
	}
	
	stream->seek(normalDataOffset);
	for (size_t i = 0; i < (triangleDataOffset - normalDataOffset) / 0x0C; i++)
	{
		mNormalData.push_back(glm::vec3(stream->readFloat(), stream->readFloat(), stream->readFloat()));
	}

	stream->seek(triangleDataOffset);
	for (size_t i = 0; i < (triangleGroupOffset - triangleDataOffset) / 0x18; i++)
	{
		std::shared_ptr<LCollisionTriangle> triangle = std::make_shared<LCollisionTriangle>();

		triangle->name = fmt::format("Triangle {}", i);

		triangle->positionIdx[0] = stream->readUInt16();
		triangle->positionIdx[1] = stream->readUInt16();
		triangle->positionIdx[2] = stream->readUInt16();

		triangle->normalIdx = stream->readUInt16();

		triangle->edgeTanIdx[0] = stream->readUInt16();
		triangle->edgeTanIdx[1] = stream->readUInt16();
		triangle->edgeTanIdx[2] = stream->readUInt16();

		triangle->unkIdx = stream->readUInt16();
		triangle->dot = stream->readFloat();
		triangle->enabledMask = stream->readUInt16();
		triangle->friction = stream->readUInt16();

		mTriangles.push_back(triangle);
	}

	std::cout << "[CollisionDOMNode]: Finished reading Collision File" << std::endl;

	return false;
}

bool LMapCollisionDOMNode::Save(bStream::CMemoryStream* stream)
{
	return false;
}

void LMapCollisionDOMNode::PostProcess()
{

}

void LMapCollisionDOMNode::PreProcess()
{

}