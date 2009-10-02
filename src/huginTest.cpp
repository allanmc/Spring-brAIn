

#include "HuginTest.h"

/* A simple parse error handler: It prints the error message on stderr. */

void error_handler (h_location_t line_no, h_string_t message, void *data)
{
    if (ai!=NULL) 
	{
		ai->utility->Log(ALL, BN, "Error: Error at line %d: %s\n", (int) line_no, message);
	}
}


HuginTest::HuginTest( AIClasses* aiClasses )
{
	ai = aiClasses;
	char file1[100];
	char file2[100];
	const char *dir = DataDirs::GetInstance(ai->callback)->GetConfigDir();
	strcpy(file1, dir);
	strcpy(file2, dir);
	load_and_propagate( strcat(file1,"\\uber.net") );
}

void HuginTest::load_and_propagate (h_string_t net_file_name)
{
	size_t l = strlen (net_file_name);
	char *file_name_buffer;
	h_domain_t domain;
	FILE *log_file;

	if (l >= 4 && strcmp (net_file_name + (l - 4), ".net") == 0)
	{
		l -= 4;
	}

	ai->utility->Log(ALL, BN, "Error: Loading!?!?!\n");
	ai->utility->Log(ALL, BN, "Output: Hello World\n");
	if ((file_name_buffer = (char*)malloc (l + 5)) == NULL)
	{
		ai->utility->Log(ALL, BN, "Error: Out of memory\n");
		return;
	}

	strcpy (file_name_buffer, net_file_name);
	strcpy (file_name_buffer + l, ".net");

	if ((domain = h_net_parse_domain (file_name_buffer, error_handler, NULL)) == NULL)
	{
		print_error ();
		return;
	}

	strcpy (file_name_buffer + l, ".log");

	if ((log_file = fopen (file_name_buffer, "w")) == NULL)
	{
		ai->utility->Log(ALL, BN, "Error: Could not open \"%s\"\n", file_name_buffer);
		exit (EXIT_FAILURE);
	}

	free (file_name_buffer);

	h_domain_set_log_file (domain, log_file);

	if (h_domain_triangulate (domain, h_tm_fill_in_weight) != 0)
	{
		print_error ();
		return;
	}

	if (h_domain_compile (domain) != 0)
	{
		print_error ();
		return;
	}

	h_domain_set_log_file (domain, NULL);

	fclose (log_file);

	print_junction_trees (domain);

	if (!domain_has_utilities (domain))
	{
		ai->utility->Log(ALL, BN, "\n\nPrior beliefs:\n");
	}
	else
	{
		if (h_domain_update_policies (domain) != 0)
		{
			print_error ();
			return;
		}

		ai->utility->Log(ALL, BN, "\n\nOverall expected utility: %g\n", h_domain_get_expected_utility (domain));

		ai->utility->Log(ALL, BN, "\nPrior beliefs (and expected utilities):\n");
	}

	print_beliefs_and_utilities (domain);

	h_domain_delete (domain);
}

/* This function is used when a Hugin API error is detected: It prints
   an error message on stderr and exits the program.
*/

void HuginTest::print_error (void)
{
    ai->utility->Log(ALL, BN, "Error: Error: %s\n", h_error_description (h_error_code ()));
    exit (EXIT_FAILURE);
}

void HuginTest::print_junction_trees (h_domain_t domain)
{
	h_junction_tree_t jt = h_domain_get_first_junction_tree (domain);

	if (jt == NULL)
	{
		print_error ();
		return;
	}

	for (; jt != NULL; jt = h_jt_get_next (jt))
	{
		h_clique_t *list = h_jt_get_cliques (jt);

		if (list == NULL)
		{
			print_error ();
			return;
		}

		ai->utility->Log(ALL, BN, "Junction tree:\n");

		for (; *list != NULL; list++)
		{
			h_node_t *members = h_clique_get_members (*list);

			if (members == NULL)
			{
				print_error ();
				return;
			}

			ai->utility->Log(ALL, BN, "  Clique:");
			print_nodes (members);
		} 
	}
}


/* Print the beliefs and expected utilities of all nodes in the domain. */

void HuginTest::print_beliefs_and_utilities (h_domain_t domain)
{
	int has_utilities = domain_has_utilities (domain);
	h_node_t node = h_domain_get_first_node (domain);

	for (; node != NULL; node = h_node_get_next (node))
	{
		h_node_category_t category = h_node_get_category (node);
		char type = (category == h_category_chance ? 'C' : category == h_category_decision ? 'D' : 'U');

		ai->utility->Log(ALL, BN, "\n[%c] %s (%s)\n", type, h_node_get_label (node), h_node_get_name (node));

		if (category == h_category_utility)
		{
			ai->utility->Log(ALL, BN, "  - Expected utility: %g\n", h_node_get_expected_utility (node, 0));
		}
		else if (h_node_get_kind (node) == h_kind_discrete)
		{
			h_count_t state_count = h_node_get_number_of_states (node);
			size_t i;

			if (has_utilities)
			{
				for (i = 0; i < state_count; i++)
				{
					ai->utility->Log(ALL, BN, "  - %s %g (%g)\n",
					h_node_get_state_label (node, i),
					h_node_get_belief (node, i),
					h_node_get_expected_utility (node, i));
				}
			}
			else
			{
				for (i = 0; i < state_count; i++)
				{
					ai->utility->Log(ALL, BN, "  - %s %g\n",
					h_node_get_state_label (node, i),
					h_node_get_belief (node, i));
				}
			}
		}
		else  /* "node" is a continuous chance node */
		{
			ai->utility->Log(ALL, BN, "  - Mean : %g\n", h_node_get_mean (node));
			ai->utility->Log(ALL, BN, "  - SD   : %g\n", sqrt (h_node_get_variance (node)));
		}
	}
}


/* Print the names of all nodes in the list. */

void HuginTest::print_nodes (h_node_t *list)
{
	for (; *list != NULL; list++)
	{
		h_string_t name = h_node_get_name (*list);

		if (name == NULL)
		{
			print_error ();
			return;
		}

		ai->utility->Log(ALL, BN, " %s", name);
	}

	ai->utility->Log(ALL, BN, "\n");
}


/* Are there utility nodes in "domain"? */

int HuginTest::domain_has_utilities (h_domain_t domain)
{
	h_node_t node = h_domain_get_first_node (domain);

	for (; node != NULL; node = h_node_get_next (node))
	{
		if (h_node_get_category (node) == h_category_utility)
		{
			return 1;
		}
	}
	return 0;
}
