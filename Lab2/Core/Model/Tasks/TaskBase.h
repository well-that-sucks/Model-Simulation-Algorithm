#pragma once

#include <functional>
#include <string>
#include <stack>

namespace Model
{
	namespace Nodes
	{
		class NodeBase;
	}

	namespace Tasks
	{
		class TaskBase
		{
		public:
			TaskBase();
			TaskBase(double Time);
			TaskBase(std::string Type);
			TaskBase(std::string Type, double Time);
			TaskBase(const TaskBase& InTask);

			double GetInitialTime() const;
			TaskBase& SetInitialTime(double InTime);

			std::string GetType() const;
			void SetType(std::string Type);

			std::function<double()>& GetTaskSpecificDelayFunction();
			TaskBase& SetTaskSpecificDelayFunction(std::function<double()>& InFunction);

		protected:
			std::string m_Type;
			double m_Time;

			std::function<double()> m_TaskSpecificDelayFunction;
		};
	}
}
