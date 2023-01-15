#include "Model.h"
#include "../../Utils/Printer.h"

#include <algorithm>

Model::Model::Model(const std::vector<std::reference_wrapper<Nodes::NodeBase>>& InNodes) : m_Nodes(InNodes)
{
}

Model::Model::Model(const Model& PreDefinedModel) 
	: m_Nodes(PreDefinedModel.m_Nodes)
{
}

Model::Model::Model(Model&& PreDefinedModel) noexcept
	: m_Nodes(std::move(PreDefinedModel.m_Nodes))
{
}

void Model::Model::Simulate(double SimulationTime)
{
	double CurrentTime = 0.0;

	while (CurrentTime < SimulationTime) 
	{
		double MinTimeNext = INFINITY;
		int MinIndex = -1;

		for (int i = 0; i < m_Nodes.size(); ++i)
		{
			if (m_Nodes[i].get().IsWorking() && m_Nodes[i].get().GetNextTime() < MinTimeNext)
			{
				MinTimeNext = m_Nodes[i].get().GetNextTime();
				MinIndex = i;
			}
		}

		if (MinIndex != -1)
		{
			CurrentTime = MinTimeNext;

			if (CurrentTime <= SimulationTime)
			{
				// Update nodes state and propagade model forward
				std::for_each(m_Nodes.begin(), m_Nodes.end(),
					[&CurrentTime](Nodes::NodeBase& Node)
					{
						Node.SetCurrentTime(CurrentTime);
					});

				// Invoking OutAction for each node with the same TimeNext in case there are multiple such nodes
				std::for_each(m_Nodes.begin(), m_Nodes.end(), [&CurrentTime](Nodes::NodeBase& Node) { if (Node.IsWorking() && Node.GetNextTime() == CurrentTime) { Node.OutAction(); } });

				// Post update nodes after current iteration
				std::for_each(m_Nodes.begin(), m_Nodes.end(), [](Nodes::NodeBase& Node) { Node.ExecutePostTickFunction(); });

				Printer::PrintAllNodesState(m_Nodes, CurrentTime);
			}
		}
		else
		{
			// Model is hung.
		}
	}

	Printer::PrintModelResults(Model::GetModelNodes(), SimulationTime);
}

const std::vector<std::reference_wrapper<Model::Nodes::NodeBase>>& Model::Model::GetModelNodes() const
{
	return m_Nodes;
}
