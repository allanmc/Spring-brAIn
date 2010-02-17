#ifndef RL_A_H
#define RL_A_H
namespace brainSpace
{
struct RL_Action
	{
		int Action;
		int ID;
		RL_Action( int action, int id)
		{
			Action = action;
			ID = id;
		}
		RL_Action() {}
		
		bool operator==(const RL_Action &other) const {
			return (ID == other.ID);
		}
	};
}
#endif
