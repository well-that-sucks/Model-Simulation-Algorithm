#pragma once

#include <vector>

namespace Model
{
	class Model;

	namespace Nodes
	{
		class NodeBase;
	}
}

class Printer
{
public:
	static void PrintNodeState(const Model::Nodes::NodeBase& InNode);
	static void PrintAllNodesState(const std::vector<std::reference_wrapper<Model::Nodes::NodeBase>>& InModelNodes, double ModelSimulationTime);
	static void PrintNodeStatistic(const Model::Nodes::NodeBase& InNode, double ModelSimulationTime);
	static void PrintModelResults(const std::vector<std::reference_wrapper<Model::Nodes::NodeBase>>& InModelNodes, double ModelSimulationTime);
};