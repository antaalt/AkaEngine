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
String formatBytes(uint64_t bytes)
{
	char* suffix[] = { "B", "KB", "MB", "GB", "TB" };
	size_t length = sizeof(suffix) / sizeof(suffix[0]);

	size_t i = 0;
	float bytesFloat = (float)bytes;
	if (bytes > 1000) 
	{
		for (i = 0; (bytes / 1000) > 0 && i < length - 1; i++, bytes /= 1000)
			bytesFloat = bytes / 1000.f;
	}
	return String::format("%.02lf %s", bytesFloat, suffix[i]);
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
			uint32_t categorySize[EnumCount<AllocatorCategory>()] = { 0 };
			uint32_t memoryTypeSize[EnumCount<AllocatorCategory>()][EnumCount<AllocatorMemoryType>()] = { 0 };
			struct TypeAllocationInfo
			{
				size_t count;
				size_t elementSize;
				size_t uniqueAllocation;
			};
			// This structure should be hold in allocation tracker & updated at allocations instead.
			HashMap<const std::type_info*, TypeAllocationInfo> memories[EnumCount<AllocatorCategory>()][EnumCount<AllocatorMemoryType>()];
			for (AllocatorCategory category : EnumRange<AllocatorCategory>())
			{
				for (AllocatorMemoryType type : EnumRange<AllocatorMemoryType>())
				{
					for (auto& allocation : tracker.get(type, category))
					{
						// Store sum per category
						memoryTypeSize[EnumToIndex(category)][EnumToIndex(type)] += static_cast<uint32_t>(allocation.second.elementSize * allocation.second.count);
						auto& memory = memories[EnumToIndex(category)][EnumToIndex(type)];
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
					categorySize[EnumToIndex(category)] += memoryTypeSize[EnumToIndex(category)][EnumToIndex(type)];
				}
				// Store sum per type
			}
			ImGui::Text("Allocator tracker is using %s", formatBytes(tracker.getUsedMemory()).cstr());
			if (ImGui::BeginTable("Allocations", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings))
			{
				ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHide, 400.0f);
				ImGui::TableSetupColumn("Size");
				ImGui::TableSetupColumn("Element Size");
				ImGui::TableSetupColumn("Count");
				ImGui::TableSetupColumn("Unique allocations");
				ImGui::TableHeadersRow();
				for (AllocatorCategory category : EnumRange<AllocatorCategory>())
				{
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					bool openedMemoryType = ImGui::TreeNodeEx(toString(category), 0, "%s", toString(category));
					ImGui::TableNextColumn();
					ImGui::Text("%s", formatBytes(categorySize[EnumToIndex(category)]).cstr()); // Size
					ImGui::TableNextColumn();
					ImGui::TextDisabled("--"); // ElementSize
					ImGui::TableNextColumn();
					ImGui::TextDisabled("--"); // Count
					ImGui::TableNextColumn();
					ImGui::TextDisabled("--"); // Unique allocations
					if (openedMemoryType)
					{
						for (AllocatorMemoryType type : EnumRange<AllocatorMemoryType>())
						{
							ImGui::TableNextRow();
							ImGui::TableNextColumn();
							String label = String::format("%s%s", toString(category), toString(type));
							bool openedCategory = ImGui::TreeNodeEx(label.cstr(), 0, "%s", toString(type));
							ImGui::TableNextColumn();
							ImGui::Text("%s", formatBytes(memoryTypeSize[EnumToIndex(category)][EnumToIndex(type)]).cstr()); // Size
							ImGui::TableNextColumn();
							ImGui::TextDisabled("--"); // ElementSize
							ImGui::TableNextColumn();
							ImGui::TextDisabled("--"); // Count
							ImGui::TableNextColumn();
							ImGui::TextDisabled("--"); // Unique allocations
							if (openedCategory)
							{
								ImU32 textColor = ImColor(0.8f, 0.8f, 0.8f);
								for (auto& allocation : memories[EnumToIndex(category)][EnumToIndex(type)])
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
									ImGui::Text("%s", formatBytes(allocation.second.elementSize * allocation.second.count).cstr());
									ImGui::TableNextColumn();
									ImGui::Text("%s", formatBytes(allocation.second.elementSize).cstr());
									ImGui::TableNextColumn();
									ImGui::Text("%llu", allocation.second.count);
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