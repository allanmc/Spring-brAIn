#include "global.h"
#include <math.h>

void error_handler (h_location_t line_no, h_string_t message, void *data);

static AIClasses *ai;;

class HuginTest {
	public:
		HuginTest( AIClasses* aiClasses );
		~HuginTest();
		float getBelief(const char* nodeName, const char* stateName);
		void setEvidence(const char* nodeName, const char* stateName);
	
	private:
		void load_and_propagate (h_string_t net_file_name);
		void print_junction_trees (h_domain_t domain);
		void print_beliefs_and_utilities ();
		void print_nodes (h_node_t *list);
		int domain_has_utilities (h_domain_t domain);
		void print_error (void);
		h_domain_t domain;

};