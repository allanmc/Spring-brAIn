#ifndef RL_A_H
#define RL_A_H
namespace brainSpace
{
struct RL_Action
	{
		int Action;
		int ID;
		bool Complex;
		RL_Action( int action, int id, bool complex)
		{
			Action = action;
			ID = id;
			Complex = complex;
		}
		RL_Action() {}
		
		bool operator==(const RL_Action &other) const {
			return (ID == other.ID);
		}
	};
}
#endif
