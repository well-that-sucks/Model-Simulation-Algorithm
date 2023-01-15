#pragma once

#include "NodeBase.h"
#include "../Tasks/TaskBase.h"

#include <functional>
#include <vector>
#include <map>

namespace Model
{
	namespace Nodes
	{
		class NodeTransition : public NodeBase
		{
		public:
			NodeTransition(std::string Name);

			void InAction(std::shared_ptr<Tasks::TaskBase> InTask) override;
			void OutAction() override;

			double GetNextTime() const override;

			int GetQueueSize() const override;

			std::ostringstream GetNodeInfo() const override;

			bool IsAvailable() const override;

		private:
			struct SubProcess
			{
				std::shared_ptr<Tasks::TaskBase> Task;
				double TimeNext;
			};

		protected:
			void UpdateNextTime(const std::shared_ptr<Tasks::TaskBase>& ExecutedTask) override;

		private:
			SubProcess& GetMinTimeSubprocess() noexcept;

		private:
			std::vector<SubProcess> m_Subprocesses;

			std::function<std::shared_ptr<Tasks::TaskBase>&(std::deque<std::shared_ptr<Tasks::TaskBase>>&)> m_GetNextTask;
		};
	}
}