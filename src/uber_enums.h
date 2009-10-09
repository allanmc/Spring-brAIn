namespace node
{
	enum node_ids
	{
		_seenUnits = 0,
		_myStrategy = 1,
		_seenRes = 2,
		_seenProd = 3,
		_attacks = 4,
		_seenDef = 5,
		_enemyResource = 6,
		_enemyProduction = 7,
		_enemyDefence = 8,
		_enemyUnits = 9,
		_enemyStrategy = 10
	};
	namespace seenUnits
	{
		enum State
		{
			_0 = 0,
			_1_10 = 1,
			_11_50 = 2,
			_51_300 = 3
		};
	}
	namespace myStrategy
	{
		enum State
		{
			_Aggressive = 0,
			_Defensive = 1
		};
	}
	namespace seenRes
	{
		enum State
		{
			_0 = 0,
			_1_10 = 1,
			_11_20 = 2,
			_21_40 = 3
		};
	}
	namespace seenProd
	{
		enum State
		{
			_0 = 0,
			_1_2 = 1,
			_3_6 = 2,
			_7_10 = 3
		};
	}
	namespace attacks
	{
		enum State
		{
			_0 = 0,
			_1_3 = 1,
			_4_8 = 2,
			_9_ = 3
		};
	}
	namespace seenDef
	{
		enum State
		{
			_0 = 0,
			_1_5 = 1,
			_6_10 = 2,
			_11_20 = 3
		};
	}
	namespace enemyResource
	{
		enum State
		{
			_0 = 0,
			_1_10 = 1,
			_11_20 = 2,
			_21_40 = 3
		};
	}
	namespace enemyProduction
	{
		enum State
		{
			_0 = 0,
			_1_2 = 1,
			_3_6 = 2,
			_7_10 = 3
		};
	}
	namespace enemyDefence
	{
		enum State
		{
			_0 = 0,
			_1_5 = 1,
			_6_10 = 2,
			_11_20 = 3
		};
	}
	namespace enemyUnits
	{
		enum State
		{
			_0 = 0,
			_1_10 = 1,
			_11_50 = 2,
			_51_300 = 3
		};
	}
	namespace enemyStrategy
	{
		enum State
		{
			_Aggressive = 0,
			_Defensive = 1
		};
	}
}

