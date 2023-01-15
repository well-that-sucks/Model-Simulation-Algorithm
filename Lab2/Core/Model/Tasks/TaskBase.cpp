#include "TaskBase.h"

#include "../GraphNodes/NodeBase.h"

Model::Tasks::TaskBase::TaskBase() 
	: TaskBase("", 0)
{
}

Model::Tasks::TaskBase::TaskBase(double Time) 
	: TaskBase("", Time)
{
}

Model::Tasks::TaskBase::TaskBase(std::string Type) 
	: TaskBase(Type, 0)
{
}

Model::Tasks::TaskBase::TaskBase(std::string Type, double Time) 
	: m_Type(Type), m_Time(Time), m_TaskSpecificDelayFunction(nullptr)
{
}

Model::Tasks::TaskBase::TaskBase(const TaskBase& InTask)
	: m_Type(InTask.m_Type), m_Time(InTask.m_Time), m_TaskSpecificDelayFunction(InTask.m_TaskSpecificDelayFunction)
{
}

double Model::Tasks::TaskBase::GetInitialTime() const
{
	return m_Time;
}

Model::Tasks::TaskBase& Model::Tasks::TaskBase::SetInitialTime(double InTime)
{
	m_Time = InTime;
	return *this;
}

std::string Model::Tasks::TaskBase::GetType() const
{
	return m_Type;
}

void Model::Tasks::TaskBase::SetType(std::string Type)
{
	m_Type = Type;
}

std::function<double()>& Model::Tasks::TaskBase::GetTaskSpecificDelayFunction()
{
	return m_TaskSpecificDelayFunction;
}

Model::Tasks::TaskBase& Model::Tasks::TaskBase::SetTaskSpecificDelayFunction(std::function<double()>& InFunction)
{
	m_TaskSpecificDelayFunction = InFunction;
	return *this;
}
