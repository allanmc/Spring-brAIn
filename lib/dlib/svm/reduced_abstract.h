// Copyright (C) 2008  Davis E. King (davisking@users.sourceforge.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#undef DLIB_REDUCED_TRAINERs_ABSTRACT_
#ifdef DLIB_REDUCED_TRAINERs_ABSTRACT_

#include "../matrix.h"
#include "../algs.h"
#include "function_abstract.h"
#include "kernel_abstract.h"
#include "../optimization.h"

namespace dlib
{

// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------

    template <
        typename trainer_type 
        >
    class reduced_decision_function_trainer
    {
        /*!
            REQUIREMENTS ON trainer_type
                - trainer_type == some kind of batch trainer object (e.g. svm_nu_trainer)

            WHAT THIS OBJECT REPRESENTS
                This object represents an implementation of a reduced set algorithm
                for support vector decision functions.  This object acts as a post
                processor for anything that creates decision_function objects.  It
                wraps another trainer object and performs this reduced set post 
                processing with the goal of representing the original decision 
                function in a form that involves fewer support vectors.
        !*/

    public:
        typedef typename trainer_type::kernel_type kernel_type;
        typedef typename trainer_type::scalar_type scalar_type;
        typedef typename trainer_type::sample_type sample_type;
        typedef typename trainer_type::mem_manager_type mem_manager_type;
        typedef typename trainer_type::trained_function_type trained_function_type;

        reduced_decision_function_trainer (
        );
        /*!
            ensures
                - This object is in an uninitialized state.  You must
                  construct a real one with the other constructor and assign it
                  to this instance before you use this object.
        !*/

        reduced_decision_function_trainer (
            const trainer_type& trainer,
            const unsigned long num_sv 
        );
        /*!
            requires
                - num_sv > 0
            ensures
                - returns a trainer object that applies post processing to the decision_function
                  objects created by the given trainer object with the goal of creating
                  decision_function objects with fewer support vectors.
                - The reduced decision functions that are output will have at most
                  num_sv support vectors.
        !*/

        template <
            typename in_sample_vector_type,
            typename in_scalar_vector_type
            >
        const decision_function<kernel_type> train (
            const in_sample_vector_type& x,
            const in_scalar_vector_type& y
        ) const;
        /*!
            ensures
                - trains a decision_function using the trainer that was supplied to
                  this object's constructor and then finds a reduced representation
                  for it and returns the reduced version.  
            throws
                - std::bad_alloc
                - any exceptions thrown by the trainer_type object
        !*/

    }; 

// ----------------------------------------------------------------------------------------

    template <
        typename trainer_type
        >
    const reduced_decision_function_trainer<trainer_type> reduced (
        const trainer_type& trainer,
        const unsigned long num_sv
    ) { return reduced_decision_function_trainer<trainer_type>(trainer, num_sv); }
    /*!
        requires
            - num_sv > 0
            - trainer_type == some kind of batch trainer object that creates decision_function
              objects (e.g. svm_nu_trainer)
        ensures
            - returns a reduced_decision_function_trainer object that has been
              instantiated with the given arguments.
    !*/

// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------

    template <
        typename trainer_type 
        >
    class reduced_decision_function_trainer2
    {
        /*!
            REQUIREMENTS ON trainer_type
                - trainer_type == some kind of batch trainer object (e.g. svm_nu_trainer)
                - trainer_type::sample_type must be a dlib::matrix object
                - kernel_derivative<trainer_type::kernel_type> must be defined

            WHAT THIS OBJECT REPRESENTS
                This object represents an implementation of a reduced set algorithm
                for support vector decision functions.  This object acts as a post
                processor for anything that creates decision_function objects.  It
                wraps another trainer object and performs this reduced set post 
                processing with the goal of representing the original decision 
                function in a form that involves fewer support vectors.

                This object's implementation is the same as that in the above
                reduced_decision_function_trainer object except it also performs 
                a global gradient based optimization at the end to further
                improve the approximation to the original decision function
                object. 
        !*/

    public:
        typedef typename trainer_type::kernel_type kernel_type;
        typedef typename trainer_type::scalar_type scalar_type;
        typedef typename trainer_type::sample_type sample_type;
        typedef typename trainer_type::mem_manager_type mem_manager_type;
        typedef typename trainer_type::trained_function_type trained_function_type;

        reduced_decision_function_trainer2 (
        );
        /*!
            ensures
                - This object is in an uninitialized state.  You must
                  construct a real one with the other constructor and assign it
                  to this instance before you use this object.
        !*/

        reduced_decision_function_trainer2 (
            const trainer_type& trainer,
            const unsigned long num_sv,
            double eps = 1e-3
        );
        /*!
            requires
                - num_sv > 0
                - eps > 0
            ensures
                - returns a trainer object that applies post processing to the decision_function
                  objects created by the given trainer object with the goal of creating
                  decision_function objects with fewer support vectors.
                - The reduced decision functions that are output will have at most
                  num_sv support vectors.
                - the gradient based optimization will continue until the change in the
                  objective function is less than eps.  So smaller values of eps will
                  give better results but take longer to compute.
        !*/

        template <
            typename in_sample_vector_type,
            typename in_scalar_vector_type
            >
        const decision_function<kernel_type> train (
            const in_sample_vector_type& x,
            const in_scalar_vector_type& y
        ) const;
        /*!
            ensures
                - trains a decision_function using the trainer that was supplied to
                  this object's constructor and then finds a reduced representation
                  for it and returns the reduced version.  
            throws
                - std::bad_alloc
                - any exceptions thrown by the trainer_type object
        !*/

    }; 

// ----------------------------------------------------------------------------------------

    template <
        typename trainer_type
        >
    const reduced_decision_function_trainer2<trainer_type> reduced2 (
        const trainer_type& trainer,
        const unsigned long num_sv,
        double eps = 1e-3
    ) { return reduced_decision_function_trainer2<trainer_type>(trainer, num_sv, eps); }
    /*!
        requires
            - num_sv > 0
            - trainer_type == some kind of batch trainer object that creates decision_function
              objects (e.g. svm_nu_trainer)
            - kernel_derivative<trainer_type::kernel_type> is defined
            - eps > 0
        ensures
            - returns a reduced_decision_function_trainer2 object that has been
              instantiated with the given arguments.
    !*/

// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------

}

#endif // DLIB_REDUCED_TRAINERs_ABSTRACT_

