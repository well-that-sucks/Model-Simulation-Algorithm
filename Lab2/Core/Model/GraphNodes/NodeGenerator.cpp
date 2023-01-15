#include "NodeGenerator.h"
#include "../Tasks/TaskBase.h"
#include "../../../Utils/MathUtils.h"

Model::Nodes::NodeGenerator::NodeGenerator(std::string NodeName)
	: NodeBase(NodeName)
{
}

void Model::Nodes::NodeGenerator::OutAction()
{
	NodeBase::OutAction();

	std::shared_ptr<Tasks::TaskBase> NewTask = std::make_shared<Tasks::TaskBase>(std::move(GenerateTask().SetInitialTime(m_CurrentTime)));
	if (std::optional<std::reference_wrapper<NodeBase>> NextNode = GetNextNode(NewTask))
	{
		NextNode->get().InAction(NewTask);
		++m_StatisticsData.TotalPasses;
	}

	UpdateNextTime(NewTask);
}

std::optional<std::reference_wrapper<Model::Nodes::NodeBase>> Model::Nodes::NodeGenerator::GetNextNode(const std::shared_ptr<Tasks::TaskBase>& ExecutedTask)
{
	// If overriding function is set - use it (delegate to the parent method which knows how to handle it)
	if (std::optional<std::reference_wrapper<NodeBase>> NextNode = NodeBase::GetNextNode(ExecutedTask))
	{
		return NextNode;
	}

	// Default behavior otherwise
	return std::nullopt;
}

void Model::Nodes::NodeGenerator::SetTaskDistribution(std::vector<std::pair<Tasks::TaskBase, double>> InDistribution)
{
	m_TaskDistribution = std::move(InDistribution);
	TransformProbabilities();
}

bool Model::Nodes::NodeGenerator::IsAvailable() const
{
	return false;
}

int Model::Nodes::NodeGenerator::GetQueueSize() const
{
	return 0;
}

void Model::Nodes::NodeGenerator::TransformProbabilities()
{
	std::vector<std::pair<Tasks::TaskBase, double>>::iterator VecIter = m_TaskDistribution.begin() + 1;

	for (VecIter; VecIter != m_TaskDistribution.end(); ++VecIter)
	{
		VecIter->second += (VecIter - 1)->second;
	}
}

Model::Tasks::TaskBase Model::Nodes::NodeGenerator::GenerateTask()
{
	if (!m_TaskDistribution.empty())
	{
		std::vector<std::pair<Tasks::TaskBase, double>>::iterator VecIter = m_TaskDistribution.begin();

		const double RandNum = MathUtils::GetRandomNumberNormalized();

		while (VecIter != m_TaskDistribution.end() && (RandNum > VecIter->second))
		{
			++VecIter;
		}

		if (VecIter != m_TaskDistribution.end())
		{
			return VecIter->first;
			//return Tasks::TaskBase(VecIter->first.GetType(), VecIter->first.GetInitialTime()).SetTaskSpecificDelayFunction(VecIter->SetTask);
		}
	}

	return Tasks::TaskBase(m_CurrentTime);
}
