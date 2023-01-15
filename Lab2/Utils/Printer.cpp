#include "Printer.h"
#include "../Core/Model/GraphNodes/NodeBase.h"
#include "../Core/Model/Model.h"

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream> 
#include <map>

void Printer::PrintNodeState(const Model::Nodes::NodeBase& InNode)
{
	std::cout << InNode.GetNodeInfo().str() << std::endl;
	std::cout << "============================================" << std::endl;
}

void Printer::PrintAllNodesState(const std::vector<std::reference_wrapper<Model::Nodes::NodeBase>>& InModelNodes, double ModelSimulationTime)
{
	std::cout << "=========================> " << "Current simulation time: " << std::fixed << std::setprecision(3) << ModelSimulationTime << " <=========================" << std::endl;
	std::for_each(InModelNodes.cbegin(), InModelNodes.cend(),
		[](const Model::Nodes::NodeBase& Node)
		{
			PrintNodeState(Node);
		});

	std::cout << std::endl;

}

void Printer::PrintNodeStatistic(const Model::Nodes::NodeBase& InNode, double ModelSimulationTime)
{
	std::cout << InNode.GetNodeStatistics(ModelSimulationTime).str() << std::endl;
	std::cout << "============================================" << std::endl;
}

void Printer::PrintModelResults(const std::vector<std::reference_wrapper<Model::Nodes::NodeBase>>& InModelNodes, double ModelSimulationTime)
{
	std::cout << "-------------------------> TOTAL STATISTICS <-------------------------" << std::endl;

	std::for_each(InModelNodes.cbegin(), InModelNodes.cend(),
		[ModelSimulationTime](const Model::Nodes::NodeBase& Node)
		{
			PrintNodeStatistic(Node, ModelSimulationTime);
		});
}
