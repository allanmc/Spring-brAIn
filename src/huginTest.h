#include "global.h"
#include <math.h>

void error_handler (h_location_t line_no, h_string_t message, void *data);

class HuginTest {
	public:
		HuginTest( AIClasses* aiClasses );
		void load_and_propagate (h_string_t net_file_name, h_string_t case_file_name);
		void print_junction_trees (h_domain_t domain);
		void print_beliefs_and_utilities (h_domain_t domain);
		void print_nodes (h_node_t *list);
		int domain_has_utilities (h_domain_t domain);
		void print_error (void);

	private:
		AIClasses *ai;


};