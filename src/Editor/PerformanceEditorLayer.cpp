#include "PerformanceEditorLayer.hpp"

#include <Aka/Layer/ImGuiLayer.h>
#include <Aka/Memory/AllocatorTracker.hpp>

namespace app {

using namespace aka;


PerformanceEditorLayer::PerformanceEditorLayer() :
	EditorLayer("Performances")
{
}

void PerformanceEditorLayer::onCreate(aka::Renderer* _renderer)
{
}

void PerformanceEditorLayer::onDestroy(aka::Renderer* _renderer)
{
}
void PerformanceEditorLayer::onDrawUI(aka::DebugDrawList& debugDrawList)
{
	//ImGui::ShowDemoWindow()
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New"))
			{
			}
			if (ImGui::MenuItem("Close"))
			{
			}
			ImGui::Separator();
		}
		ImGui::EndMenuBar();
	}
	if (ImGui::BeginTabBar("Performances"))
	{
		if (ImGui::BeginTabItem("Memory"))
		{
#if defined(AKA_TRACK_MEMORY_ALLOCATIONS)
			AllocatorTracker& tracker = getAllocatorTracker();
			// Compute sums beforehands
			uint32_t memoryTypeSize[EnumCount<AllocatorMemoryType>()];
			uint32_t categorySize[EnumCount<AllocatorMemoryType>()][EnumCount<AllocatorCategory>()];
			struct TypeAllocationInfo
			{
				size_t count;
				size_t elementSize;
				size_t uniqueAllocation;
			};
			HashMap<const std::type_info*, TypeAllocationInfo> memories[EnumCount<AllocatorMemoryType>()][EnumCount<AllocatorCategory>()];
			for (AllocatorMemoryType type : EnumRange<AllocatorMemoryType>())
			{
				memoryTypeSize[EnumToIndex(type)] = 0;
				for (AllocatorCategory category : EnumRange<AllocatorCategory>())
				{
					categorySize[EnumToIndex(type)][EnumToIndex(category)] = 0;
					for (auto& allocation : tracker.get(type, category))
					{
						// Store sum per category
						categorySize[EnumToIndex(type)][EnumToIndex(category)] += static_cast<uint32_t>(allocation.second.elementSize * allocation.second.count);
						auto& memory = memories[EnumToIndex(type)][EnumToIndex(category)];
						auto it = memory.find(allocation.second.info);
						if (it == memory.end())
						{
							memory.insert(std::make_pair(allocation.second.info, TypeAllocationInfo{ allocation.second.count, allocation.second.elementSize, 1 }));
						}
						else
						{
							auto& pair = memory[allocation.second.info];
							AKA_ASSERT(pair.elementSize == allocation.second.elementSize, "");
							pair.count += allocation.second.count;
							pair.uniqueAllocation++;
						}
					}
					memoryTypeSize[EnumToIndex(type)] += categorySize[EnumToIndex(type)][EnumToIndex(category)];
				}
				// Store sum per type
			}
			if (ImGui::BeginTable("Allocations", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings))
			{
				ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHide, 400.0f);
				ImGui::TableSetupColumn("Element size");
				ImGui::TableSetupColumn("Count");
				ImGui::TableSetupColumn("Size");
				ImGui::TableSetupColumn("Unique allocations");
				ImGui::TableHeadersRow();
				for (AllocatorMemoryType type : EnumRange<AllocatorMemoryType>())
				{
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					bool openedMemoryType = ImGui::TreeNodeEx(toString(type), 0, "%s (%u bytes)", toString(type), memoryTypeSize[EnumToIndex(type)]);
					if (openedMemoryType)
					{
						ImGui::TableNextColumn();
						ImGui::TextDisabled("--"); // Element size
						ImGui::TableNextColumn();
						ImGui::TextUnformatted(""); // Count
						ImGui::TableNextColumn();
						ImGui::TextUnformatted(""); // Size
						ImGui::TableNextColumn();
						ImGui::TextUnformatted(""); // Unique allocations
						for (AllocatorCategory category : EnumRange<AllocatorCategory>())
						{
							ImGui::TableNextRow();
							ImGui::TableNextColumn();
							String label = String::format("%s%s", toString(type), toString(category));
							bool openedCategory = ImGui::TreeNodeEx(label.cstr(), 0, "%s (%u bytes)", toString(category), categorySize[EnumToIndex(type)][EnumToIndex(category)]);
							ImGui::TableNextColumn();
							ImGui::TextDisabled("--"); // Element size
							ImGui::TableNextColumn();
							ImGui::TextUnformatted(""); // Count
							ImGui::TableNextColumn();
							ImGui::TextUnformatted(""); // Size
							ImGui::TableNextColumn();
							ImGui::TextUnformatted(""); // Unique allocations
							if (openedCategory)
							{
								ImU32 textColor = ImColor(0.8f, 0.8f, 0.8f);
								for (auto& allocation : memories[EnumToIndex(type)][EnumToIndex(category)])
								{
									ImGui::PushStyleColor(ImGuiCol_Text, textColor);
									ImGui::TableNextRow();
									ImGui::TableNextColumn();
									ImGui::Text("%s", allocation.first->name());
									if (ImGui::IsItemHovered())
									{
										ImGui::BeginTooltip();
										ImGui::Text("%s", allocation.first->name());
										ImGui::EndTooltip();
									}
									ImGui::TableNextColumn();
									ImGui::Text("%llu bytes", allocation.second.elementSize);
									ImGui::TableNextColumn();
									ImGui::Text("%llu", allocation.second.count);
									ImGui::TableNextColumn();
									ImGui::Text("%llu bytes", allocation.second.elementSize* allocation.second.count);
									ImGui::TableNextColumn();
									ImGui::Text("%llu", allocation.second.uniqueAllocation);
									ImGui::PopStyleColor();
								}
								ImGui::TreePop();
							}
						}
						ImGui::TreePop();
					}
				}
				ImGui::EndTable();
			}
#else
			ImGui::Text("Compiled without memory tracking. You need to define AKA_TRACK_MEMORY_ALLOCATIONS at compilation.");
#endif
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("UC"))
		{
			ImGui::Text("Nothing to do here");
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

};