#include <algorithm>
#include <list>

#include "NodeTransition.h"

Model::Nodes::NodeTransition::NodeTransition(std::string Name)
	: NodeBase(Name)
{
	m_bIsWorking = false;
}

void Model::Nodes::NodeTransition::InAction(std::shared_ptr<Tasks::TaskBase> InTask)
{
	NodeBase::InAction(InTask);

	SubProcess& SubprocessEntry = m_Subprocesses.emplace_back(InTask);
	UpdateNextTime(SubprocessEntry.Task);
}

void Model::Nodes::NodeTransition::OutAction()
{
	NodeBase::OutAction();

	if (m_Subprocesses.empty())
	{
		return;
	}

	SubProcess& MinTimeSubprocess = GetMinTimeSubprocess();

	if (std::optional<std::reference_wrapper<NodeBase>> NextNode = GetNextNode(MinTimeSubprocess.Task))
	{
		NextNode->get().InAction(MinTimeSubprocess.Task);
		++m_StatisticsData.TotalPasses;
		std::erase_if(m_Subprocesses, [&MinTimeSubprocess](const SubProcess& Entry) { return Entry.Task == MinTimeSubprocess.Task; }); // deleting task with smallest TimeNext
	}
	else
	{
		++m_StatisticsData.TotalPacketsLeavingModel;
		//m_SpecificStatisticsData.TotalTaskLifetime += m_CurrentTime - MinTimeSubprocess.Task->GetInitialTime();
	}

	if (!m_Subprocesses.empty())
	{
		UpdateNextTime(GetMinTimeSubprocess().Task);
	}
	else
	{
		m_bIsWorking = false;
	}
}

double Model::Nodes::NodeTransition::GetNextTime() const
{
	double MinTimeNext = INFINITY;
	int MinIndex = -1;

	for (int i = 0; i < m_Subprocesses.size(); ++i)
	{
		if (m_Subprocesses[i].Task && m_Subprocesses[i].TimeNext < MinTimeNext)
		{
			MinTimeNext = m_Subprocesses[i].TimeNext;
			MinIndex = i;
		}
	}

	return MinTimeNext;
}

int Model::Nodes::NodeTransition::GetQueueSize() const
{
	return 0;
}

std::ostringstream Model::Nodes::NodeTransition::GetNodeInfo() const
{
	return NodeBase::GetNodeInfo();
}

bool Model::Nodes::NodeTransition::IsAvailable() const
{
	return true;
}

void Model::Nodes::NodeTransition::UpdateNextTime(const std::shared_ptr<Tasks::TaskBase>& ExecutedTask)
{
	std::vector<SubProcess>::iterator SubprocessIter = std::find_if(m_Subprocesses.begin(), m_Subprocesses.end(),
		[&ExecutedTask](const SubProcess& Entry)
		{
			return Entry.Task == ExecutedTask;
		}
	);

	if (SubprocessIter != m_Subprocesses.end())
	{
		SubProcess& SubprocessEntry = *SubprocessIter;
		SubprocessEntry.TimeNext = m_CurrentTime + m_DelayFunction(ExecutedTask);
		m_NextTime = GetNextTime();
		m_bIsWorking = true;
	}
}

Model::Nodes::NodeTransition::SubProcess& Model::Nodes::NodeTransition::GetMinTimeSubprocess() noexcept
{
	double MinTimeNext = INFINITY;
	int MinIndex = -1;

	for (int i = 0; i < m_Subprocesses.size(); ++i)
	{
		if (m_Subprocesses[i].Task && (m_Subprocesses[i].TimeNext < MinTimeNext))
		{
			MinTimeNext = m_Subprocesses[i].TimeNext;
			MinIndex = i;
		}
	}

	return m_Subprocesses[MinIndex];
}
